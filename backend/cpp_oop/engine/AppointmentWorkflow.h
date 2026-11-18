#ifndef APPOINTMENT_WORKFLOW_H
#define APPOINTMENT_WORKFLOW_H

#include <sqlite3.h>
#include <mutex>
#include <string>
#include <vector>

struct AppointmentRecord {
    int id = 0;
    std::string patientToken;
    std::string doctorId;
    std::string date;
    std::string time;
    int durationMinutes = 30;
    std::string reason;
    std::string status;
    std::string createdAt;
};

struct WorkflowResult {
    bool success = false;
    std::string message;
    int recordId = 0;
};

struct AppointmentSummary {
    int total = 0;
    int booked = 0;
    int cancelled = 0;
    int completed = 0;
};

class AppointmentWorkflow {
public:
    explicit AppointmentWorkflow(const std::string& databasePath);
    ~AppointmentWorkflow();

    AppointmentWorkflow(const AppointmentWorkflow&) = delete;
    AppointmentWorkflow& operator=(const AppointmentWorkflow&) = delete;

    bool initialize();
    WorkflowResult book(const std::string& patientToken,
                        const std::string& doctorId,
                        const std::string& date,
                        const std::string& time,
                        const std::string& reason,
                        int durationMinutes = 30);
    WorkflowResult cancel(int appointmentId);
    WorkflowResult complete(int appointmentId);
    WorkflowResult reschedule(int appointmentId,
                              const std::string& date,
                              const std::string& time,
                              int durationMinutes = 30);
    bool get(int appointmentId, AppointmentRecord& result);
    std::vector<AppointmentRecord> byPatient(const std::string& token);
    std::vector<AppointmentRecord> byDoctor(const std::string& doctorId,
                                             const std::string& date);
    std::vector<AppointmentRecord> byDate(const std::string& date);
    AppointmentSummary summary(const std::string& date);

    static bool validDate(const std::string& date);
    static bool validTime(const std::string& time);
    static int minutesFromTime(const std::string& time);

private:
    sqlite3* db_;
    std::mutex mutex_;

    bool execute(const char* sql);
    bool patientExists(const std::string& token);
    bool doctorExists(const std::string& doctorId);
    bool conflicts(const std::string& doctorId,
                   const std::string& date,
                   const std::string& time,
                   int duration,
                   int ignoredId = 0);
};

#endif
