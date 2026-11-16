#include "Roles.h"
#include <iostream>

using namespace std;

Receptionist::Receptionist(string userId, string userName, string desk)
    : User(userId, userName, Role::RECEPTIONIST) {
    deskId = desk;
}

string Receptionist::getRoleName() const {
    return "Receptionist";
}

bool Receptionist::hasPermission(Permission permission) const {
    return permission == Permission::CHECKIN_PATIENT || permission == Permission::VIEW_AUDIT_LOG;
}

void Receptionist::printDashboardSummary() const {
    cout << "[Reception Portal] User: " << name << " (" << id << ") at Desk " << deskId << "\n";
}

const string& Receptionist::getDeskId() const {
    return deskId;
}

Doctor::Doctor(string userId, string userName, string dept, string room)
    : User(userId, userName, Role::DOCTOR) {
    department = dept;
    roomNumber = room;
}

string Doctor::getRoleName() const {
    return "Doctor";
}

bool Doctor::hasPermission(Permission permission) const {
    return permission == Permission::CALL_PATIENT || permission == Permission::COMPLETE_CONSULTATION;
}

void Doctor::printDashboardSummary() const {
    cout << "[Doctor Portal] Dr. " << name << " | Dept: " << department << " | Room: " << roomNumber << "\n";
}

const string& Doctor::getDepartment() const {
    return department;
}

const string& Doctor::getRoomNumber() const {
    return roomNumber;
}

Administrator::Administrator(string userId, string userName, string level)
    : User(userId, userName, Role::ADMINISTRATOR) {
    adminLevel = level;
}

string Administrator::getRoleName() const {
    return "Administrator";
}

bool Administrator::hasPermission(Permission) const {
    return true;
}

void Administrator::printDashboardSummary() const {
    cout << "[Admin Portal] Administrator: " << name << " (Level: " << adminLevel << ")\n";
}

PatientUser::PatientUser(string userId, string userName, string patientToken)
    : User(userId, userName, Role::PATIENT) {
    token = patientToken;
}

string PatientUser::getRoleName() const {
    return "Patient";
}

bool PatientUser::hasPermission(Permission permission) const {
    return permission == Permission::TRACK_OWN_QUEUE;
}

void PatientUser::printDashboardSummary() const {
    cout << "[Patient Portal] " << name << " | Token: " << token << "\n";
}

const string& PatientUser::getToken() const {
    return token;
}
