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
    course(string code, string name, int Cred, int Cap, char Slot, vector<string> pre, int cur) {
        ccode = code;
        cname = name;
        cred = Cred;
        cap = Cap;
        slot = Slot;
        prereq = pre;
        curSize = cur;
    }

    bool checkCap() const {
        return curSize > 0;
    }

    friend class student;
};

class student {
private:
    string sid;
    string sname;
    int year;
    unordered_set<string> complc;
    vector<string> enrolc;

public:
    student() : sid(""), sname(""), year(0), complc(), enrolc() {}
    student(string id, string n, int y, unordered_set<string> Comple) {
        sid = id;
        sname = n;
        year = y;
        complc = Comple;
    }

    bool checkpre(const string& code, const unordered_map<string, course>& cMap) const {
        for (const auto& x : cMap.at(code).prereq) {
            if (complc.find(x) == complc.end()) return false;
        }
        return true;
    }

    bool clash(const string& code, const unordered_map<string, course>& cMap) const {
        for (const auto& x : enrolc) {
            if (cMap.at(code).slot == cMap.at(x).slot) return true;
        }
        return false;
    }

    void enroll(const string& code, unordered_map<string, course>& cMap) {
        enrolc.push_back(code);
        cMap[code].curSize--;
    }

    friend class course;
};

int main() {
    int q;
    cin >> q;
    string comm;

    unordered_map<string, student> sMap;
    unordered_map<string, course> cMap;
    unordered_map<string, vector<string>> csList;
    unordered_map<string, queue<string>> waitList;

    while (q--) {
        cin >> comm;
        string Sid, name, code;
        int y, ncc, Cred, Cap, npre, cur;
        char Slot;

        if (comm == "add_student") {
            cin >> Sid >> name >> y >> ncc;
            unordered_set<string> Comple;
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
                cur = Cap;
                cMap.emplace(code, course(code, name, Cred, Cap, Slot, pre, cur));
            }
        }

        else if (comm == "enroll") {
            cin >> Sid >> code;

            if (sMap.find(Sid) == sMap.end() || cMap.find(code) == cMap.end()) continue;
            if (sMap[Sid].checkpre(code, cMap)) {
                if (cMap[code].checkCap()) {
                    if (!sMap[Sid].clash(code, cMap)) {
                        sMap[Sid].enroll(code, cMap);
                        csList[code].push_back(Sid);
                    }
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
