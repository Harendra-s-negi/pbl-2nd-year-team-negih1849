#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
#include <iostream>

using namespace std;

enum class Role {
    RECEPTIONIST,
    DOCTOR,
    ADMINISTRATOR,
    PATIENT
};

enum class Permission {
    CHECKIN_PATIENT,
    CALL_PATIENT,
    COMPLETE_CONSULTATION,
    MODIFY_SCORING_CONFIG,
    VIEW_AUDIT_LOG,
    TRACK_OWN_QUEUE
};

class User {
protected:
    string id;
    string name;
    Role role;

public:
    User(string userId, string userName, Role userRole);
    virtual ~User();

    virtual string getRoleName() const = 0;
    virtual bool hasPermission(Permission permission) const = 0;
    virtual void printDashboardSummary() const = 0;

    const string& getId() const;
    const string& getName() const;
    Role getRole() const;
};

#endif
