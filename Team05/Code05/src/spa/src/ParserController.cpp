//
// Created by Low Cheng Yi on 30/8/2020.
//

#include "ParserController.h"
#include "Validator.h"

ParserController::ParserController() = default;

void ParserController::HandleProcedure(const string& line, PKB& pkb) {
  if (Validator::IsNAME(trim(line))) {
    logic.ProcessProcedure(line, pkb);
  } else {
    throw "Invalid proc_name";
  }
}

void ParserController::HandleContainer(const string& line, const string& parent, const string& stmtNumber, const string& type, PKB& pkb, const bool elseBlock) {
    if (Validator::IsValidConditionalStmt(trim(line))) {
        logic.ProcessContainer(erase(line), parent, stmtNumber, type, pkb, elseBlock);
    } else {
        throw "Invalid cond expr/expr/term/factor";
    }
}

void ParserController::HandleRead(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, const bool elseBlock) {
    if (Validator::IsNAME(trim(line))) {
        logic.ProcessRead(line, parent, stmtNumber, pkb, elseBlock);
    } else {
        throw "Invalid var_name";
    }
}

void ParserController::HandlePrint(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, const bool elseBlock) {
    if (Validator::IsNAME(trim(line))) {
        logic.ProcessPrint(trim(line), parent, stmtNumber, pkb, elseBlock);
    } else {
        throw "Invalid var_name";
    }
}

void ParserController::HandleCall(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, const bool elseBlock) {
    if (Validator::IsNAME(trim(line))) {
        logic.ProcessCall(line, parent, stmtNumber, pkb, elseBlock);
    } else {
        throw "Invalid proc_name";
    }
}

void ParserController::HandleAssignment(const string& line, const string& parent, const string& stmtNumber, PKB& pkb, const bool elseBlock) {
    if (Validator::IsValidAssignmentStmt(trim(line))) {
        logic.ProcessAssignment(erase(line), parent, stmtNumber, pkb, elseBlock);
    } else {
        throw "Invalid var_name/const/expr";
    }
}