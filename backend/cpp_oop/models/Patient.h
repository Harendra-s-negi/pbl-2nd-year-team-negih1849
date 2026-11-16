#ifndef PATIENT_H
#define PATIENT_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

struct ClinicalVitals {
    int heartRate;
    int systolicBp;
    int oxygenSaturation;
    int painLevel;
    int vitalScore;
};

enum class PatientStatus {
    WAITING,
    IN_TREATMENT,
    COMPLETED
};

class Patient {
private:
    string id;
    string token;
    string name;
    int age;
    string gender;
    string department;
    string assignedDoctorId;
    vector<string> riskFactors;
    ClinicalVitals vitals;
    int urgencyScore;
    int waitMinutes;
    int effectiveScore;
    PatientStatus status;
    int arrivalOrder;

    static int globalArrivalCounter;

public:
    Patient();
    Patient(string pId, string pToken, string pName, int pAge,
            string pGender, string pDept, string pDocId,
            ClinicalVitals v, vector<string> risks, int waitMins = 0);

    void calculateScores();
    void addWaitTime(int minutes);

    bool operator<(const Patient& other) const;
    bool operator==(const Patient& other) const;

    friend ostream& operator<<(ostream& os, const Patient& p);

    const string& getId() const;
    const string& getToken() const;
    const string& getName() const;
    int getAge() const;
    const string& getDepartment() const;
    const string& getDoctorId() const;
    int getUrgencyScore() const;
    int getWaitMinutes() const;
    int getEffectiveScore() const;
    PatientStatus getStatus() const;
    void setStatus(PatientStatus s);
    const vector<string>& getRiskFactors() const;
};

#endif
