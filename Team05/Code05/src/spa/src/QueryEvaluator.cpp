#include "QueryEvaluator.h"

#include <string>
#include <list>
#include <map>
#include <iostream>
#include <utility>
#include <memory>
#include <vector>

using namespace std;

QueryEvaluator::QueryEvaluator(PKB &pkb) {
	this->pkb = pkb;
	this->query = nullptr;
}

list<string> QueryEvaluator::EvaluateQuery(PQLQuery &q) {
	query = &q;
	if (!query->IsQueryValid()) {
        return list<string>();
	}

	string selectAttribute = query->GetAttributeToSelect();
	list<string> allOfSelectAttribute = GetAllOfEntity(query->GetSelectEntity());

	for (auto & it : allOfSelectAttribute) {
	    map<string, string> combination;
	    combination.insert(pair<string, string>(selectAttribute, it));
	    results.insert(combination);
	}

	vector<unique_ptr<Clause>> * clauses = query->GetClauses();

    // For logging
    cout << "getClauses" << endl;
    for (const auto& c : *clauses) {
        cout << "- Relationship: " << c->GetRelationshipString()
             << "   LHS: " << c->left << "   RHS: " << c->right << endl;
    }
    // End for logging

    for (const auto& c: *clauses) {
	    Clause *clause = c.get();
		set<map<string, string>> clauseResults;

		if (clause->relationship == PATTERN) {
            cout << "EVALUATING: Pattern Clause" << endl;
            auto* pattern = dynamic_cast<Pattern*>(clause);
		    cout << "Pattern clause:\n" << "Attribute: " << pattern->assignAttribute << "   Variable: " << pattern->left
		         << "   Expression: " << pattern->right << endl;
            clauseResults = EvaluatePattern(pattern);
		} else {
            cout << "EVALUATING: Such That Clause" << endl;
            if (query->IsLeftAttribute(clause) || clause->IsLeftUnderscore()) {
                if (query->IsRightAttribute(clause) || clause->IsRightUnderscore()) {
                    clauseResults = EvaluateBidirectionalRelationship(clause);
                } else {
                    clauseResults = EvaluateInverseRelationship(clause);
                }
            } else {
                if (query->IsRightAttribute(clause) || clause->IsRightUnderscore()) {
                    clauseResults = EvaluateDirectRelationship(clause);
                } else {
                    clauseResults = EvaluateSimpleRelationship(clause);
                }
            }
        }

		// For logging
		cout << "Before combination: Original results" << endl;
		for (const auto & result : results) {
            for (const auto& x : result)
                cout << "{" << x.first << ": " << x.second << "} ";
            cout << endl;
		}
        cout << "Before combination: Results from clause evaluation" << endl;
        for (const auto & clauseResult : clauseResults) {
            for (const auto& x : clauseResult)
                cout << "{" << x.first << ": " << x.second << "} ";
            cout << endl;
        }
        // End for logging

        FilterResults(clauseResults);
        if (results.empty()) {
            cout << "Query results set is empty; terminating clause evaluation process" << endl;
            break;
        }

        // For logging
        cout << "After combination: Original results" << endl;
        for (const auto & clauseResult : clauseResults) {
            for (const auto& x : clauseResult)
                cout << "{" << x.first << ": " << x.second << "} ";
            cout << endl;
        }
        // End for logging
	}

    list<string> resultsList = GetAttributeList(selectAttribute);
    RemoveDuplicates(resultsList);
    return resultsList;
}

set<map<string, string>> QueryEvaluator::EvaluatePattern(Pattern *pattern) {
    set<map<string, string>> relationships;
    cout << "Received from Preprocessor: " << pattern->left << " " << pattern->right << endl;
    string left = pattern->left;
    if (pattern->IsLeftUnderscore())
        left = GetUnderscoreKey();
    string right = pattern->right;
    if (query->IsLeftPatternStringAttribute(pattern) || pattern->IsLeftUnderscore()) {
        // Case 1: pattern a (v, "RHS") or pattern a (_, "RHS")
        cout << "Left string of pattern is an attribute/underscore" << endl;
        list<string> variableList = pkb.GetPatternVariables(right, pattern->isExpressionPartial);
        list<string> relationshipList;
        for (auto & it : variableList) {
            cout << "Calling PKB API with arguments: " << it << " " << right << endl;
            list<string> statementList = pkb.GetPatternStatements(it, right, pattern->isExpressionPartial);
            cout << "Received from PKB: ";
            for (auto & it2 : statementList) {
                cout << it2 << " ";
                map<string, string> entry;
                entry.insert(pair<string, string>(pattern->assignAttribute, it2));
                entry.insert(pair<string, string>(left, it));
                relationships.insert(entry);
            }
            cout << endl;
        }
    } else {
        // Case 2: pattern a ("LHS", "RHS")
        cout << "Left string of pattern is a variable" << endl;
        left = left.substr(1, left.size() - 2);     // remove quotation marks
        cout << "Calling PKB API with arguments: " << left << " " << right << endl;
        list<string> relationshipList = pkb.GetPatternStatements(left, right, pattern->isExpressionPartial);
        cout << "Received from PKB: ";
        for (auto & it2 : relationshipList) {
            cout << it2 << " ";
            map<string, string> entry;
            entry.insert(pair<string, string>(pattern->assignAttribute, it2));
            relationships.insert(entry);
        }
        cout << endl;
    }
    return relationships;
}

// e.g. Follows(7, s) or Parent(7, s) or Uses(7, v) or Uses(7, _)
set<map<string, string>> QueryEvaluator::EvaluateDirectRelationship(Clause *clause) {
    set<map<string, string>> relationships;
	list<string> relationshipList;
    string left = clause->left;
    switch (clause->relationship) {
	case FOLLOWS:
		relationshipList = pkb.GetFollows(left);
		break;
	case FOLLOWS_STAR:
		relationshipList = pkb.GetAllFollows(left);
		break;
	case PARENT:
	    relationshipList = pkb.GetChildren(left);
	    break;
	case PARENT_STAR:
	    relationshipList = pkb.GetChildrenOfChildren(left);
	    break;
	case USES:
    {
       if (!pkb.IsNumber(left)) {     // to remove quotation marks if they exist -- e.g. Uses("main", v)
            left = left.substr(1, left.size() - 2);
        }
        relationshipList = pkb.GetUsesVariables(left);
        break;
    }
	case MODIFIES:
    {
        if (!pkb.IsNumber(left)) {     // to remove quotation marks if they exist -- e.g. Uses("main", v)
            left = left.substr(1, left.size() - 2);
        }
        relationshipList = pkb.GetModifiesVariables(left);
        break;
    }
	}
    list<string> typeList;
	string right = clause->right;
	if (clause->IsRightUnderscore()) {
	    right = GetUnderscoreKey();
	    if (clause->relationship == FOLLOWS || clause->relationship == FOLLOWS_STAR
	        || clause->relationship == PARENT || clause->relationship == PARENT_STAR) {
            typeList = GetAllOfEntity(STATEMENT);
	    } else if (clause->relationship == USES || clause->relationship == MODIFIES) {
            typeList = GetAllOfEntity(VARIABLE);
	    }
	} else {
        typeList = GetAllOfEntity(query->GetRightAttributeEntity(clause));
    }
    RemoveDuplicates(relationshipList);
	for (auto & it : relationshipList) {
	    auto typeIt = std::find(typeList.begin(), typeList.end(), it);
		if (typeIt != typeList.end()) {
            map<string, string> entry;
            entry.insert(pair<string, string>(right, it));
            relationships.insert(entry);
        }
	}
	return relationships;
}

// e.g. Follows(s, 7) or Parent(s, 7) or Uses(s, "x")
set<map<string, string>> QueryEvaluator::EvaluateInverseRelationship(Clause *clause) {
	set<map<string, string>> relationships;
	list<string> relationshipList;
	switch (clause->relationship) {
	case FOLLOWS:
		relationshipList = pkb.GetFollowedBy(clause->right);
		break;
	case FOLLOWS_STAR:
		relationshipList = pkb.GetAllFollowedBy(clause->right);
		break;
	case PARENT:
	{	// Necessary to check for possible procedure "disguised" as parent statement
		list<string> parent = pkb.GetParent(clause->right);
		for (auto & parentIt : parent) {
            if (pkb.IsNumber(parentIt))
                relationshipList.push_back(parentIt);
        }
		break;
	}
	case PARENT_STAR:
	{	// Necessary to check for possible procedure "disguised" as parent statement
		list<string> parents = pkb.GetAllParents(clause->right);
		for (auto & parent : parents) {
			if (pkb.IsNumber(parent))
                relationshipList.push_back(parent);
		}
		break;
	}
	case USES:
    {   // to remove quotation marks if they exist -- e.g. Uses(s, "x")
        string rhsWithoutQuotation = clause->right.substr(1, clause->right.size() - 2);
        relationshipList = pkb.GetUsedByStatements(rhsWithoutQuotation);
        break;
    }
	case MODIFIES:
    {   // to remove quotation marks if they exist -- e.g. Uses(s, "x")
        string rhsWithoutQuotation = clause->right.substr(1, clause->right.size() - 2);
        relationshipList = pkb.GetModifiedByStatements(rhsWithoutQuotation);
        break;
    }
	}
    list<string> typeList;
	string left;
    if (clause->IsLeftUnderscore()) {
        left = GetUnderscoreKey();
        if (clause->relationship == FOLLOWS || clause->relationship == FOLLOWS_STAR
            || clause->relationship == PARENT || clause->relationship == PARENT_STAR) {
            typeList = GetAllOfEntity(STATEMENT);
        }
    } else {
        left = clause->left;
        typeList = GetAllOfEntity(query->GetLeftAttributeEntity(clause));
    }
	for (auto & it : relationshipList) {
	    auto typeIt = std::find(typeList.begin(), typeList.end(), it);
        if (typeIt != typeList.end()) {
            map<string, string> entry;
            entry.insert(pair<string, string>(left, it));
            relationships.insert(entry);
        }
	}
	return relationships;
}

// e.g. Follows(s1, s2) or Parent(s1, s2) or Uses(x, v)
// Important note: Need to filter out only those entities that have valid values
set<map<string, string>> QueryEvaluator::EvaluateBidirectionalRelationship(Clause *clause) {
    set<map<string, string>> relationships;
	list<string> leftEntityList;
	string left, right;
    if (clause->IsLeftUnderscore()) {
        left = GetUnderscoreKey();
        if (clause->relationship == FOLLOWS || clause->relationship == FOLLOWS_STAR
            || clause->relationship == PARENT || clause->relationship == PARENT_STAR) {
            leftEntityList = GetAllOfEntity(STATEMENT);
        }
    } else {
        left = clause->left;
        leftEntityList = GetAllOfEntity(query->GetLeftAttributeEntity(clause));
    }
	list<string> leftList, rightList;
	for (auto & it : leftEntityList) {
		list<string> rightUnfilteredList;
		switch (clause->relationship) {
		case FOLLOWS:
			rightUnfilteredList = pkb.GetFollows(it);
			break;
		case FOLLOWS_STAR:
			rightUnfilteredList = pkb.GetAllFollows(it);
			break;
		case PARENT:
			rightUnfilteredList = pkb.GetChildren(it);
			break;
		case PARENT_STAR:
			rightUnfilteredList = pkb.GetChildrenOfChildren(it);
			break;
		case USES:
			rightUnfilteredList = pkb.GetUsesVariables(it);
			break;
		case MODIFIES:
			rightUnfilteredList = pkb.GetModifiesVariables(it);
			break;
		}
        RemoveDuplicates(rightUnfilteredList);
		list<string> rightTypeList;
        if (clause->IsRightUnderscore()) {
            right = GetUnderscoreKey();
            if (clause->relationship == FOLLOWS || clause->relationship == FOLLOWS_STAR
                || clause->relationship == PARENT || clause->relationship == PARENT_STAR) {
                rightTypeList = GetAllOfEntity(STATEMENT);
            } else if (clause->relationship == USES || clause->relationship == MODIFIES) {
                rightTypeList = GetAllOfEntity(VARIABLE);
            }
        } else {
            right = clause->right;
            rightTypeList = GetAllOfEntity(query->GetRightAttributeEntity(clause));
        }
        // Handle cases such as Follows(s, s) and Parent(s, s)
        if (left == right && (clause->relationship == FOLLOWS || clause->relationship == FOLLOWS_STAR
                              || clause->relationship == PARENT || clause->relationship == PARENT_STAR))
            return relationships;
		for (auto & it2 : rightUnfilteredList) {
			auto typeIt = std::find(rightTypeList.begin(), rightTypeList.end(), it2);
			if (typeIt != rightTypeList.end()) {
                map<string, string> entry;
                entry.insert(pair<string, string>(left, it));
                entry.insert(pair<string, string>(right, it2));
                relationships.insert(entry);
			}
		}
	}
	return relationships;
}

// e.g. Follows(7, 8) or Parent(7, 8) or Uses(8, "v")
set<map<string, string>> QueryEvaluator::EvaluateSimpleRelationship(Clause *clause) {
    set<map<string, string>> relationships;
	list<string> relationshipList;
    string left = clause->left;
    switch (clause->relationship) {
	case FOLLOWS:
		relationshipList = pkb.GetFollows(left);
		break;
	case FOLLOWS_STAR:
		relationshipList = pkb.GetAllFollows(left);
		break;
	case PARENT:
		relationshipList = pkb.GetChildren(left);
		break;
	case PARENT_STAR:
		relationshipList = pkb.GetChildrenOfChildren(left);
		break;
	case USES:
    {
        if (!pkb.IsNumber(left)) {     // to remove quotation marks if they exist -- e.g. Uses("main", "count")
            left = left.substr(1, left.size() - 2);
        }
        relationshipList = pkb.GetUsesVariables(left);
        break;
    }
	case MODIFIES:
    {
        if (!pkb.IsNumber(left)) {     // to remove quotation marks if they exist -- e.g. Uses("main", "count")
            left = left.substr(1, left.size() - 2);
        }
        relationshipList = pkb.GetModifiesVariables(left);
        break;
    }
	}
	string right = clause->right;
    if (!pkb.IsNumber(right)) {         // to remove quotation marks if they exist -- e.g. Uses("main", "count")
        right = right.substr(1, right.size() - 2);
    }
	auto it = std::find(relationshipList.begin(), relationshipList.end(), right);
	if (it != relationshipList.end()) {
		relationships = results;
	} else {
		relationships = set<map<string, string>>();
	}
	return relationships;
}

void QueryEvaluator::RemoveDuplicates(list<string>& l) {
    set<string> duplicates;
    auto it = l.begin();
    for (auto & curr : l) {
        if (duplicates.insert(curr).second)
            *it++ = curr;
    }
    l.erase(it,  l.end());
}

// Filters the general results set against the incoming set (of clause results)
void QueryEvaluator::FilterResults(set<map<string, string>>& incoming) {
    set<map<string, string>> newResults;
    for (auto resultsCombination : results) {
        for (auto clauseCombination : incoming) {
            map<string, string> entry = MergeCombinations(resultsCombination, clauseCombination);
            if (!entry.empty())
                newResults.insert(entry);
        }
    }
    results = newResults;
}

// Returns the merged map if there are no conflicting values, and returns empty map if there are
map<string, string> QueryEvaluator::MergeCombinations(map<string, string>& first, map<string, string>& second) {
    map<string, string> merged;
    for (auto & it : second) {
        if (first.find(it.first) != first.end()) {
            if (it.second == first.at(it.first)) {
                merged.insert(pair<string, string>(it.first, it.second));
            } else {
                return map<string, string>();
            }
        } else {
            merged.insert(pair<string, string>(it.first, it.second));
        }
    }
    merged.insert(first.begin(), first.end());
    return merged;
}

list<string> QueryEvaluator::GetAttributeList(const string& attribute) {
    // All results in results set should already have been checked to be valid
    list<string> attributeList;
    for (const auto & result : results)
        attributeList.push_back(result.at(attribute));
    return attributeList;
}

list<string> QueryEvaluator::GetAllOfEntity(Entity entity) {
	list<string> listOfEntity;
	switch (entity) {
	case STATEMENT:
	{
        cout << "Getting statement from PKB" << endl;
		int lastStatementIndex = pkb.GetLastStatementNumber();
		for (int i = 1; i <= lastStatementIndex; i++)
			listOfEntity.push_back(to_string(i));
		break;
	}
	case VARIABLE:
        cout << "Getting variable from PKB" << endl;
		listOfEntity = pkb.GetAllVariables();
		break;
    case CONSTANT:
	    cout << "Getting constant from PKB" << endl;
        listOfEntity = pkb.GetAllConstants();
        break;
	case PROCEDURE:
        cout << "Getting procedure from PKB" << endl;
		listOfEntity = pkb.GetStatementType("procedure");
		break;
	case ASSIGN:
	    cout << "Getting assign from PKB" << endl;
		listOfEntity = pkb.GetStatementType("assign");
		break;
	case WHILE:
        cout << "Getting while from PKB" << endl;
		listOfEntity = pkb.GetStatementType("while");
		break;
	case IF:
        cout << "Getting if from PKB" << endl;
		listOfEntity = pkb.GetStatementType("if");
		break;
	case PRINT:
        cout << "Getting print from PKB" << endl;
		listOfEntity = pkb.GetStatementType("print");
		break;
	case READ:
        cout << "Getting read from PKB" << endl;
		listOfEntity = pkb.GetStatementType("read");
		break;
    case CALL:
        cout << "Getting call from PKB" << endl;
        listOfEntity = pkb.GetStatementType("call");
        break;
    }
	return listOfEntity;
}

string QueryEvaluator::GetUnderscoreKey() {
    underscoreCounter++;
    return "_" + to_string(underscoreCounter);
}
