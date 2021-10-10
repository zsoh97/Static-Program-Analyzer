#include "RelationshipTable.h"

using std::list;
using std::ostringstream;
using std::pair;
using std::string;
using std::unordered_map;
using std::unordered_set;

template<class T, class U>
template<class F, class S>
void RelationshipTable<T, U>::addTableToString(ostringstream& os, const string& name, const unordered_map<F, unordered_set<S>>& m) {
    os << name << ": \n";
    for (const auto& pair : m) {
        os << "{" << pair.first << ":";
        for (const auto& item : pair.second) {
            os << " " << item;
        }
        os << "}\n";
    }
}

template<class T, class U>
void RelationshipTable<T, U>::addToString(ostringstream& os, const string& forwardName, const string& inverseName) {
    addTableToString(os, forwardName, forwardTable);
    addTableToString(os, inverseName, inverseTable);
}

template<class T>
static list<T> listFrom(const unordered_set<T>& set) {
    return list<T>(set.begin(), set.end());
}

template<class T, class U>
static void insert(unordered_map<T, unordered_set<U>>& table, T key, U value) {
    // If key does not exist in table
    if (table.find(key) == table.end()) {
        // Create new unordered_set of children
        unordered_set<U> children;
        // Add child to the new unordered_set
        children.insert(value);
        // Insert the new key value pair
        table.insert(pair<T, unordered_set<U>>(key, children));
    } else {
        // Key already exists, add to the back of the unordered_set
        table[key].insert(value);
    }
}

template<class T, class U>
void RelationshipTable<T, U>::set(T parent, U child) {
    // Parent and child are generic
    insert(forwardTable, parent, child);
    insert(inverseTable, child, parent);
}

template<class T, class U>
list<U> RelationshipTable<T, U>::getAllForward(T parent) {
    return listFrom(forwardTable[parent]);
}

template<class T, class U>
list<T> RelationshipTable<T, U>::getAllInverse(U child) {
    return listFrom(inverseTable[child]);
}

template class RelationshipTable<string, string>;
