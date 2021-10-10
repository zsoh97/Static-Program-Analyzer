//
// Created by Low Cheng Yi on 30/8/2020.
//

#include "Logic.h"
#include "Validator.h"
#include <regex>
#include <unordered_set>

Logic::Logic() = default;

string Logic::ProcessProcedure(const string& line, PKB& pkb) {
    string name = erase(line);
    pkb.SetParent("0", name, false);
    pkb.SetStatementType("procedure", name);
    pkb.InsertProcedure(name);
    return name;
}

void Logic::ProcessContainer(const string& line, const string& parent, const string& stmtNumber, const string& type, PKB& pkb, const bool elseBlock) {

    pkb.SetParent(parent, stmtNumber, elseBlock);
    pkb.SetStatementType(type, stmtNumber);

    unordered_set<string> varSet;
    regex reg("[-+*/%()|&<>=!]");
    smatch m;
    string temp = erase(line);
    while(regex_search(temp, m, reg)) {
        int end = m.position();
        string var = temp.substr(0, end);
        if (end != 0) {
            varSet.insert(var);
        }
        temp = temp.substr(end+1);
    }

    for(const string& var: varSet) {
        if (Validator::IsNAME(var)) {
            pkb.InsertVariable(var);
        }
        pkb.SetUses(stmtNumber, var);
        if (pkb.IsNumber(var)) {
            pkb.InsertConstant(var);
        } else {
            pkb.InsertVariable(var);
        }
    }
}

void Logic::ProcessRead(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, const bool elseBlock) {
    const string& var = erase(line);
    pkb.SetParent(parent, stmtNumber, elseBlock);
    pkb.SetStatementType("read", stmtNumber);
    pkb.SetModifies(stmtNumber, var);
}

void Logic::ProcessPrint(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, const bool elseBlock) {
    const string& var = erase(line);
    pkb.SetParent(parent, stmtNumber, elseBlock);
    pkb.SetStatementType("print", stmtNumber);
    pkb.SetUses(stmtNumber, var);
}

void Logic::ProcessCall(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, const bool elseBlock) {
    const string& var = erase(line);
    pkb.SetParent(parent, stmtNumber, elseBlock);
    pkb.SetStatementType("call", stmtNumber);
    pkb.SetUses(stmtNumber, var);
}

void Logic::ProcessAssignment(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, const bool elseBlock) {
    std::string stmt = erase(line);
    int pos = stmt.find('=');

    string leftVar = stmt.substr(0, pos);
    string pattern = stmt.substr(pos+1);
    pkb.SetParent(parent, stmtNumber, elseBlock);
    pkb.SetStatementType("assign", stmtNumber);
    pkb.SetModifies(stmtNumber, leftVar);
    pkb.SetPattern(stmtNumber, pattern);

    unordered_set<string> varSet;
    regex reg("[-+*/%()]");
    smatch m;
    string temp = erase(stmt.substr(pos+1));
    while(regex_search(temp, m, reg)) {
        int end = m.position();
        string var = temp.substr(0, end);
        if (!var.empty()) {
            varSet.insert(var);
        }
        temp = temp.substr(end + 1);
    }
    if (!temp.empty()) {
        varSet.insert(temp);
    }
    pkb.InsertVariable(leftVar);
    for(const string& var: varSet) {
        pkb.SetUses(stmtNumber, var);
        if (pkb.IsNumber(var)) {
            pkb.InsertConstant(var);
        } else {
            pkb.InsertVariable(var);
        }
    }

}