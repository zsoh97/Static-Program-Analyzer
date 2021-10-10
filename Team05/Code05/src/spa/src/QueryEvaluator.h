#pragma once

#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <map>

#include "QueryPreprocessor.h"
#include "QueryClause.h"
#include "PKB.h"

using namespace std;

class QueryEvaluator {
	PQLQuery *query;
	PKB pkb;
    set<map<string, string>> results;

public:
	explicit QueryEvaluator(PKB &pkb);
	list<string> EvaluateQuery(PQLQuery &q);
    static void RemoveDuplicates(list<string>& l);
    static map<string, string> MergeCombinations(map<string, string>& first, map<string, string>& second);

private:
    int underscoreCounter = 0;
	set<map<string, string>> EvaluatePattern(Pattern *pattern);
    set<map<string, string>> EvaluateDirectRelationship(Clause *clause);
    set<map<string, string>> EvaluateInverseRelationship(Clause *clause);
    set<map<string, string>> EvaluateBidirectionalRelationship(Clause *clause);
    set<map<string, string>> EvaluateSimpleRelationship(Clause *clause);
	void FilterResults(set<map<string, string>>& clauseResults);
    list<string> GetAttributeList(const string& attribute);
	list<string> GetAllOfEntity(Entity entity);
	string GetUnderscoreKey();
};