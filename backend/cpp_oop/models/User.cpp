#include "User.h"

User::User(string userId, string userName, Role userRole) {
    id = userId;
    name = userName;
    role = userRole;
}

User::~User() {}

const string& User::getId() const {
    return id;
}

const string& User::getName() const {
    return name;
}

Role User::getRole() const {
    return role;
}
