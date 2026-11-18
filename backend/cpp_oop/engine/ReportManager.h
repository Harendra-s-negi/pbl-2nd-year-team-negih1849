#ifndef REPORT_MANAGER_H
#define REPORT_MANAGER_H

#include "../models/Patient.h"
#include <string>
#include <vector>

using namespace std;

class ReportManager {
public:
    static bool exportDischargeSummary(const Patient& p, const string& doctorName, const string& filename);
    static bool exportQueueAuditReport(const vector<Patient>& patients, const string& filename);
    static string readReportFile(const string& filename);
};

#endif
