#ifndef ROLES_H
#define ROLES_H

#include "User.h"

class Receptionist : public User {
private:
    string deskId;

public:
    Receptionist(string userId, string userName, string desk);
    string getRoleName() const override;
    bool hasPermission(Permission permission) const override;
    void printDashboardSummary() const override;
    const string& getDeskId() const;
};

class Doctor : public User {
private:
    string department;
    string roomNumber;

public:
    Doctor(string userId, string userName, string dept, string room);
    string getRoleName() const override;
    bool hasPermission(Permission permission) const override;
    void printDashboardSummary() const override;
    const string& getDepartment() const;
    const string& getRoomNumber() const;
};

class Administrator : public User {
private:
    string adminLevel;

public:
    Administrator(string userId, string userName, string level = "SuperAdmin");
    string getRoleName() const override;
    bool hasPermission(Permission permission) const override;
    void printDashboardSummary() const override;
};

class PatientUser : public User {
private:
    string token;

public:
    PatientUser(string userId, string userName, string patientToken);
    string getRoleName() const override;
    bool hasPermission(Permission permission) const override;
    void printDashboardSummary() const override;
    const string& getToken() const;
};

#endif
