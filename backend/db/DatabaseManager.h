#ifndef DATABASE_MANAGER_H
#define DATABASE_MANAGER_H

#include <sqlite3.h>
#include <string>
#include <vector>
#include <mutex>
#include "../cpp_oop/models/Patient.h"

using namespace std;

class DatabaseManager {
private:
    sqlite3* db;
    string dbPath;
    mutex dbMutex;

    void executeRaw(const string& sql);

public:
    explicit DatabaseManager(string path = "hospital.db");
    ~DatabaseManager();

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    void initSchema();
    void seedDoctorsIfEmpty();

    bool savePatient(const Patient& p, const ClinicalVitals& v);
    bool updatePatientStatus(const string& token, const string& status);
    bool logAudit(const string& timestamp, const string& actor, const string& action);

    vector<Patient> loadWaitingPatients();
    int getTotalPatientsCount();
};

#endif
