#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>

using namespace std;

#include "QueryResultProjector.h"

// might not be necessary...
void QueryResultProjector::InsertResults(vector<string> v, list<string> &l) {
	// need to sort?
	
	// Convert vector to list
	std::copy(v.begin(), v.end(), back_inserter(l));
}