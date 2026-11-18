#include "c_ds/heap.h"
#include "c_ds/linked_list.h"
#include "c_ds/hash_table.h"
#include "c_ds/queue.h"
#include "c_ds/avl_tree.h"

#include "cpp_oop/models/User.h"
#include "cpp_oop/models/Roles.h"
#include "cpp_oop/models/Patient.h"
#include "cpp_oop/engine/HospitalSystem.h"
#include "cpp_oop/engine/ReportManager.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <memory>

using namespace std;

void runDataStructuresDemo() {
    cout << "=========================================\n";
    cout << " 1. TESTING PURE C DATA STRUCTURES\n";
    cout << "=========================================\n\n";

    cout << "--- A. Binary Max Heap ---\n";
    C_MaxHeap* heap = heap_create(4);

    C_PatientRecord p1 = {"PT-201", "T-1001", "Asha Rao", 34, 45, 55, 50};
    C_PatientRecord p2 = {"PT-202", "T-1002", "Imran Qureshi", 61, 72, 6, 72};
    C_PatientRecord p3 = {"PT-203", "T-1003", "Fatima Sheikh", 28, 30, 80, 38};
    C_PatientRecord p4 = {"PT-204", "T-1004", "Vikram Nair", 45, 58, 22, 60};
    C_PatientRecord p5 = {"PT-205", "T-1005", "Leela Menon", 72, 85, 15, 86};

    cout << "Inserting 5 patient records into heap...\n";
    heap_insert(heap, p1);
    heap_insert(heap, p2);
    heap_insert(heap, p3);
    heap_insert(heap, p4);
    heap_insert(heap, p5);

    heap_print(heap);

    C_PatientRecord topPatient;
    if (heap_peek(heap, &topPatient)) {
        cout << "\nHeap Peek (Highest Priority): " << topPatient.name 
             << " (" << topPatient.token << ") with Score: " << topPatient.effectiveScore << "\n";
    }

    cout << "\nExtracting patients from Max Heap in order of priority:\n";
    while (heap->size > 0) {
        C_PatientRecord next;
        heap_extract_max(heap, &next);
        cout << "   Token: " << next.token << " | " << setw(15) << left << next.name 
             << " | Score: " << next.effectiveScore << "\n";
    }
    heap_destroy(heap);

    cout << "\n--- B. Doubly Linked List ---\n";
    C_DoublyLinkedList* list = list_create();
    list_append(list, p1);
    list_append(list, p2);
    list_append(list, p3);
    list_append(list, p4);
    list_print(list);

    cout << "\nRemoving patient T-1002 from list...\n";
    C_PatientRecord removed;
    if (list_remove_by_token(list, "T-1002", &removed)) {
        cout << "Removed: " << removed.name << " (" << removed.token << ")\n";
    }
    list_print(list);
    list_destroy(list);

    cout << "\n--- C. Hash Table ---\n";
    C_HashTable* table = hash_table_create();
    hash_table_put(table, "T-1001", p1);
    hash_table_put(table, "T-1002", p2);
    hash_table_put(table, "T-1003", p3);
    hash_table_put(table, "T-1004", p4);
    hash_table_put(table, "T-1005", p5);
    hash_table_print(table);

    cout << "\nLooking up token T-1004 in hash table...\n";
    C_PatientRecord found;
    if (hash_table_get(table, "T-1004", &found)) {
        cout << "Found: " << found.name << " | Age: " << found.age 
             << " | Score: " << found.effectiveScore << "\n";
    }
    hash_table_destroy(table);

    cout << "\n--- D. Circular Queue ---\n";
    C_CircularQueue* q = queue_create(5);
    queue_enqueue(q, p1);
    queue_enqueue(q, p2);
    queue_enqueue(q, p3);
    queue_print(q);

    C_PatientRecord dequeued;
    if (queue_dequeue(q, &dequeued)) {
        cout << "Dequeued patient: " << dequeued.name << " (" << dequeued.token << ")\n";
    }
    queue_print(q);
    queue_destroy(q);

    cout << "\n--- E. AVL Tree ---\n";
    C_AVLTree* avl = avl_create();
    avl_insert(avl, p3);
    avl_insert(avl, p1);
    avl_insert(avl, p4);
    avl_insert(avl, p2);
    avl_insert(avl, p5);
    avl_print(avl);

    C_PatientRecord avlFound;
    if (avl_search_by_token(avl, "T-1005", &avlFound)) {
        cout << "Searching AVL for T-1005: Found " << avlFound.name 
             << " with Score: " << avlFound.effectiveScore << "\n";
    }
    avl_destroy(avl);
}

void runOOPDemo() {
    cout << "\n=========================================\n";
    cout << " 2. TESTING C++ OBJECT-ORIENTED PROGRAMMING\n";
    cout << "=========================================\n\n";

    cout << "--- A. Inheritance and Polymorphism ---\n";
    vector<shared_ptr<User>> users;
    users.push_back(make_shared<Receptionist>("U-01", "P. Menon", "Front Desk 1"));
    users.push_back(make_shared<Doctor>("D-101", "Dr. A. Sharma", "Cardiology", "Room 302"));
    users.push_back(make_shared<Administrator>("ADM-01", "S. Kulkarni", "Director"));

    for (const auto& user : users) {
        user->printDashboardSummary();
        cout << "   Can check in: " << (user->hasPermission(Permission::CHECKIN_PATIENT) ? "YES" : "NO") << "\n";
        cout << "   Can call:     " << (user->hasPermission(Permission::CALL_PATIENT) ? "YES" : "NO") << "\n";
    }

    cout << "\n--- B. Operator Overloading and Encapsulation ---\n";
    ClinicalVitals v1{110, 145, 94, 8, 22};
    Patient patientA("PT-1001", "T-101", "Ramesh Sen", 68, "M", "Cardiology", "D-101", v1, {"cardiac"}, 15);

    ClinicalVitals v2{78, 120, 99, 3, 5};
    Patient patientB("PT-1002", "T-102", "Sunita Devi", 25, "F", "General Medicine", "D-104", v2, {}, 60);

    cout << "Patient A: " << patientA << "\n";
    cout << "Patient B: " << patientB << "\n";

    cout << "\nComparison (patientB < patientA): "
         << (patientB < patientA ? "TRUE (Patient A has higher priority)" : "FALSE") << "\n";
}

void runHospitalSimulation() {
    cout << "\n=========================================\n";
    cout << " 3. FULL HOSPITAL QUEUE SIMULATION\n";
    cout << "=========================================\n\n";

    HospitalSystem hospital;

    auto receptionist = hospital.getStaff()[0];
    auto doctor = hospital.getStaff()[1];

    cout << "Checking in patients...\n";
    ClinicalVitals vit1{75, 120, 98, 4, 10};
    string t1 = hospital.checkInPatient(receptionist, "Asha Rao", 34, "F", "General Medicine", "D-104", vit1, {"diabetic"}, 50);

    ClinicalVitals vit2{130, 165, 89, 9, 25};
    string t2 = hospital.checkInPatient(receptionist, "Imran Qureshi", 61, "M", "Cardiology", "D-101", vit2, {"cardiac"}, 5);

    ClinicalVitals vit3{82, 115, 97, 5, 8};
    string t3 = hospital.checkInPatient(receptionist, "Fatima Sheikh", 28, "F", "General Medicine", "D-104", vit3, {"pregnant"}, 45);

    ClinicalVitals vit4{140, 175, 85, 10, 25};
    string t4 = hospital.checkInPatient(receptionist, "Vikram Nair", 45, "M", "Emergency", "D-101", vit4, {}, 2);

    cout << "Total waiting in queue: " << hospital.getQueueSize() << "\n\n";

    cout << "Searching for patient T-1002...\n";
    Patient searched;
    if (hospital.findPatientByToken(t2, searched)) {
        cout << "Found: " << searched << "\n\n";
    }

    cout << "Doctor calling patients by priority:\n";
    Patient called;
    int turn = 1;
    while (hospital.callNextPatient(doctor, called)) {
        cout << "   Call #" << turn++ << ": " << called.getName() 
             << " (" << called.getToken() << ") - Score: " 
             << called.getEffectiveScore() << "\n";
    }

    hospital.printAuditStack(5);
}

void runFileHandlingDemo() {
    cout << "\n=========================================\n";
    cout << " 4. TESTING FILE STREAMS & REPORTS (FSTREAM)\n";
    cout << "=========================================\n\n";

    ClinicalVitals v{120, 150, 92, 7, 20};
    Patient p("PT-205", "T-1005", "Leela Menon", 72, "F", "Cardiology", "D-101", v, {"cardiac"}, 15);

    string dischargeFile = "discharge_T1005.txt";
    cout << "Exporting patient discharge summary to " << dischargeFile << "...\n";
    if (ReportManager::exportDischargeSummary(p, "Dr. A. Sharma", dischargeFile)) {
        cout << "Discharge summary successfully written using ofstream.\n\n";
    }

    cout << "Reading discharge summary back using ifstream:\n";
    string reportContent = ReportManager::readReportFile(dischargeFile);
    cout << reportContent << "\n";
}

int main() {
    cout << "\n=========================================\n";
    cout << "   SMART HOSPITAL MANAGEMENT SYSTEM\n";
    cout << "=========================================\n\n";

    runDataStructuresDemo();
    runOOPDemo();
    runHospitalSimulation();
    runFileHandlingDemo();

    cout << "\nAll tests completed successfully.\n";
    return 0;
}
