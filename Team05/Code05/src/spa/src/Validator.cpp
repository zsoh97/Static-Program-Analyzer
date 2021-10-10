//
// Created by zsoh9 on 30/8/2020.
//
#pragma once
#include "Validator.h"

#include <regex>
#include <stack>
#include <iostream>

std::string ltrim(std::string str, const std::string& chars)
{
    str.erase(0, str.find_first_not_of(chars));
    return str;
}

std::string rtrim(std::string str, const std::string& chars)
{
    str.erase(str.find_last_not_of(chars) + 1);
    return str;
}

std::string trim(std::string str, const std::string& chars)
{
    return ltrim(rtrim(std::move(str), chars), chars);
}

std::string erase(std::string str)
{
    str.erase(remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}


Validator::Validator() = default;

bool Validator::IsNAME(const std::string &toCheck) {
    if(toCheck.find(' ') == std::string::npos) {
        return regex_match(toCheck, std::regex("^[a-zA-Z][a-zA-Z0-9]*$"));
    } else {
        return false;
    }
}

bool Validator::IsConst(const std::string &toCheck) {
    if(toCheck.find(' ') == std::string::npos) {
        return regex_match(toCheck, std::regex("^[0-9]+$"));
    } else {
        return false;
    }
}

bool
Validator::IsValidWithOperator(const std::string &lhs, const std::string &rhs) {
    std::smatch m;
    std::regex ops("[-+*/%]");
    std::string exprOperator = rhs.substr(0, 1);
    if (regex_search(exprOperator, m, ops)) {
        return IsExpr(trim(lhs.substr(1, lhs.length() - 2))) &&
               IsExpr(trim(rhs.substr(1)));
    } else {
        return false;
    }
}

unsigned int Validator::GetExpressionBracketEnd(const std::string &toFind) {
    std::stack<char> stack;
    for (unsigned int i = 0; i < toFind.length(); i++) {
        if (toFind.at(i) == '(') {
            stack.push('(');
            continue;
        }
        if (toFind.at(i) == ')') {
            stack.pop();
            if (stack.empty()) {
                return i;
            } else {
                continue;
            }
        }
    }
    return 0;
}

bool Validator::IsExpr(const std::string &toCheck) {
    if (toCheck.empty()) {
        return false;
    }
    if (IsNAME(toCheck) || IsConst(toCheck)) {
        return true;
    }
    std::regex reg("[-+*/%(]");
    std::smatch m;
    std::string rhs = toCheck;
    std::string lhs;
    if (regex_search(rhs, m, reg)) {
        if (m.str() != "(") {
            return IsExpr(trim(rhs.substr(0, m.position()))) &&
                   IsExpr(trim(rhs.substr(m.position() + 1)));
        } else {
            if (m.position() != 0) {
                return false;
            }
            unsigned int pos = GetExpressionBracketEnd(rhs);
            if (pos == 0) {
                return false;
            }
            lhs = rhs.substr(0, pos + 1);
            rhs = rhs.substr(pos + 1);
            if (rhs.empty()) {
                return IsExpr(trim(lhs.substr(1, lhs.length() - 2)));
            } else {
                return IsValidWithOperator(trim(lhs), trim(rhs));
            }
        }
    } else {
        return false;
    }
}

bool Validator::IsValidRelationFactor(const std::string &toCheck) {
    return IsNAME(toCheck) || IsConst(toCheck) || IsExpr(toCheck);
}

bool Validator::IsValidRelationExpression(const std::string &toCheck) {
    std::regex reg("[<>!=]=|[<>]");
    std::smatch m;
    std::string rhs = toCheck;
    std::string lhs;
    while (regex_search(rhs, m, reg)) {
        int end = m.position(); // <-- use the match position
        lhs = rhs.substr(0, end);
        rhs = rhs.substr(end + m.length());
    }
    if (toCheck.length() == rhs.length()) {
        return false;
    } else {
        return IsValidRelationFactor(trim(lhs)) && IsValidRelationFactor(trim(rhs));
    }
}

unsigned int
Validator::GetConditionalExpressionBracketEnd(const std::string &toFind) {
    std::stack<char> stack;
    for (unsigned int i = 0; i < toFind.length(); i++) {
        if (toFind.at(i) == '!') {
            int temp = 1;
            while (toFind.at(i + temp) == ' ') {
                temp++;
            }
            std::cout<<temp<<std::endl;
            if (toFind.at(i + temp) == '(') {
                stack.push('(');
                i = i + temp;
                continue;
            }
        }
        if (toFind.at(i) == '(') {
            stack.push('(');
            continue;
        }
        if (toFind.at(i) == ')') {
            if (stack.empty()) {
                return 0;
            }
            stack.pop();
            if (stack.empty()) {
                return i;
            } else {
                continue;
            }
        }
    }
    return 0;
}

bool Validator::IsValidConditionalExpression(const std::string &toCheck) {
    std::cout<<toCheck<<std::endl;
    if (toCheck.at(0) != '(' || toCheck.back() != ')' ||
        toCheck.length() <= 2) {
        return false;
    }
    std::string rhs = trim(toCheck.substr(1, toCheck.length() - 2));
    if (IsValidRelationExpression(rhs)) {
        return true;
    }
    std::string lhs;
    unsigned int pos = GetConditionalExpressionBracketEnd(rhs);
    std::cout<<pos<<std::endl;
    if (pos == 0) {
        return false;
    }
    lhs = rhs.substr(0, pos + 1);
    rhs = rhs.substr(pos + 1);
    std::cout<<lhs<<std::endl;
    std::cout<<rhs<<std::endl;
    if (!rhs.empty()) {
        rhs = trim(rhs);
        if (rhs.substr(0, 2) != "&&" && rhs.substr(0, 2) != "||") {
            return false;
        }
        rhs = trim(rhs.substr(2));
        return IsValidConditionalExpression(trim(lhs)) &&
               IsValidConditionalExpression(trim(rhs));
    }
    if (lhs.at(0) == '!') {
        return IsValidConditionalExpression(trim(lhs.substr(1)));
    } else {
        return IsValidConditionalExpression(trim(lhs));
    }
}

bool Validator::IsValidAssignmentStmt(const std::string &toCheck) {
    std::string lhs = toCheck.substr(0, toCheck.find('='));
    std::string rhs = toCheck.substr(toCheck.find('=') + 1);
    if (!IsNAME(trim(lhs))) {
        return false;
    } else {
        return IsExpr(trim(rhs));
    }
}

bool Validator::IsValidConditionalStmt(const std::string &toCheck) {
    return IsValidConditionalExpression(trim(toCheck));
}