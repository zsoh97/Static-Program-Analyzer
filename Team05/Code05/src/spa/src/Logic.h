//
// Created by Low Cheng Yi on 30/8/2020.
//

#ifndef LOGIC
#define LOGIC

#include <string>
#include "PKB.h"

using namespace std;

class Logic {
public:
    Logic();
    static string ProcessProcedure(const string& line, PKB& pkb);
    static void ProcessContainer(const string& line, const string& parent, const string& stmtNumber, const string& type, PKB& pkb, bool elseBlock);
    static void ProcessRead(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, bool elseBlock);
    static void ProcessPrint(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, bool elseBlock);
    static void ProcessCall(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, bool elseBlock);
    static void ProcessAssignment(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, bool elseBlock);
};

#endif