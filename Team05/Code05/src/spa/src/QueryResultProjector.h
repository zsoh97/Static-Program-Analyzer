#pragma once

#include "QueryPreprocessor.h"
#include <string>
#include <vector>
#include <list>

using namespace std;

class QueryResultProjector {
public:
	void InsertResults(vector<string> v, list<string> &l);
};