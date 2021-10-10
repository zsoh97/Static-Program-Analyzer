//
// Created by zsoh9 on 30/8/2020.
//

#ifndef AUTOTESTER_VALIDATOR_H
#define AUTOTESTER_VALIDATOR_H

#include <string>

std::string ltrim(std::string str, const std::string& chars = "\t\n\v\f\r ");

std::string rtrim(std::string str, const std::string& chars = "\t\n\v\f\r ");

std::string trim(std::string str, const std::string& chars = "\t\n\v\f\r ");

std::string erase(std::string str);

class Validator {
private:
    static bool IsValidRelationFactor(const std::string &toCheck);

    static bool IsValidConditionalExpression(const std::string &toCheck);

    static bool IsValidWithOperator(const std::string &lhs, const std::string &rhs);

    static unsigned int GetExpressionBracketEnd(const std::string &toFind);

    static unsigned int GetConditionalExpressionBracketEnd(const std::string &toFind);
public:
    Validator();

    static bool IsConst(const std::string &toCheck);

    static bool IsNAME(const std::string &toCheck);

    static bool IsExpr(const std::string &toCheck);

    static bool IsValidRelationExpression(const std::string &toCheck);

    static bool IsValidAssignmentStmt(const std::string &toCheck);

    static bool IsValidConditionalStmt(const std::string &toCheck);
};

#endif //AUTOTESTER_VALIDATOR_H
