#include "Patient.h"
#include <iomanip>
#include <algorithm>

int Patient::globalArrivalCounter = 0;

Patient::Patient() {
    id = "";
    token = "";
    name = "";
    age = 0;
    gender = "";
    department = "";
    assignedDoctorId = "";
    vitals = {0, 0, 0, 0, 0};
    urgencyScore = 0;
    waitMinutes = 0;
    effectiveScore = 0;
    status = PatientStatus::WAITING;
    arrivalOrder = ++globalArrivalCounter;
}

Patient::Patient(string pId, string pToken, string pName, int pAge,
                 string pGender, string pDept, string pDocId,
                 ClinicalVitals v, vector<string> risks, int waitMins) {
    id = pId;
    token = pToken;
    name = pName;
    age = pAge;
    gender = pGender;
    department = pDept;
    assignedDoctorId = pDocId;
    vitals = v;
    riskFactors = risks;
    waitMinutes = waitMins;
    status = PatientStatus::WAITING;
    arrivalOrder = ++globalArrivalCounter;
    calculateScores();
}

void Patient::calculateScores() {
    int ageScore = (age > 65 || age < 5) ? 8 : 2;
    int riskScore = (riskFactors.size() * 3 > 10) ? 10 : (int)(riskFactors.size() * 3);
    urgencyScore = vitals.vitalScore + vitals.painLevel + ageScore + riskScore;
    if (urgencyScore > 80) urgencyScore = 80;

    int waitBonus = waitMinutes / 10;
    if (waitBonus > 20) waitBonus = 20;

    effectiveScore = urgencyScore + waitBonus;
    if (effectiveScore > 100) effectiveScore = 100;
}

void Patient::addWaitTime(int minutes) {
    if (minutes > 0 && status == PatientStatus::WAITING) {
        waitMinutes += minutes;
        calculateScores();
    }
}

bool Patient::operator<(const Patient& other) const {
    if (effectiveScore == other.effectiveScore) {
        return arrivalOrder > other.arrivalOrder;
    }
    return effectiveScore < other.effectiveScore;
}

bool Patient::operator==(const Patient& other) const {
    return token == other.token || id == other.id;
}

ostream& operator<<(ostream& os, const Patient& p) {
    string statusStr = (p.status == PatientStatus::WAITING) ? "Waiting" :
                       (p.status == PatientStatus::IN_TREATMENT) ? "In Treatment" : "Completed";

    os << left << "[" << setw(7) << p.token << "] "
       << setw(16) << p.name << " | Age: " << setw(2) << p.age
       << " | Dept: " << setw(18) << p.department
       << " | Score: " << setw(3) << p.effectiveScore
       << " (Base: " << setw(2) << p.urgencyScore << " + Wait: " << p.waitMinutes << "m)"
       << " | Status: " << statusStr;
    return os;
}

const string& Patient::getId() const { return id; }
const string& Patient::getToken() const { return token; }
const string& Patient::getName() const { return name; }
int Patient::getAge() const { return age; }
const string& Patient::getDepartment() const { return department; }
const string& Patient::getDoctorId() const { return assignedDoctorId; }
int Patient::getUrgencyScore() const { return urgencyScore; }
int Patient::getWaitMinutes() const { return waitMinutes; }
int Patient::getEffectiveScore() const { return effectiveScore; }
PatientStatus Patient::getStatus() const { return status; }
void Patient::setStatus(PatientStatus s) { status = s; }
const vector<string>& Patient::getRiskFactors() const { return riskFactors; }
