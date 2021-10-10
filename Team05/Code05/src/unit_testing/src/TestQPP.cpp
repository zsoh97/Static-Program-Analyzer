#include "catch.hpp"
#include "QueryPreprocessor.h"
#include "QueryClause.h"

// test attributes declaration
TEST_CASE("Attribute declaration") {
    PQLQuery pql = ParseQuery("procedure p;  stmt s1,  s2, s3; assign  as, ass; while www ; if ifs; print pnl; read reS; Select s2");
    unordered_map<string, Entity> attributes = pql.GetAttributes();

    REQUIRE(pql.IsQueryValid() == true);
    REQUIRE(pql.GetAttributeToSelect() == "s2");
    REQUIRE(pql.GetSelectEntity() == STATEMENT);

    SECTION("Check attributes are stored properly in map") {
        REQUIRE(attributes.size() == 10);
        unordered_map<string, Entity> attrTest;
        attrTest.insert({"p", PROCEDURE});
        attrTest.insert({"s1", STATEMENT});
        attrTest.insert({"s2", STATEMENT});
        attrTest.insert({"s3", STATEMENT});
        attrTest.insert({"as", ASSIGN});
        attrTest.insert({"ass", ASSIGN});
        attrTest.insert({"www", WHILE});
        attrTest.insert({"ifs", IF});
        attrTest.insert({"pnl", PRINT});
        attrTest.insert({"reS", READ});
        REQUIRE(attributes == attrTest);
    }

    REQUIRE(pql.IsQueryValid() == true);
    REQUIRE(pql.GetAttributeToSelect() == "s2");
    REQUIRE(pql.GetSelectEntity() == STATEMENT);
}

// test Follows creation
TEST_CASE("Follows Clause") {
    SECTION("Follows Clause with underscore") {
        PQLQuery pql = ParseQuery("stmt s; Select s such that Follows(    s, _)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cFollows = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cFollows->IsLeftUnderscore() == false);
            REQUIRE(cFollows->IsRightUnderscore() == true);
            REQUIRE(cFollows->relationship == FOLLOWS);
            REQUIRE(cFollows->left == "s");
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cFollows) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cFollows) == STATEMENT);
            REQUIRE(pql.IsRightAttribute(cFollows) == false);
        }
    }

    SECTION("Follows uses 2 attributes declared") {
        PQLQuery pql = ParseQuery("stmt s1, s2; Select s2 such that     Follows(s2, s1)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cFollows = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cFollows->IsLeftUnderscore() == false);
            REQUIRE(cFollows->IsRightUnderscore() == false);
            REQUIRE(cFollows->left == "s2");
            REQUIRE(cFollows->right == "s1");
            REQUIRE(cFollows->relationship == FOLLOWS);
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cFollows) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cFollows) == STATEMENT);
            REQUIRE(pql.IsRightAttribute(cFollows) == true);
            REQUIRE(pql.GetRightAttributeEntity(cFollows) == STATEMENT);
        }
    }

    SECTION("Follows uses 1 attribute and one stmt number") {
        PQLQuery pql = ParseQuery("stmt s; Select   s such that Follows(s, 4)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cFollows = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cFollows->IsLeftUnderscore() == false);
            REQUIRE(cFollows->IsRightUnderscore() == false);
            REQUIRE(cFollows->relationship == FOLLOWS);
            REQUIRE(cFollows->left == "s");
            REQUIRE(cFollows->right == "4");
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cFollows) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cFollows) == STATEMENT);
            REQUIRE(pql.IsRightAttribute(cFollows) == false);
        }
    }

    SECTION("Follows uses invalid stmt number") {
        PQLQuery pql = ParseQuery("while wow; Select wow such that Follows(-2, 10)");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Follows uses variable") {
        PQLQuery pql = ParseQuery(R"(stmt s; Select wow such that Follows(s, "x")");
        REQUIRE(pql.IsQueryValid() == false);
    }
}

// test Follows* creation
TEST_CASE("Follows* Clause") {
    SECTION("Follows* Clause with underscore") {
        PQLQuery pql = ParseQuery("stmt s; Select s such that Follows*(_, s)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cFollowsStar = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cFollowsStar->IsLeftUnderscore() == true);
            REQUIRE(cFollowsStar->IsRightUnderscore() == false);
            REQUIRE(cFollowsStar->relationship == FOLLOWS_STAR);
            REQUIRE(cFollowsStar->right == "s");
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cFollowsStar) == false);
            REQUIRE(pql.IsRightAttribute(cFollowsStar) == true);
            REQUIRE(pql.GetRightAttributeEntity(cFollowsStar) == STATEMENT);
        }
    }

    SECTION("Follows* uses 2 attributes declared") {
        PQLQuery pql = ParseQuery("stmt s1, s2; Select s2 such that Follows*     (s1, s2)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "s2");
        REQUIRE(pql.GetSelectEntity() == STATEMENT);

        Clause *cFollowsStar = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cFollowsStar->IsLeftUnderscore() == false);
            REQUIRE(cFollowsStar->IsRightUnderscore() == false);
            REQUIRE(cFollowsStar->left == "s1");
            REQUIRE(cFollowsStar->right == "s2");
            REQUIRE(cFollowsStar->relationship == FOLLOWS_STAR);
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cFollowsStar) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cFollowsStar) == STATEMENT);
            REQUIRE(pql.IsRightAttribute(cFollowsStar) == true);
            REQUIRE(pql.GetRightAttributeEntity(cFollowsStar) == STATEMENT);
        }
    }

    SECTION("Follows* uses 1 attribute and one stmt number") {
        PQLQuery pql = ParseQuery("stmt s; Select s such that       Follows*(s, 4)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cFollowsStar = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cFollowsStar->IsLeftUnderscore() == false);
            REQUIRE(cFollowsStar->IsRightUnderscore() == false);
            REQUIRE(cFollowsStar->relationship == FOLLOWS_STAR);
            REQUIRE(cFollowsStar->left == "s");
            REQUIRE(cFollowsStar->right == "4");
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cFollowsStar) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cFollowsStar) == STATEMENT);
            REQUIRE(pql.IsRightAttribute(cFollowsStar) == false);
        }
    }

    SECTION("Follows* uses invalid stmt number") {
        PQLQuery pql = ParseQuery("while wow; Select wow such that Follows* (10, 1!)");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Follows* uses variable") {
        PQLQuery pql = ParseQuery(R"(stmt s; Select wow such that Follows*("v", s)");
        REQUIRE(pql.IsQueryValid() == false);
    }
}

// test Parent creation
TEST_CASE("Parent Clause") {
    SECTION("Parent Clause with underscore") {
        PQLQuery pql = ParseQuery("stmt s; Select s such that Parent(    s, _)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cParent = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cParent->IsLeftUnderscore() == false);
            REQUIRE(cParent->IsRightUnderscore() == true);
            REQUIRE(cParent->relationship == PARENT);
            REQUIRE(cParent->left == "s");
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cParent) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cParent) == STATEMENT);
            REQUIRE(pql.IsRightAttribute(cParent) == false);
        }
    }

    SECTION("Parent uses 2 attributes declared") {
        PQLQuery pql = ParseQuery("stmt s1, s2; Select s2 such that     Parent(s2, s1)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cParent = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cParent->IsLeftUnderscore() == false);
            REQUIRE(cParent->IsRightUnderscore() == false);
            REQUIRE(cParent->left == "s2");
            REQUIRE(cParent->right == "s1");
            REQUIRE(cParent->relationship == PARENT);
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cParent) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cParent) == STATEMENT);
            REQUIRE(pql.IsRightAttribute(cParent) == true);
            REQUIRE(pql.GetRightAttributeEntity(cParent) == STATEMENT);
        }
    }

    SECTION("Parent uses 1 attribute and one stmt number") {
        PQLQuery pql = ParseQuery("stmt s; Select   s such that Parent(s, 20)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cParent = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cParent->IsLeftUnderscore() == false);
            REQUIRE(cParent->IsRightUnderscore() == false);
            REQUIRE(cParent->relationship == PARENT);
            REQUIRE(cParent->left == "s");
            REQUIRE(cParent->right == "20");
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cParent) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cParent) == STATEMENT);
            REQUIRE(pql.IsRightAttribute(cParent) == false);
        }
    }

    SECTION("Parent uses invalid stmt number") {
        PQLQuery pql = ParseQuery("while wow; Select wow such that Parent(-2, 10)");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Parent uses variable") {
        PQLQuery pql = ParseQuery(R"(stmt s; Select wow such that Parent(s, "x")");
        REQUIRE(pql.IsQueryValid() == false);
    }
}

// test Parent* creation
TEST_CASE("Parent* Clause") {
    SECTION("Parent* Clause with underscore") {
        PQLQuery pql = ParseQuery("stmt s; Select s such that Parent*(_, s)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cParentStar = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cParentStar->IsLeftUnderscore() == true);
            REQUIRE(cParentStar->IsRightUnderscore() == false);
            REQUIRE(cParentStar->relationship == PARENT_STAR);
            REQUIRE(cParentStar->right == "s");
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cParentStar) == false);
            REQUIRE(pql.IsRightAttribute(cParentStar) == true);
            REQUIRE(pql.GetRightAttributeEntity(cParentStar) == STATEMENT);
        }
    }

    SECTION("Parent* uses 2 attributes declared") {
        PQLQuery pql = ParseQuery("stmt s1, s2; Select s2 such that Parent*     (s1, s2)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "s2");
        REQUIRE(pql.GetSelectEntity() == STATEMENT);

        Clause *cParentStar = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cParentStar->IsLeftUnderscore() == false);
            REQUIRE(cParentStar->IsRightUnderscore() == false);
            REQUIRE(cParentStar->left == "s1");
            REQUIRE(cParentStar->right == "s2");
            REQUIRE(cParentStar->relationship == PARENT_STAR);
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cParentStar) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cParentStar) == STATEMENT);
            REQUIRE(pql.IsRightAttribute(cParentStar) == true);
            REQUIRE(pql.GetRightAttributeEntity(cParentStar) == STATEMENT);
        }
    }

    SECTION("Parent* uses 1 attribute and one stmt number") {
        PQLQuery pql = ParseQuery("stmt s; Select s such that       Parent*(12, s)");
        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        REQUIRE(clauses->size() == 1);
        REQUIRE(pql.IsQueryValid() == true);

        Clause *cParentStar = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(cParentStar->IsLeftUnderscore() == false);
            REQUIRE(cParentStar->IsRightUnderscore() == false);
            REQUIRE(cParentStar->relationship == PARENT_STAR);
            REQUIRE(cParentStar->right == "s");
            REQUIRE(cParentStar->left == "12");
        }

        SECTION("Check attribute methods") {
            REQUIRE(pql.IsLeftAttribute(cParentStar) == false);
            REQUIRE(pql.IsRightAttribute(cParentStar) == true);
            REQUIRE(pql.GetRightAttributeEntity(cParentStar) == STATEMENT);
        }
    }

    SECTION("Parent* uses invalid stmt number") {
        PQLQuery pql = ParseQuery("while wow; Select wow such that Parent* (10, 1!)");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Parent* uses variable") {
        PQLQuery pql = ParseQuery(R"(stmt s; Select wow such that Parent*("v", s)");
        REQUIRE(pql.IsQueryValid() == false);
    }
}

// test Uses creation
TEST_CASE("Uses Clause") {
    SECTION("Uses left underscore") {
        PQLQuery pql = ParseQuery(R"(variable v; Select v such that Uses(_, v))");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Uses has invalid variable") {
        PQLQuery pql = ParseQuery(R"(while w; Select w such that Uses(w, "123x"))");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Uses uses stmt number and attribute variable") {
        PQLQuery pql = ParseQuery(R"(variable c; Select c such that Uses(9, c))");
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "c");
        REQUIRE(pql.GetSelectEntity() == VARIABLE);

        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        Clause *cUses = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(clauses->size() == 1);
            REQUIRE(cUses->left == "9");
            REQUIRE(cUses->right == "c");
            REQUIRE(cUses->relationship == USES);
        }

        SECTION("Check attribute and methods") {
            REQUIRE(pql.IsLeftAttribute(cUses) == false);
            REQUIRE(pql.IsRightAttribute(cUses) == true);
            REQUIRE(pql.GetRightAttributeEntity(cUses) == VARIABLE);
        }
    }

    SECTION("Uses uses attribute and underscore variable") {
        PQLQuery pql = ParseQuery(R"(assign a; Select a such that Uses(a, _))");
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "a");
        REQUIRE(pql.GetSelectEntity() == ASSIGN);

        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        Clause *cUses = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(clauses->size() == 1);
            REQUIRE(cUses->left == "a");
            REQUIRE(cUses->right == "_");
            REQUIRE(cUses->relationship == USES);
            REQUIRE(cUses->IsRightUnderscore() == true);
            REQUIRE(cUses->IsLeftUnderscore() == false);
        }

        SECTION("Check attribute and methods") {
            REQUIRE(pql.IsLeftAttribute(cUses) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cUses) == ASSIGN);
            REQUIRE(pql.IsRightAttribute(cUses) == false);
        }
    }

    SECTION("Uses uses attribute and variable") {
        PQLQuery pql = ParseQuery(R"(while w; Select w such that Uses(w, "abc"))");
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "w");
        REQUIRE(pql.GetSelectEntity() == WHILE);

        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        Clause *cUses = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(clauses->size() == 1);
            REQUIRE(cUses->left == "w");
            REQUIRE(cUses->right == "\"abc\"");
            REQUIRE(cUses->relationship == USES);
        }

        SECTION("Check attribute and methods") {
            REQUIRE(pql.IsLeftAttribute(cUses) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cUses) == WHILE);
            REQUIRE(pql.IsRightAttribute(cUses) == false);
        }
    }
}

// test Modifies creation
TEST_CASE("Modifies Clause") {
    SECTION("Modifies left underscore") {
        PQLQuery pql = ParseQuery(R"(variable v; Select v such that Modifies(_, v))");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Modifies uses invalid proc name") {
        PQLQuery pql = ParseQuery(R"(while w; Select w such that Modifies("1stProc", 9))");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Modifies uses stmt number and attribute variable") {
        PQLQuery pql = ParseQuery(R"(variable c; Select c such that Modifies(9, c))");
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "c");
        REQUIRE(pql.GetSelectEntity() == VARIABLE);

        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        Clause *cModifies = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(clauses->size() == 1);
            REQUIRE(cModifies->left == "9");
            REQUIRE(cModifies->right == "c");
            REQUIRE(cModifies->relationship == MODIFIES);
        }

        SECTION("Check attribute and methods") {
            REQUIRE(pql.IsLeftAttribute(cModifies) == false);
            REQUIRE(pql.IsRightAttribute(cModifies) == true);
            REQUIRE(pql.GetRightAttributeEntity(cModifies) == VARIABLE);
        }
    }

    SECTION("Modifies uses attribute and underscore variable") {
        PQLQuery pql = ParseQuery(R"(if ifs; Select ifs such that Modifies(ifs, _))");
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "ifs");
        REQUIRE(pql.GetSelectEntity() == IF);

        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        Clause *cModifies = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(clauses->size() == 1);
            REQUIRE(cModifies->left == "ifs");
            REQUIRE(cModifies->right == "_");
            REQUIRE(cModifies->relationship == MODIFIES);
            REQUIRE(cModifies->IsRightUnderscore() == true);
            REQUIRE(cModifies->IsLeftUnderscore() == false);
        }

        SECTION("Check attribute and methods") {
            REQUIRE(pql.IsLeftAttribute(cModifies) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cModifies) == IF);
            REQUIRE(pql.IsRightAttribute(cModifies) == false);
        }
    }

    SECTION("Modifies uses attribute and variable") {
        PQLQuery pql = ParseQuery(R"(read re; Select re such that Modifies(re, "abc"))");
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "re");
        REQUIRE(pql.GetSelectEntity() == READ);

        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        Clause *cModifies = clauses->at(0).get();
        SECTION("Check Clause creation") {
            REQUIRE(clauses->size() == 1);
            REQUIRE(cModifies->left == "re");
            REQUIRE(cModifies->right == "\"abc\"");
            REQUIRE(cModifies->relationship == MODIFIES);
        }

        SECTION("Check attribute and methods") {
            REQUIRE(pql.IsLeftAttribute(cModifies) == true);
            REQUIRE(pql.GetLeftAttributeEntity(cModifies) == READ);
            REQUIRE(pql.IsRightAttribute(cModifies) == false);
        }
    }
}

// test Pattern creation
TEST_CASE("Pattern Clause") {
    SECTION("Missing assign attribute") {
        PQLQuery pql = ParseQuery(R"(assign a; variable v; Select a pattern (v, _))");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Pattern attribute is not ASSIGN") {
        PQLQuery pql = ParseQuery(R"(assign a; while w; Select a pattern w(_, _))");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Pattern variable is not a valid variable") {
        PQLQuery pql = ParseQuery(R"(assign a; Select a pattern a      ("0mg", _)");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Pattern with incomplete partial expression format") {
        PQLQuery pql = ParseQuery(R"(assign a; variable v; Select a pattern a (v, _"x + y"))");
        REQUIRE(pql.IsQueryValid() == false);
    }

    SECTION("Pattern with complete expression and variable attribute") {
        PQLQuery pql = ParseQuery(R"(assign a; variable var; Select var pattern a (var, "x +        y"))");
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "var");
        REQUIRE(pql.GetSelectEntity() == VARIABLE);

        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        Pattern *cPattern = dynamic_cast<Pattern*>(clauses->at(0).get());
        SECTION("Check Clause creation") {
            REQUIRE(clauses->size() == 1);
            REQUIRE(cPattern->left == "var");
            REQUIRE(cPattern->right == "x +        y");
            REQUIRE(cPattern->relationship == PATTERN);
            REQUIRE(cPattern->assignAttribute == "a");
            REQUIRE(cPattern->isExpressionPartial == false);
        }

        SECTION("Check attribute and methods") {
            REQUIRE(pql.IsLeftAttribute(cPattern) == true);
            REQUIRE(pql.IsLeftPatternStringAttribute(cPattern) == true); // should be the same as the above
            REQUIRE(pql.GetLeftAttributeEntity(cPattern) == VARIABLE);
            REQUIRE(pql.IsRightAttribute(cPattern) == false);
        }
    }

    SECTION("Pattern with partial expression and defined variable") {
        PQLQuery pql = ParseQuery(R"(assign a; Select a pattern a ("z", _"  x+       y"_))");
        REQUIRE(pql.IsQueryValid() == true);
        REQUIRE(pql.GetAttributeToSelect() == "a");
        REQUIRE(pql.GetSelectEntity() == ASSIGN);

        vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
        Pattern *cPattern = dynamic_cast<Pattern*>(clauses->at(0).get());
        SECTION("Check Clause creation") {
            REQUIRE(clauses->size() == 1);
            REQUIRE(cPattern->left == "\"z\"");
            REQUIRE(cPattern->right == "  x+       y");
            REQUIRE(cPattern->relationship == PATTERN);
            REQUIRE(cPattern->assignAttribute == "a");
            REQUIRE(cPattern->isExpressionPartial == true);
        }

        SECTION("Check attribute and methods") {
            REQUIRE(pql.IsLeftAttribute(cPattern) == false);
            REQUIRE(pql.IsLeftPatternStringAttribute(cPattern) == false); // should be the same as the above
            REQUIRE(pql.IsRightAttribute(cPattern) == false);
        }
    }
}

// test such that, Pattern combination
TEST_CASE("Such that then Pattern Clauses") {
    PQLQuery pql = ParseQuery(R"(assign a; stmt s; variable v; Select s such that Uses(s, v) pattern a (v, _))");
    REQUIRE(pql.IsQueryValid() == true);
    REQUIRE(pql.GetAttributeToSelect() == "s");
    REQUIRE(pql.GetSelectEntity() == STATEMENT);

    vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
    REQUIRE(clauses->size() == 2);

    SECTION("Test first parsed clause (such that)") {
        Clause *cSuchThat = clauses->at(0).get();
        REQUIRE(cSuchThat->left == "s");
        REQUIRE(cSuchThat->right == "v");
        REQUIRE(cSuchThat->relationship == USES);

        REQUIRE(pql.GetLeftAttributeEntity(cSuchThat) == STATEMENT);
        REQUIRE(pql.GetRightAttributeEntity(cSuchThat) == VARIABLE);
    }

    SECTION("Test second parsed clause (pattern)") {
        Pattern* cPattern = dynamic_cast<Pattern*>(clauses->at(1).get());
        REQUIRE(cPattern->left == "v");
        REQUIRE(cPattern->right == "");
        REQUIRE(cPattern->relationship == PATTERN);
        REQUIRE(cPattern->assignAttribute == "a");
        REQUIRE(cPattern->isExpressionPartial == true);
    }
}

// test Pattern, such that combination
TEST_CASE("Pattern then such that Clauses") {
    PQLQuery pql = ParseQuery(R"(assign aa; stmt s1, s2; Select s1 pattern  aa ("y", _"x+1"_) such that Parent*(s1, s2))");
    REQUIRE(pql.IsQueryValid() == true);
    REQUIRE(pql.GetAttributeToSelect() == "s1");
    REQUIRE(pql.GetSelectEntity() == STATEMENT);

    vector<unique_ptr<Clause>> * clauses = pql.GetClauses();
    REQUIRE(clauses->size() == 2);

    SECTION("Test second parsed clause (pattern)") {
        Pattern *cPattern = dynamic_cast<Pattern *>(clauses->at(0).get());
        REQUIRE(cPattern->isExpressionPartial == true);
        REQUIRE(cPattern->left == "\"y\"");
        REQUIRE(cPattern->right == "x+1");
        REQUIRE(cPattern->relationship == PATTERN);
        REQUIRE(cPattern->assignAttribute == "aa");
    }

    SECTION("Test first parsed clause (such that)") {
        Clause *cSuchThat = clauses->at(1).get();
        REQUIRE(cSuchThat->left == "s1");
        REQUIRE(cSuchThat->right == "s2");
        REQUIRE(cSuchThat->relationship == PARENT_STAR);

        REQUIRE(pql.GetLeftAttributeEntity(cSuchThat) == STATEMENT);
        REQUIRE(pql.GetRightAttributeEntity(cSuchThat) == STATEMENT);
    }
}