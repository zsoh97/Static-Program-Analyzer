#pragma once

#include <string>

using namespace std;

enum Relationship {
    //NONE,
    FOLLOWS,
    FOLLOWS_STAR,
    PARENT,
    PARENT_STAR,
    USES,
    MODIFIES,
    PATTERN
};

Relationship StringToRelationship(string rsType);

// such that clauses will be by default, the base Clause object
struct Clause {
    string left;
    string right;
    Relationship relationship;

    // constructors and methods
    Clause(string left, string right);
    Clause(string left, string right, string rsType);
    void SetRelationship(Relationship rs);
    virtual string GetRelationshipString();
    bool IsLeftUnderscore();
    bool IsRightUnderscore();
};

// Pattern clause will be an inheritance from the Clause object
struct Pattern : public Clause { // pattern clause
    string assignAttribute;
    bool isExpressionPartial;

    Pattern(string left, string right, string assignAttr);
    string GetRelationshipString();
};

struct InvalidQueryException : public exception {
    const char * what () const throw () {
        return "PQL Query is invalid!";
    }
};

string TrimLeft(string str);
string TrimRight(string str);
string Trim(string str);