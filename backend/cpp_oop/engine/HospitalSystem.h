#ifndef HOSPITAL_SYSTEM_H
#define HOSPITAL_SYSTEM_H

#include "../models/User.h"
#include "../models/Roles.h"
#include "../models/Patient.h"

#include <queue>
#include <unordered_map>
#include <stack>
#include <vector>
#include <memory>
#include <string>

using namespace std;

struct AuditLogEntry {
    string timestamp;
    string actor;
    string action;
};

class HospitalSystem {
private:
    priority_queue<Patient> triageQueue;
    unordered_map<string, Patient> patientRegistry;
    stack<AuditLogEntry> auditTrail;
    vector<shared_ptr<User>> staffMembers;
    int tokenCounter;

    string generateNextToken();
    string getCurrentTimeStr() const;

public:
    HospitalSystem();

    void seedInitialStaff();

    string checkInPatient(const shared_ptr<User>& actor,
                          const string& name, int age,
                          const string& gender, const string& dept,
                          const string& docId, ClinicalVitals vitals,
                          const vector<string>& risks, int waitMins = 0);

    void addPatientDirect(const Patient& p);

    bool callNextPatient(const shared_ptr<User>& actor, Patient& outPatient);

    bool findPatientByToken(const string& token, Patient& outPatient) const;

    void simulateTimeAdvance(int minutes);

    void logAction(const string& who, const string& action);

    void printAuditStack(int maxEntries = 5);

    size_t getQueueSize() const;
    const vector<shared_ptr<User>>& getStaff() const;
};

#endif
