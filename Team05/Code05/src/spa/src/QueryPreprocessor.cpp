#include "QueryClause.h"
#include "QueryPreprocessor.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <exception>
#include <utility>
#include <memory>

using namespace std;

// implementation of the PQLQuery class methods

Entity StringToEntity(string attribute) {
    Entity currEntity;
    if (attribute.compare("procedure") == 0) {
        currEntity = PROCEDURE;
    } else if (attribute.compare("stmt") == 0) {
        currEntity = STATEMENT;
    } else if (attribute.compare("variable") == 0) {
        currEntity = VARIABLE;
    } else if (attribute.compare("assign") == 0) {
        currEntity = ASSIGN;
    } else if (attribute.compare("while") == 0) {
        currEntity = WHILE;
    } else if (attribute.compare("if") == 0) {
        currEntity = IF;
    } else if (attribute.compare("print") == 0) {
        currEntity = PRINT;
    } else if (attribute.compare("read") == 0) {
        currEntity = READ;
    } else if (attribute.compare("constant") == 0) {
        currEntity = CONSTANT;
    } else if (attribute.compare("call") == 0) {
        currEntity = CALL;
    } else {
            throw InvalidQueryException();
    }

    return currEntity;
}

void PQLQuery::BecomeInvalid() {
    this->isValid = false;
}

bool PQLQuery::IsQueryValid() {
    return this->isValid;
}

Entity PQLQuery::GetSelectEntity() {
    cout << "Reached: getSelectEntity()" << endl;
    return CheckEntity(this->attributeToSelect);
}

void PQLQuery::SetClause(unique_ptr<Clause> clausePtr) {
    this->clauses.emplace_back(move(clausePtr));
}

void PQLQuery::SetClause(unique_ptr<Pattern> patternPtr) {
    this->clauses.emplace_back(move(patternPtr));
}

vector<unique_ptr<Clause>> * PQLQuery::GetClauses() {
    return &this->clauses;
}

//void PQLQuery::SetClause(Clause clause) {
//    this->clauses.push_back(clause);
//}
//
//vector<Clause> PQLQuery::GetClauses() {
//    return this->clauses;
//}

string PQLQuery::GetAttributeToSelect() {
    return this->attributeToSelect;
}

Entity PQLQuery::CheckEntity(string attrName) {
    // For logging
    switch (attributes[attrName]) {
        case PROCEDURE:
            cout << "Entity checked: procedure" << endl;
            break;
        case VARIABLE:
            cout << "Entity checked: variable" << endl;
            break;
        case STATEMENT:
            cout << "Entity checked: statement" << endl;
            break;
        case ASSIGN:
            cout << "Entity checked: assign" << endl;
            break;
        case WHILE:
            cout << "Entity checked: while" << endl;
            break;
        case IF:
            cout << "Entity checked: if" << endl;
            break;
        case PRINT:
            cout << "Entity checked: print" << endl;
            break;
        case READ:
            cout << "Entity checked: read" << endl;
            break;
        case CALL:
            cout << "Entity checked: read" << endl;
            break;
        case CONSTANT:
            cout << "Entity checked: read" << endl;
            break;
    }
    // End for logging

    return attributes[attrName]; // previously: attributes.at(attrName) -- gave an exception?
}

// clause related methods to check and get attribute and Entity in Clause

bool PQLQuery::IsLeftAttribute(Clause *clause) {
    string leftAttr = clause->left;
    unordered_map<string, Entity>::const_iterator checkAttr = attributes.find(leftAttr);
    if (checkAttr == attributes.end()) {
        return false;
    }
    return true;
}

bool PQLQuery::IsRightAttribute(Clause *clause) {
    string rightAttr = clause->right;
    unordered_map<string, Entity>::const_iterator checkAttr = attributes.find(rightAttr);
    if (checkAttr == attributes.end()) {
        return false;
    }
    return true;
}

bool PQLQuery::IsLeftPatternStringAttribute(Pattern *pattern) {
    string leftAttr = pattern->left;
    unordered_map<string, Entity>::const_iterator checkAttr = attributes.find(leftAttr);
    if (checkAttr == attributes.end()) {
        return false;
    }
    return true;
}

Entity PQLQuery::GetLeftAttributeEntity(Clause *clause) {
    if (!IsLeftAttribute(clause)) {
        // throw exception
    }
    string leftAttr = clause->left;
    return CheckEntity(leftAttr);
}

Entity PQLQuery::GetRightAttributeEntity(Clause *clause) {
    if (!IsRightAttribute(clause)) {
        // throw exception
    }
    string rightAttr = clause->right;
    return CheckEntity(rightAttr);
}

unordered_map<string, Entity> PQLQuery::GetAttributes() {
    return this->attributes;
}

// implement parseQuery

PQLQuery ParseQuery(string query) {
    cout << "Beginning to parse PQL query: " << query << endl;
    PQLQuery pqlQuery;
    try {
        // first collect all attributes listed by user which is separated by ";"
        unordered_map<string, Entity> attributes;
        size_t attrPos = query.find(";");
        while (attrPos != string::npos) {
            string attr = Trim(query.substr(0, attrPos));
            size_t pos = attr.find(" ");
            Entity attrEntity = StringToEntity(attr.substr(0, pos));
            attr = TrimLeft(attr.substr(pos + 1));

            // handle multiple attributes of same entity (separated by comma ",")
            size_t commaPos = attr.find(",");
            while (commaPos != string::npos) {
                string attrName = TrimRight(attr.substr(0, commaPos));
                attributes.insert({attrName, attrEntity});
                attr = TrimLeft(attr.substr(commaPos + 1));
                commaPos = attr.find(",");
            }
            attributes.insert({attr, attrEntity});

            // moving on
            query = TrimLeft(query.substr(attrPos + 1));
            attrPos = query.find(";");
        }

        // create PQLQuery object and set the select attribute
        if (query.find("Select") != 0) { // all PQL queries should have "Select" come after attribute naming
            throw InvalidQueryException();
        }
        query = Trim(query.substr(7));
        string selectAttr;
        if (query.find(" ") == string::npos) {
            selectAttr = query;
        } else {
            selectAttr = query.substr(0, query.find(" "));
        }
        // check if selectAttr attribute has been declared:
        if (!IsAttributeDeclared(attributes, selectAttr)) {
            throw InvalidQueryException();
        }
        pqlQuery = PQLQuery(selectAttr, attributes);
        query = TrimLeft(query.substr(selectAttr.size()));

        // take in clauses and set the clauses in the PQLQuery object
        while (!query.empty()) {
            size_t patternPos = query.find("pattern");
            size_t suchThatPos = query.find("such that");

            if (patternPos == 0) {
                if (query.find("pattern") != 0) {
                    throw InvalidQueryException();
                }
                query = TrimLeft(query.substr(8)); // remove "pattern"
                size_t bracketPos = query.find("(");
                if (bracketPos == string::npos) {
                    throw InvalidQueryException();
                }
                string patternAttr = TrimRight(query.substr(0, bracketPos));
                query = TrimLeft(query.substr(bracketPos + 1));
                size_t commaPos = query.find(",");
                if (commaPos == string::npos) {
                    throw InvalidQueryException();
                }
                string variable = TrimRight(query.substr(0, commaPos));
                //checkQuoteValidity(variable);
                query = TrimLeft(query.substr(commaPos + 1));
                size_t endBracketPos = query.find(")");
                if (endBracketPos == string::npos) {
                    throw InvalidQueryException();
                }
                string expression = TrimRight(query.substr(0, endBracketPos));
                Pattern patternClause = Pattern(variable, expression, patternAttr);
                CheckPatternValidity(attributes, patternClause);
                unique_ptr<Pattern> patternPtr = make_unique<Pattern>(patternClause);
                pqlQuery.SetClause(move(patternPtr));

                // loop for following Clauses
                try {
                    query = TrimLeft(query.substr(endBracketPos + 1));
                } catch (const out_of_range &oor) {
                    break;
                }

            } else if (suchThatPos == 0) {
                if (query.find("such that") != 0) {
                    throw InvalidQueryException();
                }
                query = TrimLeft(query.substr(10)); // remove "such that"
                size_t clausePos = query.find("(");
                if (clausePos == string::npos) {
                    throw InvalidQueryException();
                }
//                while (clausePos != string::npos) { // note, clausePos might not work on advanced SPA PQL
//                if (query.find("and") == 0) { // not in the scope of IT1
//                    query = TrimLeft(query.substr(4));
//                    clausePos = query.find("(");
//                }
                string clauseRs = TrimRight(query.substr(0, clausePos));
                query = TrimLeft(query.substr(clausePos + 1));
                size_t commaPos = query.find(",");
                if (commaPos == string::npos) {
                    throw InvalidQueryException();
                }
                string clauseLeft = TrimRight(query.substr(0, commaPos));
                CheckQuoteValidity(clauseLeft);
                query = TrimLeft(query.substr(commaPos + 1));
                size_t endClausePos = query.find(")");
                if (endClausePos == string::npos) {
                    throw InvalidQueryException();
                }
                string clauseRight = TrimRight(query.substr(0, endClausePos));
                CheckQuoteValidity(clauseRight);
                Clause currClause = Clause(clauseLeft, clauseRight, clauseRs);
                CheckClauseValidity(attributes, currClause);
                unique_ptr<Clause> clausePtr = make_unique<Clause>(currClause);
                pqlQuery.SetClause(move(clausePtr));

                // loop for following clauses
                try {
                    query = TrimLeft(query.substr(endClausePos + 1));
//                    clausePos = query.find("(");
                } catch (const out_of_range &oor) {
                    break;
                }
            } else {
                throw InvalidQueryException();
            }
        }
    } catch (const out_of_range &oor) {
        throw InvalidQueryException();
    } catch (InvalidQueryException &iqe) {
        pqlQuery.BecomeInvalid();
        cout << iqe.what() << endl;
    }

    return pqlQuery;
}

void CheckQuoteValidity(string string) { // quotes must have a start and end. else, throw invalid query.
    char quote = '"';
    // string.erase(remove(string.begin(), string.end(), quote), string.end());
    size_t qPos = string.find(quote);
    if (qPos == 0 || qPos == string.size() - 1) {
        if (string[0] != string[string.size() - 1]) {
            throw InvalidQueryException();
        }
    }
}

void CheckPatternValidity(unordered_map<string, Entity> attrMap, Pattern pattern) {
    // format: pattern a (variable, expression)

    // first check the assign attribute validity
    string patternAttr = pattern.assignAttribute;
    if (!IsAttributeDeclared(attrMap, patternAttr)) {
        throw InvalidQueryException();
    } else if (attrMap[patternAttr] != ASSIGN) { // attribute declared but its not assign
        throw InvalidQueryException();
    }

    // left parameter for Pattern should either be an declared VARIABLE attr or isVariable
    if (!pattern.IsLeftUnderscore()) {
        if (!IsAttributeDeclared(attrMap, pattern.left)) {
            if (!IsVariable(pattern.left)) { // if attribute not declared means it should be isVariable
                throw InvalidQueryException();
            }
        } else if (attrMap[pattern.left] != VARIABLE) { // attribute declared but its not variable
            throw InvalidQueryException();
        }
    }
}

void CheckClauseValidity(unordered_map<string, Entity> attrMap, Clause clause) {
    Relationship rs = clause.relationship;
    switch (rs) {
        case FOLLOWS:
            if (clause.left.find('"') != string::npos || clause.right.find('"') != string::npos) {
                throw InvalidQueryException();
            }
            // if left and right is not declared, then it should be a stmt number
            if (!IsAttributeDeclared(attrMap, clause.left)) {
                if (!IsNumber(clause.left) && !clause.IsLeftUnderscore()) {
                    throw InvalidQueryException();
                }
            } else if (attrMap[clause.left] == VARIABLE) {
                throw InvalidQueryException();
            }
            if (!IsAttributeDeclared(attrMap, clause.right)) {
                if (!IsNumber(clause.right) && !clause.IsRightUnderscore()) {
                    throw InvalidQueryException();
                }
            } else if (attrMap[clause.right] == VARIABLE) {
                throw InvalidQueryException();
            }
            break;
        case FOLLOWS_STAR:
            if (clause.left.find('"') != string::npos || clause.right.find('"') != string::npos) {
                throw InvalidQueryException();
            }
            // if left and right is not declared, then it should be a stmt number
            if (!IsAttributeDeclared(attrMap, clause.left)) {
                if (!IsNumber(clause.left) && !clause.IsLeftUnderscore()) {
                    throw InvalidQueryException();
                }
            } else if (attrMap[clause.left] == VARIABLE) {
                throw InvalidQueryException();
            }
            if (!IsAttributeDeclared(attrMap, clause.right)) {
                if (!IsNumber(clause.right) && !clause.IsRightUnderscore()) {
                    throw InvalidQueryException();
                }
            } else if (attrMap[clause.right] == VARIABLE) {
                throw InvalidQueryException();
            }
            break;
        case PARENT:
            if (clause.left.find('"') != string::npos || clause.right.find('"') != string::npos) {
                throw InvalidQueryException();
            }
            // if left and right is not declared, then it should be a stmt number
            if (!IsAttributeDeclared(attrMap, clause.left)) {
                if (!IsNumber(clause.left) && !clause.IsLeftUnderscore()) {
                    throw InvalidQueryException();
                }
            } else if (attrMap[clause.left] == VARIABLE) {
                throw InvalidQueryException();
            }
            if (!IsAttributeDeclared(attrMap, clause.right)) {
                if (!IsNumber(clause.right) && !clause.IsRightUnderscore()) {
                    throw InvalidQueryException();
                }
            } else if (attrMap[clause.right] == VARIABLE) {
                throw InvalidQueryException();
            }
            break;
        case PARENT_STAR:
            if (clause.left.find('"') != string::npos || clause.right.find('"') != string::npos) {
                throw InvalidQueryException();
            }
            // if left and right is not declared, then it should be a stmt number or underscore
            if (!IsAttributeDeclared(attrMap, clause.left)) {
                if (!IsNumber(clause.left) && !clause.IsLeftUnderscore()) {
                    throw InvalidQueryException();
                }
            } else if (attrMap[clause.left] == VARIABLE) {
                throw InvalidQueryException();
            }
            if (!IsAttributeDeclared(attrMap, clause.right)) {
                if (!IsNumber(clause.right) && !clause.IsRightUnderscore()) {
                    throw InvalidQueryException();
                }
            } else if (attrMap[clause.right] == VARIABLE) {
                throw InvalidQueryException();
            }
            break;
        case USES:
            if (clause.IsLeftUnderscore()) {
                throw InvalidQueryException();
            } else {
                if (IsAttributeDeclared(attrMap, clause.left)) {
                    if (attrMap[clause.left] == READ) {
                        throw InvalidQueryException();
                    }
                } else if (!IsNumber(clause.left) && !IsVariable(clause.left)) {
                    // if not stmt number and not proc name
                    throw InvalidQueryException();
                }
            }
            // right must either be a declared attribute VARIABLE
            // or a variable which contains ", or an underscore
            if (!clause.IsRightUnderscore()) {
                if (!IsAttributeDeclared(attrMap, clause.right)) {
                    if (!IsVariable(clause.right)) {
                        throw InvalidQueryException();
                    }
                } else if (attrMap[clause.right] != VARIABLE) { // attribute declared but its not variable
                    throw InvalidQueryException();
                }
            }
            break;
        case MODIFIES:
            if (clause.IsLeftUnderscore()) {
                throw InvalidQueryException();
            } else {
                if (IsAttributeDeclared(attrMap, clause.left)) {
                    if (attrMap[clause.left] == PRINT) {
                        throw InvalidQueryException();
                    }
                } else if (!IsNumber(clause.left) && !IsVariable(clause.left)) {
                    // if not stmt number and not proc name
                    throw InvalidQueryException();
                }
            }
            if (!clause.IsRightUnderscore()) {
                if (!IsAttributeDeclared(attrMap, clause.right)) {
                    if (!IsVariable(clause.right)) { // attribute not declared must be a variable
                        throw InvalidQueryException();
                    }
                } else if (attrMap[clause.right] != VARIABLE) { // attribute declared but its not variable
                    throw InvalidQueryException();
                }
            }
            break;
        case PATTERN:
            break;
    }
}

bool IsAttributeDeclared(unordered_map<string, Entity> attrMap, string var) {
    for (auto& at : attrMap) {
        if (at.first == var) {
            return true;
        }
    }
    return false;
}

bool IsNumber(string str) {
    return str.find_first_not_of("0123456789") == string::npos;
}

bool IsVariable(string str) {
    CheckQuoteValidity(str);
    // assume that quotes are valid since they are checked above
    if (str.find('"') != string::npos) {
        str = str.substr(1, str.size() - 2);
    } else { // variable should always start and end with quotations
        throw InvalidQueryException();
    }
    bool hasNoSymbol = str.find_first_of(" `~!@#$%^&*()-_=+[]{};:'\",.<>/?\\") == string::npos;
    bool isNumberFront = str.find_first_of("0123456789") == 0;
    return !isNumberFront && hasNoSymbol;
}
