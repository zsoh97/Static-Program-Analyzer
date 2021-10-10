#include <iostream>
#include <string>
#include <iterator>
#include <ctype.h>
#include <stack>
#include <Validator.h>
#include "PKB.h"
using namespace std;

void addTableToString(ostringstream& os, const string& name, const unordered_map<string, list<string>>& m) {
    os << name << ": \n";
    for (const auto& pair : m) {
        os << "{" << pair.first << ":";
        for (const auto& item : pair.second) {
            os << " " << item;
        }
        os << "}\n";
    }
}

void addTableToString(ostringstream& os, const string& name, const unordered_map<string, string>& m) {
    os << name << ": \n";
    for (const auto& pair : m) {
        os << "{" << pair.first << ": " << pair.second << "}\n";
    }
}

void addSetToString(ostringstream& os, const string& name, const set<string>& m) {
    os << name << ": \n";
    os << "{ ";
    for (const auto& entry : m) {
        os << entry << " ";
    }
    os << "}\n";
}

string PKB::getString() {
    ostringstream os;
    addTableToString(os, "followsTable", followsTable);
    addTableToString(os, "followedByTable", followedByTable);
    addTableToString(os, "parentTable", parentTable);
    addTableToString(os, "childTable", childTable);
    addTableToString(os, "statementTypeTable", statementTypeTable);
    usesTable.addToString(os, "usesTable", "usedByTable");
    modifiesTable.addToString(os, "modifiesTable", "modifiedByTable");
    addTableToString(os, "assignmentToPatternTable", assignmentToPatternTable);
    addTableToString(os, "patternToAssignmentTable", patternToAssignmentTable);
    addSetToString(os, "varTable", variableSet);
    addSetToString(os, "constantTable", constantSet);
    addSetToString(os, "procTable", procedureSet);
    return os.str();
}

int precedence(char op) {
    switch(op) {
        case '-': case '+':
            return 1;
        case '*': case '/': case '%':
            return 2;
        default:
            return 0;
    }
}

bool isOperator(char c) {
    string operators = "+-*/%";
    return operators.find(c) != std::string::npos;
}

string PKB::getShuntingYardString(const string& input) {
    bool sameVariable = false;
    string output = "";
    stack<char> operatorStack;

    for (char currentChar : input) {
        if (isalpha(currentChar) || isdigit(currentChar)) { // Part of a number/variable
            if (!sameVariable) {
                if (currentChar == '0') { // Number is padded with zeros
                    continue;
                }
                output += ' ';
                sameVariable = true;
            }
            output += currentChar;
        } else if (currentChar == ' ') {
            continue;
        } else if (isOperator(currentChar)) { // Operator
            sameVariable = false; // we have finished reading a variable, next token is operator
            while(!operatorStack.empty() && (precedence(currentChar) <= precedence(operatorStack.top()))) {
                char operatorToAppend = operatorStack.top();
                operatorStack.pop();
                output += ' ';
                output += operatorToAppend;
            }
            operatorStack.push(currentChar);

        } else if (currentChar == '(') {
            operatorStack.push(currentChar);
        } else if (currentChar == ')') {
            sameVariable = false; // we have finished reading a variable, next token is close parentheses
            while (operatorStack.top() != '(') {
                output += ' ';
                output += operatorStack.top();
                operatorStack.pop();
            }
            // exit condition of stack is when it encounters (, pop it and discard
            operatorStack.pop();
        }

    }
    // Pop the rest of operators on stack onto output
    while (!operatorStack.empty()) {
        output += ' ';
        output += operatorStack.top();
        operatorStack.pop();
    }

    return trim(output);
}

// API PARSER
void PKB::InsertVariable(string variable) {
    variableSet.insert(variable);
}

void PKB::InsertConstant(string constant) {
    constantSet.insert(constant);
}

void PKB::InsertProcedure(string procedure) {
    procedureSet.insert(procedure);
}

// Parent string should either refer to procedure name or parent statement number
void PKB::SetParent(string parent, string child, bool isElseBlock) {
    if(isElseBlock) {
        elseBlockSet.insert(child);
        if (parent != "0" && childTable.find(parent) != childTable.end()) {
            string prevChild = childTable[parent].back();
            if(elseBlockSet.count(prevChild) != 0) {
                //prevchild exists in else block
                followsTable.insert(pair<string, string>(prevChild, child));
                followedByTable.insert(pair<string, string>(child, prevChild));
            }
        }
    } else {
        // child follows prevChild
        if (parent != "0" && childTable.find(parent) != childTable.end()) {
            string prevChild = childTable[parent].back();
            followsTable.insert(pair<string, string>(prevChild, child));
            followedByTable.insert(pair<string, string>(child, prevChild));
        }
    }

    // Parent -> Child relationship
    // If key does not exist in table
    if (childTable.find(parent) == childTable.end()) {
        // Create new list of children
        list<string> children;
        // Add child to the new list
        children.push_back(child);
        // Insert the new key value pair
        childTable.insert(pair<string, list<string>>(parent, children));
    } else {
        // Key already exists, add to the back of the list
        childTable[parent].push_back(child);
    }

    // Child -> Parent relationship
    parentTable.insert(pair<string, string>(child, parent));
}

void PKB::SetUses(string stmtNum, string variable) {
    while (stmtNum != "0") {
        usesTable.set(stmtNum, variable);

        if (parentTable.find(stmtNum) == parentTable.end()) {
            return;
        }
        stmtNum = GetParent(stmtNum).front();
    }
}

void PKB::SetModifies(string stmtNum, string variable) {
    while (stmtNum != "0") {
        modifiesTable.set(stmtNum, variable);

        if (parentTable.find(stmtNum) == parentTable.end()) {
            return;
        }
        stmtNum = GetParent(stmtNum).front();
    }
}

void PKB::SetPattern(string lhs, string rhs) {
    string shuntingYardString = getShuntingYardString(rhs);
    assignmentToPatternTable.insert(pair<string, string>(lhs, shuntingYardString));
    if (patternToAssignmentTable.find(shuntingYardString) == patternToAssignmentTable.end()) { // new pattern
        // Create new list
        list<string> newListOfStatementNums;
        // Add statement number to list
        newListOfStatementNums.push_back(lhs);
        patternToAssignmentTable.insert(pair<string, list<string>>(shuntingYardString, newListOfStatementNums));
    } else { // Existing pattern already exists, add a new statement number to list
        patternToAssignmentTable[shuntingYardString].push_back(lhs);
    }
}

void PKB::SetStatementType(string type, string stmtNum) {
    if (type != "procedure")
        lastStatementNumber++;
    statementTypeTable[type].push_back(stmtNum);
}

// API PQL

// FOLLOWS QUERIES
list<string> PKB::GetFollows(string followedBy) {
    list<string> follows;

    // Check if there is a follows entry
    if (followsTable.find(followedBy) == followsTable.end()) {
        return follows;
    }

    follows.push_back(followsTable[followedBy]);
    return follows;
}

list<string> PKB::GetFollowedBy(string follows) {
    list<string> followedBy;

    // Check if there is a followedBy entry
    if (followedByTable.find(follows) == followedByTable.end()) {
        return followedBy;
    }

    followedBy.push_back(followedByTable[follows]);
    return followedBy;
}

list<string> PKB::GetAllFollows(string followedBy) {

    // return newly created list
    list<string> listToReturn;

    // Check if parent exists
    if (parentTable.find(followedBy) == parentTable.end()) {
        // Parent does not exist
        return listToReturn;
    }

    string nestedBy = parentTable[followedBy];

    list<string> listOfStatementsInNesting = childTable[nestedBy];
    list<string>::iterator pointer = listOfStatementsInNesting.begin();

    while ((*pointer) != followedBy) {
        pointer++;
    }

    // Pointer now at element, we want everything after so advance one more
    pointer++;

    while (pointer != listOfStatementsInNesting.end()) {
        if (elseBlockSet.count(followedBy) != 0) {
            if (elseBlockSet.count(*(pointer)) != 0) {
                listToReturn.push_back(*pointer);
            }
        } else {
            if (elseBlockSet.count(*(pointer)) == 0) {
                listToReturn.push_back(*pointer);
            }
        }
        pointer++;
    }

    return listToReturn;
}

list<string> PKB::GetAllFollowedBy(string follows) {

    // return newly created list
    list<string> listToReturn;

    // Check if parent exists
    if (parentTable.find(follows) == parentTable.end()) {
        // Parent does not exist
        return listToReturn;
    }

    string nestedBy = parentTable[follows];
    list<string> listOfStatementsInNesting = childTable[nestedBy];
    list<string>::iterator pointer = listOfStatementsInNesting.begin();

    // Check if empty list
    if (listOfStatementsInNesting.empty()) {
        return listToReturn;
    }

    while ((*pointer) != follows) {
        if (elseBlockSet.count(follows) != 0) {
            if (elseBlockSet.count(*(pointer)) != 0) {
                listToReturn.push_back(*(pointer));
            }
        } else {
            if (elseBlockSet.count(*(pointer)) == 0) {
                listToReturn.push_back(*pointer);
            }
        }
        pointer++;
    }

    return listToReturn;
}

// PARENT QUERIES
list<string> PKB::GetParent(string child) {
    // return newly created list
    list<string> listToReturn;

    // Check if parent exists
    if (parentTable.find(child) == parentTable.end()) {
        // Parent does not exist
        return listToReturn;
    }

    listToReturn.push_back(parentTable[child]);
    return listToReturn;
}

list<string> PKB::GetChildren(string parent) {
    // Check if child exists
    if (childTable.find(parent) == childTable.end()) {
        // Child does not exist, return empty list
        return {};
    }

    return childTable[parent];
}

list<string> PKB::GetAllParents(string child) {
    list<string> listToReturn;

    // Check if parent exists
    if (parentTable.find(child) == parentTable.end()) {
        // Parent does not exist
        return listToReturn;
    }

    string currentParent = parentTable[child];
    if (!IsNumber(currentParent)) {
        return listToReturn; // return empty list as parent is a procedure (string of characters instead of stmt number)
    }
    while(IsNumber(currentParent)) {
        listToReturn.push_front(currentParent);
        currentParent = parentTable[currentParent];
    }
    return listToReturn;
}

list<string> PKB::GetChildrenOfChildren(string parent) {
    list<string> listToReturn;
    InsertChildrenOfChildren(parent, listToReturn);
    return listToReturn;
}

// USES QUERIES
list<string> PKB::GetUsesVariables(string stmtNum) {
    return usesTable.getAllForward(move(stmtNum));
}

list<string> PKB::GetUsedByStatements(string variable) {
    return usesTable.getAllInverse(move(variable));
}

// MODIFIES QUERIES
list<string> PKB::GetModifiesVariables(string stmtNum) {
    return modifiesTable.getAllForward(move(stmtNum));
}

list<string> PKB::GetModifiedByStatements(string variable) {
    return modifiesTable.getAllInverse(move(variable));
}

// PATTERN QUERIES

list<string> PKB::GetPatternStatements(string lhs, string rhs, bool isPartial) {
    // Check rhs
    string shuntingYardString = getShuntingYardString(rhs);
    set<string> filteredRHS;

    if (!isPartial) {
        // Check validity of expression on rhs
        if(!Validator::IsExpr(rhs)) {
            // Not valid, return empty list
            return list<string>();
        }

        if (patternToAssignmentTable.find(shuntingYardString) == patternToAssignmentTable.end()) { // no such key
            return list<string>(); // empty list
        } else {
            list<string> listOfStatements = patternToAssignmentTable[shuntingYardString];
            list<string>::iterator listIterator = listOfStatements.begin();
            while (listIterator != listOfStatements.end()) {
                filteredRHS.insert(*listIterator);
                listIterator++;
            }
        }
    } else {
        if (rhs == "") {
            // insert all assignment statements inside
            list<string> assignStatements = statementTypeTable["assign"];
            list<string>::iterator listIterator = assignStatements.begin();
            while(listIterator != assignStatements.end()) {
                filteredRHS.insert(*listIterator);
                listIterator++;
            }

        } else {
            // Check validity of expression on rhs
            if(!Validator::IsExpr(rhs)) {
                // Not valid, return empty list
                return list<string>();
            }
            for (pair<string, list<string>> element: patternToAssignmentTable) {
                string paddedString = ' ' + element.first + ' ';
                string paddedShunting = ' ' + shuntingYardString + ' ';
                if (paddedString.find(paddedShunting) != std::string::npos) { // key is a substring
                    filteredRHS.insert(element.second.begin(), element.second.end());
                }
            }
        }
    }

    // Check LHS
    if (lhs == "_") {
        list<string> toReturn;
        set<string>::iterator setIterator = filteredRHS.begin();
        while (setIterator != filteredRHS.end()) {
            toReturn.push_back(*setIterator);
            setIterator++;
        }
        return toReturn;
    } else {
        // 1. Go to modifiedByTable and check which statements modify the variable on LHS
        // 2. Filter out common statements in both containers
        list<string> statementsThatModify = modifiesTable.getAllInverse(lhs);
        list<string> toReturn;
        list<string>::iterator pointer = statementsThatModify.begin();

        while (pointer != statementsThatModify.end()) {
            if(filteredRHS.find(*pointer) != filteredRHS.end()) {
                toReturn.push_back(*pointer);
            }
            pointer++;
        }
        return toReturn;
    }
}

list<string> PKB::GetPatternVariables(string rhs, bool isPartial) {

    // Check RHS
    string shuntingYardString = getShuntingYardString(rhs);
    set<string> filteredRHS;

    if (!isPartial) {
        // Check validity of expression on rhs
        if(!Validator::IsExpr(rhs)) {
            // Not valid, return empty list
            return list<string>();
        }
        if (patternToAssignmentTable.find(shuntingYardString) == patternToAssignmentTable.end()) { // no such key
            return list<string>(); // empty list
        } else {
            list<string> listOfStatements = patternToAssignmentTable[shuntingYardString];
            list<string>::iterator listIterator = listOfStatements.begin();
            while (listIterator != listOfStatements.end()) {
                filteredRHS.insert(*listIterator);
                listIterator++;
            }
        }
    } else {
        if (rhs == "") {
            // insert all assignment statements inside
            list<string> assignStatements = statementTypeTable["assign"];
            list<string>::iterator listIterator = assignStatements.begin();
            while(listIterator != assignStatements.end()) {
                filteredRHS.insert(*listIterator);
                listIterator++;
            }
        } else {
            // Check validity of expression on rhs
            if(!Validator::IsExpr(rhs)) {
                // Not valid, return empty list
                return list<string>();
            }
            for (pair<string, list<string>> element: patternToAssignmentTable) {
                string paddedString = ' ' + element.first + ' ';
                string paddedShunting = ' ' + shuntingYardString + ' ';
                if (paddedString.find(paddedShunting) != std::string::npos) { // key is a substring
                    filteredRHS.insert(element.second.begin(), element.second.end());
                }
            }
        }
    }

    set<string> setOfVariables;
    set<string>::iterator setOfStatementsIterator = filteredRHS.begin();
    while(setOfStatementsIterator != filteredRHS.end()) {
        setOfVariables.insert(GetModifiesVariables(*setOfStatementsIterator).front());
        setOfStatementsIterator++;
    }

    set<string>::iterator setOfVariablesIterator = setOfVariables.begin();
    list<string> toReturn;

    while(setOfVariablesIterator != setOfVariables.end()) {
        toReturn.push_back(*setOfVariablesIterator);
        setOfVariablesIterator++;
    }
    return toReturn;
}

// STATEMENT TYPE QUERIES
list<string> PKB::GetStatementType(string type) {
    return statementTypeTable[type];
}

// MISC QUERIES
list<string> PKB::GetAllVariables() {
    list<string> toReturn;
    set<string>::iterator setIterator = variableSet.begin();
    while(setIterator != variableSet.end()) {
        toReturn.push_back(*setIterator);
        setIterator++;
    }
    return toReturn;
}

list<string> PKB::GetAllProcedures() {
    list<string> toReturn;
    set<string>::iterator setIterator = procedureSet.begin();
    while(setIterator != procedureSet.end()) {
        toReturn.push_back(*setIterator);
        setIterator++;
    }
    return toReturn;
}

list<string> PKB::GetAllConstants() {
    list<string> toReturn;
    set<string>::iterator setIterator = constantSet.begin();
    while(setIterator != constantSet.end()) {
        toReturn.push_back(*setIterator);
        setIterator++;
    }
    return toReturn;
}

int PKB::GetLastStatementNumber() {
    return lastStatementNumber;
}


// HELPER FUNCTIONS
bool PKB::IsNumber(string basicString) {
    std::string::const_iterator it = basicString.begin();
    while (it != basicString.end() && std::isdigit(*it)) ++it;
    return !basicString.empty() && it == basicString.end();
}

bool PKB::IsProcedure(string name) {
    set<string>::iterator it = procedureSet.find(name);
    return it != procedureSet.end();
}

// Inserts all children recursively into the input list
void PKB::InsertChildrenOfChildren(string parent, list<string> &inputList) {
    if(childTable.find(parent) == childTable.end()) {
        return;
    }
    /*
    if (childTable[parent].empty()) {
        return;
    }
     */
    list<string> children = childTable[parent];
    list<string>::iterator pointer = children.begin();
    while (pointer != children.end()) {
        inputList.push_back(*pointer);
        InsertChildrenOfChildren(*pointer, inputList);
        pointer++;
    }
}


