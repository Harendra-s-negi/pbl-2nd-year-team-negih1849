#include "AppointmentWorkflow.h"

#include <cctype>
#include <sstream>
#include <stdexcept>

namespace {
std::string textColumn(sqlite3_stmt* statement, int index) {
    const unsigned char* value = sqlite3_column_text(statement, index);
    return value ? reinterpret_cast<const char*>(value) : "";
}

void bindText(sqlite3_stmt* statement, int index, const std::string& value) {
    sqlite3_bind_text(statement, index, value.c_str(), -1, SQLITE_TRANSIENT);
}

bool prepare(sqlite3* db, const char* sql, sqlite3_stmt** statement) {
    return sqlite3_prepare_v2(db, sql, -1, statement, nullptr) == SQLITE_OK;
}

AppointmentRecord readRecord(sqlite3_stmt* statement) {
    AppointmentRecord record;
    record.id = sqlite3_column_int(statement, 0);
    record.patientToken = textColumn(statement, 1);
    record.doctorId = textColumn(statement, 2);
    record.date = textColumn(statement, 3);
    record.time = textColumn(statement, 4);
    record.durationMinutes = sqlite3_column_int(statement, 5);
    record.reason = textColumn(statement, 6);
    record.status = textColumn(statement, 7);
    record.createdAt = textColumn(statement, 8);
    return record;
}

const char* SELECT_COLUMNS =
    "id, patient_token, doctor_id, appointment_date, appointment_time, "
    "duration_minutes, reason, status, created_at";
}

AppointmentWorkflow::AppointmentWorkflow(const std::string& databasePath)
    : db_(nullptr) {
    if (sqlite3_open(databasePath.c_str(), &db_) != SQLITE_OK) {
        std::string error = db_ ? sqlite3_errmsg(db_) : "Unable to open SQLite";
        if (db_) sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error(error);
    }
    sqlite3_busy_timeout(db_, 5000);
    sqlite3_exec(db_, "PRAGMA foreign_keys = ON;", nullptr, nullptr, nullptr);
    if (!initialize()) {
        std::string error = sqlite3_errmsg(db_);
        sqlite3_close(db_);
        db_ = nullptr;
        throw std::runtime_error("Could not create appointment schema: " + error);
    }
}

AppointmentWorkflow::~AppointmentWorkflow() {
    if (db_) sqlite3_close(db_);
}

bool AppointmentWorkflow::execute(const char* sql) {
    char* error = nullptr;
    int result = sqlite3_exec(db_, sql, nullptr, nullptr, &error);
    if (error) sqlite3_free(error);
    return result == SQLITE_OK;
}

bool AppointmentWorkflow::initialize() {
    std::lock_guard<std::mutex> lock(mutex_);
    return execute(R"SQL(
        CREATE TABLE IF NOT EXISTS appointments (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            patient_token TEXT NOT NULL,
            doctor_id TEXT NOT NULL,
            appointment_date TEXT NOT NULL,
            appointment_time TEXT NOT NULL,
            duration_minutes INTEGER NOT NULL CHECK(duration_minutes BETWEEN 10 AND 240),
            reason TEXT NOT NULL,
            status TEXT NOT NULL DEFAULT 'BOOKED'
                CHECK(status IN ('BOOKED','CANCELLED','COMPLETED')),
            created_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT NOT NULL DEFAULT CURRENT_TIMESTAMP
        );
        CREATE INDEX IF NOT EXISTS idx_appointment_patient
            ON appointments(patient_token);
        CREATE INDEX IF NOT EXISTS idx_appointment_doctor_date
            ON appointments(doctor_id, appointment_date);
        CREATE INDEX IF NOT EXISTS idx_appointment_status
            ON appointments(status);
    )SQL");
}

bool AppointmentWorkflow::validDate(const std::string& date) {
    if (date.size() != 10 || date[4] != '-' || date[7] != '-') return false;
    for (std::size_t i = 0; i < date.size(); ++i) {
        if (i == 4 || i == 7) continue;
        if (!std::isdigit(static_cast<unsigned char>(date[i]))) return false;
    }
    int year = std::stoi(date.substr(0, 4));
    int month = std::stoi(date.substr(5, 2));
    int day = std::stoi(date.substr(8, 2));
    if (year < 2020 || month < 1 || month > 12 || day < 1) return false;
    const int days[] = {31,28,31,30,31,30,31,31,30,31,30,31};
    bool leap = year % 400 == 0 || (year % 4 == 0 && year % 100 != 0);
    int maximum = days[month - 1] + ((month == 2 && leap) ? 1 : 0);
    return day <= maximum;
}

bool AppointmentWorkflow::validTime(const std::string& time) {
    if (time.size() != 5 || time[2] != ':') return false;
    for (int i : {0, 1, 3, 4})
        if (!std::isdigit(static_cast<unsigned char>(time[i]))) return false;
    int hour = std::stoi(time.substr(0, 2));
    int minute = std::stoi(time.substr(3, 2));
    return hour >= 0 && hour <= 23 && minute >= 0 && minute <= 59;
}

int AppointmentWorkflow::minutesFromTime(const std::string& time) {
    if (!validTime(time)) return -1;
    return std::stoi(time.substr(0, 2)) * 60 +
           std::stoi(time.substr(3, 2));
}

bool AppointmentWorkflow::patientExists(const std::string& token) {
    sqlite3_stmt* statement = nullptr;
    if (!prepare(db_, "SELECT 1 FROM patients WHERE token=? LIMIT 1;", &statement))
        return false;
    bindText(statement, 1, token);
    bool found = sqlite3_step(statement) == SQLITE_ROW;
    sqlite3_finalize(statement);
    return found;
}

bool AppointmentWorkflow::doctorExists(const std::string& doctorId) {
    sqlite3_stmt* statement = nullptr;
    if (!prepare(db_, "SELECT 1 FROM doctors WHERE id=? LIMIT 1;", &statement))
        return false;
    bindText(statement, 1, doctorId);
    bool found = sqlite3_step(statement) == SQLITE_ROW;
    sqlite3_finalize(statement);
    return found;
}

bool AppointmentWorkflow::conflicts(const std::string& doctorId,
                                    const std::string& date,
                                    const std::string& time,
                                    int duration,
                                    int ignoredId) {
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "SELECT appointment_time,duration_minutes FROM appointments "
        "WHERE doctor_id=? AND appointment_date=? AND status='BOOKED' AND id<>?;";
    if (!prepare(db_, sql, &statement)) return true;
    bindText(statement, 1, doctorId);
    bindText(statement, 2, date);
    sqlite3_bind_int(statement, 3, ignoredId);

    int requestedStart = minutesFromTime(time);
    int requestedEnd = requestedStart + duration;
    bool conflict = false;

    while (sqlite3_step(statement) == SQLITE_ROW) {
        std::string existingTime = textColumn(statement, 0);
        int existingDuration = sqlite3_column_int(statement, 1);
        int existingStart = minutesFromTime(existingTime);
        int existingEnd = existingStart + existingDuration;
        if (requestedStart < existingEnd && existingStart < requestedEnd) {
            conflict = true;
            break;
        }
    }
    sqlite3_finalize(statement);
    return conflict;
}

WorkflowResult AppointmentWorkflow::book(const std::string& patientToken,
                                           const std::string& doctorId,
                                           const std::string& date,
                                           const std::string& time,
                                           const std::string& reason,
                                           int durationMinutes) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (patientToken.empty() || doctorId.empty() || reason.empty())
        return {false, "Patient token, doctor ID and reason are required.", 0};
    if (!validDate(date)) return {false, "Date must be YYYY-MM-DD.", 0};
    if (!validTime(time)) return {false, "Time must be HH:MM (24-hour).", 0};
    if (durationMinutes < 10 || durationMinutes > 240)
        return {false, "Duration must be from 10 to 240 minutes.", 0};
    if (minutesFromTime(time) + durationMinutes > 1440)
        return {false, "Appointment cannot continue past midnight.", 0};
    if (!patientExists(patientToken))
        return {false, "Patient token does not exist in patients table.", 0};
    if (!doctorExists(doctorId))
        return {false, "Doctor ID does not exist in doctors table.", 0};
    if (conflicts(doctorId, date, time, durationMinutes))
        return {false, "Doctor already has an overlapping appointment.", 0};

    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "INSERT INTO appointments(patient_token,doctor_id,appointment_date,"
        "appointment_time,duration_minutes,reason,status) "
        "VALUES(?,?,?,?,?,?,'BOOKED');";
    if (!prepare(db_, sql, &statement))
        return {false, sqlite3_errmsg(db_), 0};

    bindText(statement, 1, patientToken);
    bindText(statement, 2, doctorId);
    bindText(statement, 3, date);
    bindText(statement, 4, time);
    sqlite3_bind_int(statement, 5, durationMinutes);
    bindText(statement, 6, reason);

    bool success = sqlite3_step(statement) == SQLITE_DONE;
    int id = success ? static_cast<int>(sqlite3_last_insert_rowid(db_)) : 0;
    std::string message = success ? "Appointment booked." : sqlite3_errmsg(db_);
    sqlite3_finalize(statement);
    return {success, message, id};
}

WorkflowResult AppointmentWorkflow::cancel(int appointmentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (appointmentId <= 0) return {false, "Invalid appointment ID.", 0};
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "UPDATE appointments SET status='CANCELLED',updated_at=CURRENT_TIMESTAMP "
        "WHERE id=? AND status='BOOKED';";
    if (!prepare(db_, sql, &statement))
        return {false, sqlite3_errmsg(db_), 0};
    sqlite3_bind_int(statement, 1, appointmentId);
    bool success = sqlite3_step(statement) == SQLITE_DONE &&
                   sqlite3_changes(db_) == 1;
    sqlite3_finalize(statement);
    return success
        ? WorkflowResult{true, "Appointment cancelled.", appointmentId}
        : WorkflowResult{false, "Appointment not found or not BOOKED.", 0};
}

WorkflowResult AppointmentWorkflow::complete(int appointmentId) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (appointmentId <= 0) return {false, "Invalid appointment ID.", 0};
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "UPDATE appointments SET status='COMPLETED',updated_at=CURRENT_TIMESTAMP "
        "WHERE id=? AND status='BOOKED';";
    if (!prepare(db_, sql, &statement))
        return {false, sqlite3_errmsg(db_), 0};
    sqlite3_bind_int(statement, 1, appointmentId);
    bool success = sqlite3_step(statement) == SQLITE_DONE &&
                   sqlite3_changes(db_) == 1;
    sqlite3_finalize(statement);
    return success
        ? WorkflowResult{true, "Appointment completed.", appointmentId}
        : WorkflowResult{false, "Appointment not found or not BOOKED.", 0};
}

WorkflowResult AppointmentWorkflow::reschedule(int appointmentId,
                                                const std::string& date,
                                                const std::string& time,
                                                int durationMinutes) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (appointmentId <= 0) return {false, "Invalid appointment ID.", 0};
    if (!validDate(date) || !validTime(time))
        return {false, "Use valid YYYY-MM-DD and HH:MM values.", 0};
    if (durationMinutes < 10 || durationMinutes > 240)
        return {false, "Duration must be from 10 to 240 minutes.", 0};
    if (minutesFromTime(time) + durationMinutes > 1440)
        return {false, "Appointment cannot continue past midnight.", 0};

    sqlite3_stmt* find = nullptr;
    const char* findSql =
        "SELECT doctor_id,status FROM appointments WHERE id=?;";
    if (!prepare(db_, findSql, &find))
        return {false, sqlite3_errmsg(db_), 0};
    sqlite3_bind_int(find, 1, appointmentId);
    bool found = sqlite3_step(find) == SQLITE_ROW;
    std::string doctorId = found ? textColumn(find, 0) : "";
    std::string status = found ? textColumn(find, 1) : "";
    sqlite3_finalize(find);

    if (!found) return {false, "Appointment not found.", 0};
    if (status != "BOOKED")
        return {false, "Only BOOKED appointments can be rescheduled.", 0};
    if (conflicts(doctorId, date, time, durationMinutes, appointmentId))
        return {false, "Doctor already has an overlapping appointment.", 0};

    sqlite3_stmt* update = nullptr;
    const char* sql =
        "UPDATE appointments SET appointment_date=?,appointment_time=?,"
        "duration_minutes=?,updated_at=CURRENT_TIMESTAMP "
        "WHERE id=? AND status='BOOKED';";
    if (!prepare(db_, sql, &update))
        return {false, sqlite3_errmsg(db_), 0};
    bindText(update, 1, date);
    bindText(update, 2, time);
    sqlite3_bind_int(update, 3, durationMinutes);
    sqlite3_bind_int(update, 4, appointmentId);
    bool success = sqlite3_step(update) == SQLITE_DONE &&
                   sqlite3_changes(db_) == 1;
    sqlite3_finalize(update);
    return success
        ? WorkflowResult{true, "Appointment rescheduled.", appointmentId}
        : WorkflowResult{false, "Reschedule failed.", 0};
}

bool AppointmentWorkflow::get(int appointmentId, AppointmentRecord& result) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::string sql = std::string("SELECT ") + SELECT_COLUMNS +
                      " FROM appointments WHERE id=?;";
    sqlite3_stmt* statement = nullptr;
    if (!prepare(db_, sql.c_str(), &statement)) return false;
    sqlite3_bind_int(statement, 1, appointmentId);
    bool found = sqlite3_step(statement) == SQLITE_ROW;
    if (found) result = readRecord(statement);
    sqlite3_finalize(statement);
    return found;
}

std::vector<AppointmentRecord> AppointmentWorkflow::byPatient(
    const std::string& token) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<AppointmentRecord> records;
    std::string sql = std::string("SELECT ") + SELECT_COLUMNS +
        " FROM appointments WHERE patient_token=? "
        "ORDER BY appointment_date DESC,appointment_time DESC;";
    sqlite3_stmt* statement = nullptr;
    if (!prepare(db_, sql.c_str(), &statement)) return records;
    bindText(statement, 1, token);
    while (sqlite3_step(statement) == SQLITE_ROW)
        records.push_back(readRecord(statement));
    sqlite3_finalize(statement);
    return records;
}

std::vector<AppointmentRecord> AppointmentWorkflow::byDoctor(
    const std::string& doctorId, const std::string& date) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<AppointmentRecord> records;
    std::string sql = std::string("SELECT ") + SELECT_COLUMNS +
        " FROM appointments WHERE doctor_id=? AND appointment_date=? "
        "ORDER BY appointment_time;";
    sqlite3_stmt* statement = nullptr;
    if (!prepare(db_, sql.c_str(), &statement)) return records;
    bindText(statement, 1, doctorId);
    bindText(statement, 2, date);
    while (sqlite3_step(statement) == SQLITE_ROW)
        records.push_back(readRecord(statement));
    sqlite3_finalize(statement);
    return records;
}

std::vector<AppointmentRecord> AppointmentWorkflow::byDate(
    const std::string& date) {
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<AppointmentRecord> records;
    std::string sql = std::string("SELECT ") + SELECT_COLUMNS +
        " FROM appointments WHERE appointment_date=? "
        "ORDER BY appointment_time,doctor_id;";
    sqlite3_stmt* statement = nullptr;
    if (!prepare(db_, sql.c_str(), &statement)) return records;
    bindText(statement, 1, date);
    while (sqlite3_step(statement) == SQLITE_ROW)
        records.push_back(readRecord(statement));
    sqlite3_finalize(statement);
    return records;
}

AppointmentSummary AppointmentWorkflow::summary(const std::string& date) {
    std::lock_guard<std::mutex> lock(mutex_);
    AppointmentSummary result;
    sqlite3_stmt* statement = nullptr;
    const char* sql =
        "SELECT COUNT(*),"
        "SUM(CASE WHEN status='BOOKED' THEN 1 ELSE 0 END),"
        "SUM(CASE WHEN status='CANCELLED' THEN 1 ELSE 0 END),"
        "SUM(CASE WHEN status='COMPLETED' THEN 1 ELSE 0 END) "
        "FROM appointments WHERE appointment_date=?;";
    if (!prepare(db_, sql, &statement)) return result;
    bindText(statement, 1, date);
    if (sqlite3_step(statement) == SQLITE_ROW) {
        result.total = sqlite3_column_int(statement, 0);
        result.booked = sqlite3_column_int(statement, 1);
        result.cancelled = sqlite3_column_int(statement, 2);
        result.completed = sqlite3_column_int(statement, 3);
    }
    sqlite3_finalize(statement);
    return result;
}
