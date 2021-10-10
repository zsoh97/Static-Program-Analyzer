//
// Created by grant on 23/9/2020.
//

#include "catch.hpp"
#include "QueryEvaluator.h"
#include "PKB.h"

TEST_CASE("No clause") {
    PKB TestPKB;
    TestPKB.SetParent("procedure", "1", false);
    TestPKB.SetParent("procedure", "2", false);
    TestPKB.SetParent("procedure", "3", false);
    TestPKB.SetStatementType("assign", "1");
    TestPKB.SetStatementType("assign", "2");
    TestPKB.SetStatementType("read", "2");

    PQLQuery pql = ParseQuery("stmt s; Select s");
    QueryEvaluator evaluator1(TestPKB);
    list<string> result = evaluator1.EvaluateQuery(pql);
    list<string> expected = list<string>({"1", "2", "3"});
    bool identical = result.size() == expected.size();
    for (auto &it : expected) {
        auto r = std::find(result.begin(), result.end(), it);
        if (r == result.end()) {
            identical = false;
            break;
        }
    }
    REQUIRE(identical);
}

TEST_CASE("Such that clause") {
    SECTION("Follows") {
        PKB TestPKB;
        TestPKB.SetParent("procedure", "1", false);
        TestPKB.SetParent("procedure", "2", false);
        TestPKB.SetStatementType("assign", "1");
        TestPKB.SetStatementType("assign", "2");

        PQLQuery pql = ParseQuery("stmt s; Select s such that Follows(1, s)");
        QueryEvaluator evaluator1(TestPKB);
        list<string> result = evaluator1.EvaluateQuery(pql);
        list<string> expected = list<string>({"2"});
        bool identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);

        pql = ParseQuery("stmt s; Select s such that Follows(s, 2)");
        QueryEvaluator evaluator2(TestPKB);
        result = evaluator2.EvaluateQuery(pql);
        expected = list<string>({"1"});
        identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);
    }

    SECTION("Follows*") {
        PKB TestPKB;
        TestPKB.SetParent("procedure", "1", false);
        TestPKB.SetParent("procedure", "2", false);
        TestPKB.SetParent("procedure", "3", false);
        TestPKB.SetStatementType("assign", "1");
        TestPKB.SetStatementType("assign", "2");
        TestPKB.SetStatementType("assign", "3");

        PQLQuery pql = ParseQuery("stmt s; Select s such that Follows*(1, s)");
        QueryEvaluator evaluator1(TestPKB);
        list<string> result = evaluator1.EvaluateQuery(pql);
        list<string> expected = list<string>({"2", "3"});
        bool identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);

        pql = ParseQuery("stmt s; Select s such that Follows*(s, 3)");
        QueryEvaluator evaluator2(TestPKB);
        result = evaluator2.EvaluateQuery(pql);
        expected = list<string>({"1", "2"});
        identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);
    }

    SECTION("Parent") {
        PKB TestPKB;
        TestPKB.SetParent("procedure", "1", false);
        TestPKB.SetParent("1", "2", false);
        TestPKB.SetStatementType("while", "1");
        TestPKB.SetStatementType("assign", "2");

        PQLQuery pql = ParseQuery("stmt s; Select s such that Parent(1, s)");
        QueryEvaluator evaluator1(TestPKB);
        list<string> result = evaluator1.EvaluateQuery(pql);
        list<string> expected = list<string>({"2"});
        bool identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);

        pql = ParseQuery("stmt s; Select s such that Parent(s, 2)");
        QueryEvaluator evaluator2(TestPKB);
        result = evaluator2.EvaluateQuery(pql);
        expected = list<string>({"1"});
        identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);
    }
    SECTION("Parent*") {
        PKB TestPKB;
        TestPKB.SetParent("procedure", "1", false);
        TestPKB.SetParent("1", "2", false);
        TestPKB.SetParent("2", "3", false);
        TestPKB.SetStatementType("while", "1");
        TestPKB.SetStatementType("while", "2");
        TestPKB.SetStatementType("assign", "3");

        PQLQuery pql = ParseQuery("stmt s; Select s such that Parent*(1, s)");
        QueryEvaluator evaluator1(TestPKB);
        list<string> result = evaluator1.EvaluateQuery(pql);
        list<string> expected = list<string>({"2", "3"});
        bool identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);

        pql = ParseQuery("stmt s; Select s such that Parent*(s, 3)");
        QueryEvaluator evaluator2(TestPKB);
        result = evaluator2.EvaluateQuery(pql);
        expected = list<string>({"1", "2"});
        identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);
    }

    SECTION("Uses") {
        PKB TestPKB;
        TestPKB.SetParent("procedure", "1", false);
        TestPKB.SetParent("procedure", "2", false);
        TestPKB.SetStatementType("assign", "1");
        TestPKB.SetStatementType("assign", "2");
        TestPKB.SetUses("1", "alice");
        TestPKB.SetUses("2", "bob");
        TestPKB.InsertVariable("alice");
        TestPKB.InsertVariable("bob");

        PQLQuery pql = ParseQuery("variable v; Select v such that Uses(1, v)");
        QueryEvaluator evaluator1(TestPKB);
        list<string> result = evaluator1.EvaluateQuery(pql);
        list<string> expected = list<string>({"alice"});
        bool identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);

        pql = ParseQuery("stmt s; Select s such that Uses(s, \"bob\")");
        QueryEvaluator evaluator2(TestPKB);
        result = evaluator2.EvaluateQuery(pql);
        expected = list<string>({"2"});
        identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);
    }

    SECTION("Modifies") {
        PKB TestPKB;
        TestPKB.SetParent("procedure", "1", false);
        TestPKB.SetParent("procedure", "2", false);
        TestPKB.SetStatementType("assign", "1");
        TestPKB.SetStatementType("assign", "2");
        TestPKB.SetModifies("1", "alice");
        TestPKB.SetModifies("2", "bob");
        TestPKB.InsertVariable("alice");
        TestPKB.InsertVariable("bob");

        PQLQuery pql = ParseQuery("variable v; Select v such that Modifies(1, v)");
        QueryEvaluator evaluator1(TestPKB);
        list<string> result = evaluator1.EvaluateQuery(pql);
        list<string> expected = list<string>({"alice"});
        bool identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);

        pql = ParseQuery("stmt s; Select s such that Modifies(s, \"bob\")");
        QueryEvaluator evaluator2(TestPKB);
        result = evaluator2.EvaluateQuery(pql);
        expected = list<string>({"2"});
        identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);
    }
}

TEST_CASE("Pattern clause") {
    SECTION("Unknown LHS variable") {
        PKB TestPKB;
        TestPKB.SetParent("procedure", "1", false);
        TestPKB.SetParent("procedure", "2", false);
        TestPKB.SetStatementType("assign", "1");
        TestPKB.SetStatementType("assign", "2");
        TestPKB.SetModifies("1", "ab");
        TestPKB.SetModifies("2", "bc");
        TestPKB.SetUses("1", "alice");
        TestPKB.SetUses("1", "bob");
        TestPKB.SetUses("2", "bob");
        TestPKB.SetUses("2", "charlie");
        TestPKB.SetPattern("1", "alice + bob");
        TestPKB.SetPattern("2", "bob + charlie");
        TestPKB.InsertVariable("alice");
        TestPKB.InsertVariable("bob");
        TestPKB.InsertVariable("charlie");
        TestPKB.InsertVariable("ab");
        TestPKB.InsertVariable("bc");

        PQLQuery pql = ParseQuery("variable v; assign a; Select v pattern a (v, _\"bob\"_)");
        QueryEvaluator evaluator1(TestPKB);
        list<string> result = evaluator1.EvaluateQuery(pql);
        list<string> expected = list<string>({"ab", "bc"});
        bool identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);
    }

    SECTION("Known LHS variable") {
        PKB TestPKB;
        TestPKB.SetParent("procedure", "1", false);
        TestPKB.SetParent("procedure", "2", false);
        TestPKB.SetStatementType("assign", "1");
        TestPKB.SetStatementType("assign", "2");
        TestPKB.SetModifies("1", "ab");
        TestPKB.SetModifies("2", "bc");
        TestPKB.SetUses("1", "alice");
        TestPKB.SetUses("1", "bob");
        TestPKB.SetUses("2", "bob");
        TestPKB.SetUses("2", "charlie");
        TestPKB.SetPattern("1", "alice + bob");
        TestPKB.SetPattern("2", "bob + charlie");
        TestPKB.InsertVariable("alice");
        TestPKB.InsertVariable("bob");
        TestPKB.InsertVariable("charlie");

        PQLQuery pql = ParseQuery("assign a; Select a pattern a (\"ab\", _)");
        QueryEvaluator evaluator1(TestPKB);
        list<string> result = evaluator1.EvaluateQuery(pql);
        list<string> expected = list<string>({"1"});
        bool identical = result.size() == expected.size();
        for (auto &it : expected) {
            auto r = std::find(result.begin(), result.end(), it);
            if (r == result.end()) {
                identical = false;
                break;
            }
        }
        REQUIRE(identical);
    }
}