#include "HospitalSystem.h"
#include <iostream>
#include <iomanip>
#include <stdexcept>

HospitalSystem::HospitalSystem() {
    tokenCounter = 1000;
    seedInitialStaff();
}

string HospitalSystem::generateNextToken() {
    tokenCounter++;
    return "T-" + to_string(tokenCounter);
}

string HospitalSystem::getCurrentTimeStr() const {
    return "10:30 AM";
}

void HospitalSystem::seedInitialStaff() {
    staffMembers.push_back(make_shared<Receptionist>("U-1", "P. Menon", "Desk-A"));
    staffMembers.push_back(make_shared<Doctor>("D-101", "Dr. A. Sharma", "Emergency Medicine", "Room 102"));
    staffMembers.push_back(make_shared<Doctor>("D-104", "Dr. R. Iyer", "General Medicine", "Room 105"));
    staffMembers.push_back(make_shared<Administrator>("ADM-01", "S. Kulkarni", "Director"));
}

string HospitalSystem::checkInPatient(const shared_ptr<User>& actor,
                                     const string& name, int age,
                                     const string& gender, const string& dept,
                                     const string& docId, ClinicalVitals vitals,
                                     const vector<string>& risks, int waitMins) {
    if (!actor->hasPermission(Permission::CHECKIN_PATIENT)) {
        throw runtime_error("Access Denied: User does not have permission to check in patients.");
    }

    string token = generateNextToken();
    string pId = "PT-" + to_string(20000 + (tokenCounter % 1000));
    Patient p(pId, token, name, age, gender, dept, docId, vitals, risks, waitMins);

    triageQueue.push(p);
    patientRegistry.insert({token, p});

    logAction(actor->getName(), "Checked in " + name + " (" + token + ") with score " + to_string(p.getEffectiveScore()));
    return token;
}

void HospitalSystem::addPatientDirect(const Patient& p) {
    triageQueue.push(p);
    patientRegistry[p.getToken()] = p;
}

bool HospitalSystem::callNextPatient(const shared_ptr<User>& actor, Patient& outPatient) {
    if (!actor->hasPermission(Permission::CALL_PATIENT)) {
        throw runtime_error("Access Denied: User does not have permission to call patients.");
    }

    if (triageQueue.empty()) {
        return false;
    }

    outPatient = triageQueue.top();
    triageQueue.pop();

    outPatient.setStatus(PatientStatus::IN_TREATMENT);
    patientRegistry[outPatient.getToken()] = outPatient;

    logAction(actor->getName(), "Called patient: " + outPatient.getName() + " (" + outPatient.getToken() + ") [Score: " + to_string(outPatient.getEffectiveScore()) + "]");
    return true;
}

bool HospitalSystem::findPatientByToken(const string& token, Patient& outPatient) const {
    auto it = patientRegistry.find(token);
    if (it != patientRegistry.end()) {
        outPatient = it->second;
        return true;
    }
    return false;
}

void HospitalSystem::simulateTimeAdvance(int minutes) {
    vector<Patient> waitingPatients;
    while (!triageQueue.empty()) {
        Patient p = triageQueue.top();
        triageQueue.pop();
        p.addWaitTime(minutes);
        patientRegistry[p.getToken()] = p;
        waitingPatients.push_back(p);
    }

    for (const auto& p : waitingPatients) {
        triageQueue.push(p);
    }

    logAction("System", "Clock advanced by " + to_string(minutes) + " mins (Wait bonus updated)");
}

void HospitalSystem::logAction(const string& who, const string& action) {
    auditTrail.push({getCurrentTimeStr(), who, action});
}

void HospitalSystem::printAuditStack(int maxEntries) {
    cout << "\n   Audit Trail Stack (Most Recent First):\n";
    stack<AuditLogEntry> temp = auditTrail;
    int count = 0;
    while (!temp.empty() && count < maxEntries) {
        const auto& entry = temp.top();
        cout << "   " << entry.timestamp << " | " << left << setw(18) << entry.actor
             << " | " << entry.action << "\n";
        temp.pop();
        count++;
    }
}

size_t HospitalSystem::getQueueSize() const {
    return triageQueue.size();
}

const vector<shared_ptr<User>>& HospitalSystem::getStaff() const {
    return staffMembers;
}
