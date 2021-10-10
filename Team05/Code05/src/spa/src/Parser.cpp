#include <iostream>
#include <string>
#include <utility>
#include <fstream>
#include <stack>

using namespace std;

#include "ParserController.h"
#include "Validator.h"

int ParseString(string input, PKB& pkb) {
    int stmtNo = 0;
    bool expectElse = false;
    bool expectProcedure = true;
    bool elseBlock = false;
    stack<string> ifStmtNo;
    stack<string> stack;
    ParserController pc;
    try {
        while (input.find('{') != string::npos || input.find('}') != string::npos || input.find(';') != string::npos) {
            //Finding the next statement
            int posLeft = input.find('{');
            int posRight = input.find('}');
            int posSemicolon = input.find(';');
            if (input.find('{') == string::npos) {
                posLeft = INT_MAX;
            }
            if (input.find('}') == string::npos) {
                posRight = INT_MAX;
            }
            if (input.find(';') == string::npos) {
                posSemicolon = INT_MAX;
            }

            if (posLeft < posRight && posLeft < posSemicolon) {
                string stmt = trim(input.substr(0, posLeft));
                if (expectElse) {
                    if (stmt.substr(0, 4) == "else") {
                        if (ifStmtNo.empty()) {
                            throw "No 'if' statements before 'else'.";
                        }
                        stack.push(ifStmtNo.top());
                        ifStmtNo.pop();
                        expectElse = false;
                        elseBlock = true;
                    } else {
                        throw "Expecting 'else' after 'if' block";
                    }
                } else if (expectProcedure) {
                    if (stmt.substr(0, 9) == "procedure") {
                        if (!stack.empty()) {
                            throw "cannot declare procedure in another procedure.";
                        } else {
                            if (stmt[9] != ' ') {
                                throw "Invalid procedure line, missing space before procedure name.";
                            } else {
                                string procedure_name = trim(stmt.substr(10, (stmt.size() - 10)));
                                stack.push(procedure_name);
                                pc.HandleProcedure(procedure_name, pkb);
                                expectProcedure = false;
                            }
                        }
                    } else {
                        throw "Expecting procedure declaration";
                    }
                } else {
                    if (stmt.substr(0, 5) == "while") {
                        stmtNo++;
                        string cond_expr = trim(stmt.substr(5, (stmt.size() - 5)));
                        pc.HandleContainer(cond_expr, stack.top(), to_string(stmtNo), "while", pkb, elseBlock);
                        stack.push(to_string(stmtNo));
                    } else if (input.substr(0, 2) == "if") {
                        stmtNo++;
                        string cond_expr = trim(stmt.substr(2, (stmt.size() - 2)));
                        if (cond_expr.substr(cond_expr.size() - 4, 4) == "then") {
                            cond_expr = cond_expr.substr(0, cond_expr.size() - 4);
                            pc.HandleContainer(cond_expr, stack.top(), to_string(stmtNo), "if", pkb, elseBlock);
                            stack.push(to_string(stmtNo));
                            ifStmtNo.push(to_string(stmtNo));
                        } else {
                            throw "missing keyword 'then' in 'if' statement.";
                        }
                    } else {
                        if (stmt.substr(0, 9) == "procedure") {
                            throw "cannot declare procedure in another procedure.";
                        } else {
                            throw "Invalid statement";
                        }
                    }
                }
                input = input.substr(posLeft + 1);
            } else if (posSemicolon < posLeft && posSemicolon < posRight) {
                if (expectElse) {
                    throw "Expecting 'else' after 'if' statement.";
                } else if (expectProcedure) {
                    throw "statement cannot exist outside procedure block";
                } else {
                    stmtNo++;
                    string stmt = trim(input.substr(0, posSemicolon));
                    if (stmt.substr(0, 4) == "read") {
                        if (stmt[4] != ' ') {
                            throw "Invalid read statement, missing space before variable name.";
                        } else {
                            string varName = stmt.substr(5, (stmt.size() - 5));
                            pc.HandleRead(varName, stack.top(), to_string(stmtNo), pkb, elseBlock);
                        }
                    } else if (stmt.substr(0, 4) == "call") {
                        if (stmt[4] != ' ') {
                            throw "Invalid call statement, missing space before procedure name.";
                        } else {
                            string procName = stmt.substr(5, (stmt.size() - 5));
                            pc.HandleCall(procName, stack.top(), to_string(stmtNo), pkb, elseBlock);
                        }
                    } else if (stmt.substr(0, 5) == "print") {
                        if (stmt[5] != ' ') {
                            throw "Invalid print statement, missing space before variable name.";
                        } else {
                            string varName = stmt.substr(6, (stmt.size() - 6));
                            pc.HandlePrint(varName, stack.top(), to_string(stmtNo), pkb, elseBlock);
                        }
                    } else {
                        if (stmt.find('=') == string::npos) {
                            throw "Unknown statement.";
                        } else {
                            string assign_stmt = trim(stmt);
                            pc.HandleAssignment(assign_stmt, stack.top(), to_string(stmtNo), pkb, elseBlock);
                        }
                    }
                    input = input.substr(posSemicolon + 1);
                }
            } else if (posRight < posLeft && posRight < posSemicolon) {
                if (expectElse) {
                    throw ": Expecting 'else' after 'if' statement.";
                } else if (expectProcedure) {
                    throw "statement cannot exist outside procedure block";
                } else {
                    if (stack.empty()) {
                        throw "Invalid nesting level: No '{' before '}'";
                    } else {

                        if (ifStmtNo.empty()) {
                            expectElse = false;
                            elseBlock = false;
                        } else {
                            if (ifStmtNo.top() != stack.top()) {
                                expectElse = false;
                                elseBlock = false;
                            } else {
                                expectElse = true;
                            }
                        }
                        stack.pop();
                        if (stack.empty()) {
                            expectProcedure = true;
                        } else {
                            expectProcedure = false;
                        }
                    }
                    input = input.substr(posRight + 1);
                }
            }
        }
    } catch (char *msg) {
        cout << "error in stmt " << to_string(stmtNo) << ": " << msg << endl;
        return -1;
    }
    cout<<pkb.getString()<<endl;
    return 0;
}

int Parse(const string filename, PKB &pkb) {
    fstream file;
    file.open(filename, ios::in);
    if (!file.is_open()) {
        cout << "File cannot be opened." << endl;
        exit(0);
    }

    //New Implementation (String lines together first)
    string line;
    string input;
    while (getline(file, line)) {
        input.append(trim(line));
    }

    int responseCode = ParseString(input, pkb);
    if (responseCode == -1) {
        exit(0);
    }
    return 0;
}
