#include "QueryClause.h"
#include <string>
#include <iostream>

Relationship StringToRelationship(string rsType) {
    // convert the string of the relationship to the Relationship enum type
    if (rsType.compare("Follows") == 0) {
        return FOLLOWS;
    } else if (rsType.compare("Follows*") == 0) {
        return FOLLOWS_STAR;
    } else if (rsType.compare("Parent") == 0) {
        return PARENT;
    } else if (rsType.compare("Parent*") == 0) {
        return PARENT_STAR;
    } else if (rsType.compare("Uses") == 0) {
        return USES;
    } else if (rsType.compare("Modifies") == 0) {
        return MODIFIES;
    } else if (rsType.compare("pattern") == 0) {
        return PATTERN;
    }
    return static_cast<Relationship>(NULL);
}

Clause::Clause(string left, string right) {
    this->left = left;
    this->right = right;
    //this->relationship = NONE;
}

Clause::Clause(string left, string right, string rsType) {
    this->left = left;
    this->right = right;
    //this->relationship = NONE;
    this->relationship = StringToRelationship(rsType);

//    if (relationship == NONE) {
//        // rsType does not match any string in the relationship
//        // string may be parsed wrongly
//        // throw exceptions
//    }
}

void Clause::SetRelationship(Relationship rs) {
    this->relationship = rs;
}

string Clause::GetRelationshipString() {
    switch (relationship) {
        case FOLLOWS:
            return "Follows";
        case FOLLOWS_STAR:
            return "FollowsStar";
        case PARENT:
            return "Parent";
        case PARENT_STAR:
            return "ParentStar";
        case USES:
            return "Uses";
        case MODIFIES:
            return "Modifies";
        default:
            return "None";
    }
}

bool Clause::IsLeftUnderscore() {
    if (this->left == "_") {
        return true;
    }
    return false;
}

bool Clause::IsRightUnderscore() {
    if (this->right == "_") {
        return true;
    }
    return false;
}

Pattern::Pattern(string left, string right, string assignAttr)
        : Clause(left, right), assignAttribute(assignAttr) {
    this->relationship = PATTERN;
    this->isExpressionPartial = false;

    if (right.compare("_") == 0) {
        this->isExpressionPartial = true;
        this->right = "";
    } else {
        // check for partial expression
        size_t usPos = right.find("_");
        if (usPos == 0 || usPos == right.size() - 1) { // partial expression
            if (right[0] == right[right.size() - 1]) {
                this->isExpressionPartial = true;
                this->right = Trim(right.substr(1, right.size() - 2));
            } else { // if partial expression does not start and end with _
                throw InvalidQueryException();
            }
        }

        // removing quotations
        size_t qPos = this->right.find('"');
        if (qPos == 0 || qPos == right.size() - 1) {
            if (this->right[0] == this->right[this->right.size() - 1]) { // remove quotes
                this->right = this->right.substr(1, this->right.size() - 2);
            } else { // mismatched quotations
                throw InvalidQueryException();
            }
        } else if (qPos == string::npos) {
            throw InvalidQueryException();
        }

        // pattern expression should not be empty
        if (Trim(this->right).empty()) {
            throw InvalidQueryException();
        }
    }
}

string Pattern::GetRelationshipString() {
    return "pattern";
}

const string WHITESPACE = "\t\n\v\f\r ";

string TrimLeft(string str) {
    str.erase(0, str.find_first_not_of(WHITESPACE));
    return str;
}

string TrimRight(string str) {
    str.erase(str.find_last_not_of(WHITESPACE) + 1);
    return str;
}

string Trim(string str) {
    return TrimLeft(TrimRight(std::move(str)));
}
