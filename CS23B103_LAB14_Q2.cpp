#include <bits/stdc++.h>
using namespace std;

class course {
private:
    string ccode;
    string cname;
    int cred;
    int cap;
    vector<string> prereq;
    char slot;
    int curSize;

public:
    course() : ccode(""), cname(""), cred(0), cap(0), prereq(), slot(' '), curSize(0) {}
    course(string code, string name, int Cred, int Cap, char Slot, vector<string> pre, int cur) 
        : ccode(code), cname(name), cred(Cred), cap(Cap), slot(Slot), prereq(pre), curSize(cur) {}

    bool checkCap() const {
        return curSize > 0;
    }

    void incCap() {
        curSize++;
    }
    void decCap() {
        curSize--;
    }

    bool hasCycle(map<string, course>& cMap, unordered_set<string>& visited, unordered_set<string>& recStack) {
        visited.insert(ccode);
        recStack.insert(ccode);

        for (const auto& prereqCode : prereq) {
            if (recStack.find(prereqCode) != recStack.end()) {
                return true; // Cycle detected
            }
            if (visited.find(prereqCode) == visited.end() && cMap[prereqCode].hasCycle(cMap, visited, recStack)) {
                return true;
            }
        }

        recStack.erase(ccode); // Remove from recursion stack
        return false;
    }

    bool isCyclic(string code, map<string, course>& cMap) {
        unordered_set<string> visited;
        unordered_set<string> recStack;
        return cMap[code].hasCycle(cMap, visited, recStack);
    }

    friend class student;
};

class student {
private:
    string sid;
    string sname;
    int year;
    set<string> complc;
    set<string> enrolc;
    unordered_map<char, int> slotMap; 

public:
    student() : sid(""), sname(""), year(0), complc(), enrolc() {}
    student(string id, string n, int y, set<string> Comple) 
        : sid(id), sname(n), year(y), complc(Comple), enrolc() {}

    bool checkpre(const string& code, const map<string, course>& cMap) const {
        for (const auto& x : cMap.at(code).prereq) {
            if (complc.find(x) == complc.end()) return false;
        }
        return true;
    }

    bool clash(const string& code, const map<string, course>& cMap) const {
        return slotMap.find(cMap.at(code).slot) != slotMap.end();
    }

    void enroll(const string& code, const map<string, course>& cMap) {
        enrolc.insert(code);
        slotMap[cMap.at(code).slot]++; // Update slotMap on enrollment
    }

    void drop(const string& code, const map<string, course>& cMap) {
        // Remove the course code from the enrolled courses set
        enrolc.erase(code);
        char courseSlot = cMap.at(code).slot;             // Find the slot of the course being dropped
        slotMap[courseSlot]--;

        if (slotMap[courseSlot] == 0) {
            slotMap.erase(courseSlot);
        }
    }
    friend class course;
};

int main() {
    int q;
    cin >> q;
    string comm;

    map<string, student> sMap;
    map<string, course> cMap;
    map<string, set<string>> csList; 
    map<string, queue<string>> waitList;

    while (q--) {
        cin >> comm;
        string Sid, name, code;
        int y, ncc, Cred, Cap, npre, cur;
        char Slot;

        if (comm == "add_student") {
            cin >> Sid >> name >> y >> ncc;
            set<string> Comple;
            for (int i = 0; i < ncc; i++) {
                string x;
                cin >> x;
                Comple.insert(x);
            }
            sMap.emplace(Sid, student(Sid, name, y, Comple));
        }

        else if (comm == "add_course") {
            cin >> code >> name >> Cred >> Cap >> Slot >> npre;
            vector<string> pre;
            bool exist = true;
            for (int i = 0; i < npre; i++) {
                string x;
                cin >> x;
                if (cMap.find(x) == cMap.end()) {
                    exist = false;
                    break;
                }
                pre.push_back(x);
            }
            if (exist) {
                course tempCourse(code, name, Cred, Cap, Slot, pre, Cap);
                cMap[code] = tempCourse;
                if (cMap[code].isCyclic(code, cMap)) {
                    cMap.erase(code); // Remove if cycle detected
                    cout << "Cycle detected. Course " << code << " was not added." << endl;
                }
            }

        }

        else if (comm == "enroll") {
            cin >> Sid >> code;
            if (sMap.find(Sid) == sMap.end() || cMap.find(code) == cMap.end()) continue;
            if (sMap[Sid].checkpre(code, cMap)) {
                if (cMap[code].checkCap()) {
                    if (!sMap[Sid].clash(code, cMap)) {
                        sMap[Sid].enroll(code, cMap);
                        cMap[code].decCap();
                        csList[code].insert(Sid); 
                    }
                }
                else{
                    waitList[code].push(Sid);
                }
            }
        }

        else if (comm == "drop") {
            cin >> Sid >> code;
            if (sMap.find(Sid) == sMap.end() || cMap.find(code) == cMap.end()) continue;

            sMap[Sid].drop(code, cMap);
            cMap[code].incCap();

            if (csList[code].count(Sid)) {
                csList[code].erase(Sid); 
            }

            // Process waitlist
            while (!waitList[code].empty()) {
                string newSid = waitList[code].front();
                waitList[code].pop();
                
                // Check for slot clash
                if (!sMap[newSid].clash(code, cMap)) {
                    sMap[newSid].enroll(code, cMap);
                    cMap[code].decCap();
                    csList[code].insert(newSid); 
                    break; 
                }
            }
        }

        else if (comm == "print") {
            cin >> code;
            if (csList.find(code) == csList.end()) {
                cout << "Invalid Course " << code << endl;
            } else {
                cout << "Enrolled students in " << code << ":" << endl;
                for (const auto& x : csList[code]) {
                    cout << x << " ";
                }
            }
            cout << endl;
        }
    }

    return 0;
}
