#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstring>
#include <unordered_map>

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
    string filename;

public:
    Database(const string& fname = DATA_FILE) : filename(fname) {
        // Ensure file exists
        fstream file(filename, ios::binary | ios::in | ios::out);
        if (!file) {
            file.open(filename, ios::binary | ios::out);
            file.close();
        }
    }

    void insert(const string& key, int value) {
        fstream file(filename, ios::binary | ios::in | ios::out);
        if (!file) {
            file.open(filename, ios::binary | ios::out | ios::in);
        }

        // Check if entry already exists
        file.seekg(0, ios::end);
        file.seekg(0, ios::beg);

        Entry entry;
        bool exists = false;

        while (file.read((char*)&entry, ENTRY_SIZE)) {
            if (!entry.deleted && strcmp(entry.key, key.c_str()) == 0 && entry.value == value) {
                exists = true;
                break;
            }
        }

        if (!exists) {
            Entry newEntry(key, value);
            file.clear();
            file.seekp(0, ios::end);
            file.write((char*)&newEntry, ENTRY_SIZE);
        }

        file.close();
    }

    void remove(const string& key, int value) {
        fstream file(filename, ios::binary | ios::in | ios::out);
        if (!file) return;

        Entry entry;

        while (file.read((char*)&entry, ENTRY_SIZE)) {
            if (!entry.deleted && strcmp(entry.key, key.c_str()) == 0 && entry.value == value) {
                // Mark as deleted
                file.seekp(-ENTRY_SIZE, ios::cur);
                entry.deleted = true;
                file.write((char*)&entry, ENTRY_SIZE);
                break;
            }
        }

        file.close();
    }

    void find(const string& key, vector<int>& values) {
        values.clear();

        ifstream file(filename, ios::binary);
        if (!file) return;

        Entry entry;

        while (file.read((char*)&entry, ENTRY_SIZE)) {
            if (!entry.deleted && strcmp(entry.key, key.c_str()) == 0) {
                values.push_back(entry.value);
            }
        }

        file.close();

        // Sort values
        sort(values.begin(), values.end());
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