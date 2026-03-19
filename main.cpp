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

// More memory-efficient entry structure
struct FileEntry {
    char key[65];  // 64 + null terminator
    int value;

    FileEntry() : value(0) {
        memset(key, 0, sizeof(key));
    }

    FileEntry(const string& k, int v) : value(v) {
        strncpy(key, k.c_str(), 64);
        key[64] = '\0';
    }

    bool operator<(const FileEntry& other) const {
        int cmp = strcmp(key, other.key);
        if (cmp != 0) return cmp < 0;
        return value < other.value;
    }
};

const int ENTRY_SIZE = sizeof(FileEntry);

class Database {
private:
    vector<FileEntry> data;  // Sorted vector for binary search
    string filename;
    bool dirty;  // Whether data has been modified

    void loadFromFile() {
        data.clear();

        ifstream file(filename, ios::binary);
        if (!file) return;

        FileEntry entry;
        while (file.read((char*)&entry, ENTRY_SIZE)) {
            data.push_back(entry);
        }

        file.close();

        // Sort and remove duplicates
        sort(data.begin(), data.end());
        auto last = unique(data.begin(), data.end(),
            [](const FileEntry& a, const FileEntry& b) {
                return strcmp(a.key, b.key) == 0 && a.value == b.value;
            });
        data.erase(last, data.end());

        dirty = false;
    }

    void saveToFile() {
        if (!dirty) return;

        ofstream file(filename, ios::binary | ios::trunc);
        if (!file) return;

        for (const auto& entry : data) {
            file.write((char*)&entry, ENTRY_SIZE);
        }

        file.close();
        dirty = false;
    }

    // Binary search for key
    pair<int, int> findKeyRange(const string& key) {
        // Find first position with key >= target
        int left = 0, right = data.size();
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (strcmp(data[mid].key, key.c_str()) < 0) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        int start = left;

        // Find first position with key > target
        left = start, right = data.size();
        while (left < right) {
            int mid = left + (right - left) / 2;
            if (strcmp(data[mid].key, key.c_str()) <= 0) {
                left = mid + 1;
            } else {
                right = mid;
            }
        }
        int end = left;

        return {start, end};
    }

public:
    Database(const string& fname = DATA_FILE) : filename(fname), dirty(false) {
        loadFromFile();
    }

    ~Database() {
        saveToFile();
    }

    void insert(const string& key, int value) {
        FileEntry newEntry(key, value);

        // Find insertion position
        auto pos = lower_bound(data.begin(), data.end(), newEntry);

        // Check if already exists
        if (pos != data.end() && strcmp(pos->key, key.c_str()) == 0 && pos->value == value) {
            return;  // Already exists
        }

        data.insert(pos, newEntry);
        dirty = true;
    }

    void remove(const string& key, int value) {
        FileEntry target(key, value);

        // Find the entry
        auto pos = lower_bound(data.begin(), data.end(), target);

        if (pos != data.end() && strcmp(pos->key, key.c_str()) == 0 && pos->value == value) {
            data.erase(pos);
            dirty = true;
        }
    }

    void find(const string& key, vector<int>& values) {
        values.clear();

        auto range = findKeyRange(key);
        for (int i = range.first; i < range.second; i++) {
            values.push_back(data[i].value);
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