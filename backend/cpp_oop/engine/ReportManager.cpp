#include "ReportManager.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

bool ReportManager::exportDischargeSummary(const Patient& p, const string& doctorName, const string& filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open file for writing: " << filename << "\n";
        return false;
    }

    outFile << "========================================================\n";
    outFile << "          SMART HOSPITAL DISCHARGE SUMMARY CARD        \n";
    outFile << "========================================================\n";
    outFile << left << setw(20) << "Patient Name:" << p.getName() << "\n";
    outFile << left << setw(20) << "Patient Token:" << p.getToken() << "\n";
    outFile << left << setw(20) << "Age:" << p.getAge() << "\n";
    outFile << left << setw(20) << "Department:" << p.getDepartment() << "\n";
    outFile << left << setw(20) << "Attending Doctor:" << doctorName << "\n";
    outFile << left << setw(20) << "Wait Time:" << p.getWaitMinutes() << " mins\n";
    outFile << left << setw(20) << "Clinical Score:" << p.getEffectiveScore() << " / 100\n";
    outFile << left << setw(20) << "Discharge Status:" << "CONSULTATION COMPLETED\n";
    outFile << "--------------------------------------------------------\n";
    outFile << "Clinical Advice:\n";
    outFile << " - Continue prescribed medication as scheduled.\n";
    outFile << " - Follow up in 7 days or report to ER if vitals deviate.\n";
    outFile << "========================================================\n";

    outFile.close();
    return true;
}

bool ReportManager::exportQueueAuditReport(const vector<Patient>& patients, const string& filename) {
    ofstream outFile(filename);
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open file for writing: " << filename << "\n";
        return false;
    }

    outFile << "========================================================================\n";
    outFile << "             SMART HOSPITAL DAILY TRIAGE AUDIT REPORT                   \n";
    outFile << "========================================================================\n";
    outFile << left << setw(10) << "Token" 
            << setw(18) << "Name" 
            << setw(6) << "Age" 
            << setw(18) << "Department" 
            << setw(10) << "Score" 
            << setw(12) << "Wait Time\n";
    outFile << "------------------------------------------------------------------------\n";

    int totalWait = 0;
    int criticalCount = 0;

    for (const auto& p : patients) {
        outFile << left << setw(10) << p.getToken()
                << setw(18) << p.getName()
                << setw(6) << p.getAge()
                << setw(18) << p.getDepartment()
                << setw(10) << p.getEffectiveScore()
                << setw(12) << to_string(p.getWaitMinutes()) + "m\n";

        totalWait += p.getWaitMinutes();
        if (p.getEffectiveScore() >= 50) {
            criticalCount++;
        }
    }

    outFile << "========================================================================\n";
    outFile << "Summary Statistics:\n";
    outFile << " - Total Patients Processed: " << patients.size() << "\n";
    outFile << " - Critical Emergency Cases (Score >= 50): " << criticalCount << "\n";
    double avgWait = patients.empty() ? 0.0 : (double)totalWait / patients.size();
    outFile << " - Average Queue Wait Time: " << fixed << setprecision(1) << avgWait << " mins\n";
    outFile << "========================================================================\n";

    outFile.close();
    return true;
}

string ReportManager::readReportFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        return "Error: Could not open file: " + filename;
    }

    stringstream buffer;
    buffer << inFile.rdbuf();
    inFile.close();
    return buffer.str();
}
