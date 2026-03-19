#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <set>

using namespace std;

const string DATA_FILE = "storage.db";
const int MAX_KEY_LEN = 64;

struct Entry {
    char key[65];  // 64 + null terminator
    int value;
    bool deleted;

    Entry() : value(0), deleted(false) {
        memset(key, 0, sizeof(key));
    }

    Entry(const string& k, int v) : value(v), deleted(false) {
        strncpy(key, k.c_str(), 64);
        key[64] = '\0';
    }
};

const int ENTRY_SIZE = sizeof(Entry);

class Database {
private:
    unordered_map<string, set<int>> data;
    string filename;

    void loadFromFile() {
        data.clear();

        ifstream file(filename, ios::binary);
        if (!file) return;

        Entry entry;
        while (file.read((char*)&entry, ENTRY_SIZE)) {
            if (!entry.deleted) {
                data[entry.key].insert(entry.value);
            }
        }

        file.close();
    }

    void saveToFile() {
        ofstream file(filename, ios::binary | ios::trunc);
        if (!file) return;

        for (const auto& pair : data) {
            const string& key = pair.first;
            const set<int>& values = pair.second;

            for (int value : values) {
                Entry entry(key, value);
                file.write((char*)&entry, ENTRY_SIZE);
            }
        }

        file.close();
    }

public:
    Database(const string& fname = DATA_FILE) : filename(fname) {
        loadFromFile();
    }

    ~Database() {
        saveToFile();
    }

    void insert(const string& key, int value) {
        data[key].insert(value);
    }

    void remove(const string& key, int value) {
        auto it = data.find(key);
        if (it != data.end()) {
            it->second.erase(value);
            if (it->second.empty()) {
                data.erase(it);
            }
        }
    }

    void find(const string& key, vector<int>& values) {
        values.clear();

        auto it = data.find(key);
        if (it != data.end()) {
            values.assign(it->second.begin(), it->second.end());
        }
    }
};

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    Database db;

    int n;
    cin >> n;
    cin.ignore(); // Ignore newline after n

    for (int i = 0; i < n; i++) {
        string command;
        cin >> command;

        if (command == "insert") {
            string key;
            int value;
            cin >> key >> value;
            db.insert(key, value);
        }
        else if (command == "delete") {
            string key;
            int value;
            cin >> key >> value;
            db.remove(key, value);
        }
        else if (command == "find") {
            string key;
            cin >> key;

            vector<int> values;
            db.find(key, values);

            if (values.empty()) {
                cout << "null\n";
            } else {
                for (size_t j = 0; j < values.size(); j++) {
                    if (j > 0) cout << " ";
                    cout << values[j];
                }
                cout << "\n";
            }
        }
    }

    return 0;
}