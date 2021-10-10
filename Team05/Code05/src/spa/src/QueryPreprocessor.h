#pragma once

#include "QueryClause.h"
#include <unordered_map>
#include <string>
#include <utility>
#include <vector>
#include <memory>

using namespace std;

enum Entity {
    PROCEDURE,
    STATEMENT,
    VARIABLE,
    ASSIGN,
    WHILE,
    IF,
    PRINT,
    READ,
    CONSTANT,
    CALL
};

Entity StringToEntity(string attribute);

class PQLQuery {
    bool isValid;
    string attributeToSelect;
    unordered_map<string, Entity> attributes;
    vector<unique_ptr<Clause>> clauses;
    Entity CheckEntity(string attrName);

public:
    PQLQuery() {
        this->isValid = true;
    }

    PQLQuery(string attrToBeSelected, unordered_map<string, Entity> attributes) {
        this->isValid = true;
        this->attributeToSelect = std::move(attrToBeSelected);
        this->attributes = std::move(attributes);
        this->clauses = vector<unique_ptr<Clause>>();
//        this->clauses = vector<Clause>();
    }

    bool operator==(const PQLQuery &pqlR) const {
        return (this->attributeToSelect == pqlR.attributeToSelect)
               && (this->attributes == pqlR.attributes)
               && (this->clauses == pqlR.clauses);
    }

    void BecomeInvalid();
    bool IsQueryValid();
    Entity GetSelectEntity();
    void SetClause(unique_ptr<Clause> clausePtr);
    void SetClause(unique_ptr<Pattern> patternPtr);
    vector<unique_ptr<Clause>> * GetClauses();
//    void SetClause(Clause clause);
//    vector<Clause> GetClauses();
    string GetAttributeToSelect();
    unordered_map<string, Entity> GetAttributes();

    // methods for checking Clause attributes
    bool IsLeftAttribute(Clause *clause);
    bool IsRightAttribute(Clause *clause);
    bool IsLeftPatternStringAttribute(Pattern *pattern);
    Entity GetLeftAttributeEntity(Clause *clause);
    Entity GetRightAttributeEntity(Clause *clause);
    void PrioritisePattern(); // puts pattern clause at the top of the vector if there is any
};

// parser and query related methods
PQLQuery ParseQuery(string query);
void CheckQuoteValidity(string string);
void CheckPatternValidity(unordered_map<string, Entity> attrMap, Pattern pattern);
void CheckClauseValidity(unordered_map<string, Entity> attrMap, Clause clause);
bool IsAttributeDeclared(unordered_map<string, Entity> attrMap, string var);
bool IsNumber(string str);
bool IsVariable(string str);
