#pragma once

#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <list>

using std::list;
using std::ostringstream;
using std::string;
using std::unordered_map;
using std::unordered_set;

template<class T, class U>
class RelationshipTable {
    unordered_map<T, unordered_set<U>> forwardTable;
    unordered_map<U, unordered_set<T>> inverseTable;

public:
    template<class F, class S>
    static void addTableToString(ostringstream& os, const string& name, const unordered_map<F, unordered_set<S>>& m);
    void addToString(ostringstream& os, const string& forwardName, const string& inverseName);

    void set(T, U);
    list<U> getAllForward(T);
    list<T> getAllInverse(U);
};
