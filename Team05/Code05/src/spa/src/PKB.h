#pragma once

#include<stdio.h>
#include <iostream>
#include <string>
#include <set>
#include <list>
#include <unordered_map>
#include <iterator>
#include <ctype.h>
#include "RelationshipTable.h"

using namespace std;
typedef short PROC;

class PKB {

private:
    int lastStatementNumber = 0;
    set<string> variableSet;
    set<string> constantSet;
    set<string> procedureSet;

    unordered_map<string, string> followsTable;
    unordered_map<string, string> followedByTable;

    unordered_map<string, string> parentTable;
    unordered_map<string, list<string>> childTable;

    unordered_map<string, list<string>> statementTypeTable;

    RelationshipTable<string, string> usesTable;
    RelationshipTable<string, string> modifiesTable;

    unordered_map<string, string> assignmentToPatternTable;
    unordered_map<string, list<string>> patternToAssignmentTable;

    set<string> elseBlockSet;
public:
    string getString();
    string getShuntingYardString(const string& input);

    // API calls for PKB
    void InsertVariable(string variable);
    void InsertConstant(string constant);
    void InsertProcedure(string procedure);
    void SetParent(string parent, string child, bool isElseBlock);
    void SetUses(string stmtNum, string variable);
    void SetModifies(string stmtNum, string variable);
    void SetPattern(string lhs, string rhs);
    void SetStatementType(string type, string stmtNum);

    // API calls for PQL

    // Follows queries
    list<string> GetFollows(string followedBy);
    list<string> GetFollowedBy(string follows);
    list<string> GetAllFollows(string followedBy);
    list<string> GetAllFollowedBy(string follows);

    // Parent queries
    list<string> GetParent(string child);
    list<string> GetChildren(string parent);
    list<string> GetAllParents(string child);
    list<string> GetChildrenOfChildren(string parent);

    // Uses queries
    list<string> GetUsesVariables(string stmtNum);
    list<string> GetUsedByStatements(string variable);

    // Modifies queries
    list<string> GetModifiesVariables(string stmtNum);
    list<string> GetModifiedByStatements(string variable);

    // Pattern queries
    list<string> GetPatternStatements(string lhs, string rhs, bool isPartial);
    list<string> GetPatternVariables(string rhs, bool isPartial);

    // Statement type
    list<string> GetStatementType(string type);

    // MISC queries
    list<string> GetAllVariables();
    list<string> GetAllProcedures();
    list<string> GetAllConstants();
    int GetLastStatementNumber();

    bool IsNumber(string basicString);
    bool IsProcedure(string name);
    void InsertChildrenOfChildren(string parent, list<string> &inputList);
};