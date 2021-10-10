//
// Created by Low Cheng Yi on 30/8/2020.
//

#ifndef PARSER_CONTROLLER
#define PARSER_CONTROLLER
#include <string>
#include "Logic.h"

using namespace std;

class ParserController {
private:
    Logic logic;
public:
    ParserController();
    void HandleProcedure(const string& line, PKB& pkb);
    void HandleContainer(const string& line, const string& parent, const string& stmtNumber, const string& type, PKB& pkb, bool elseBlock);
    void HandleRead(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, bool elseBlock);
    void HandlePrint(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, bool elseBlock);
    void HandleCall(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, bool elseBlock);
    void HandleAssignment(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, bool elseBlock);
};

#endif