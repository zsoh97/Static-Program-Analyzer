#include "catch.hpp"
#include "PKB.h"

TEST_CASE("Follows Test") {
    PKB pkb;
    pkb.SetParent("procedure", "1", false);
    pkb.SetParent("procedure", "2", false);
    REQUIRE(pkb.GetFollows("1") == list<string>{"2"});
    REQUIRE(pkb.GetFollowedBy("2") == list<string>{"1"});
}

TEST_CASE("Follows* Test") {
    PKB pkb;
    pkb.SetParent("1", "2", false);
    pkb.SetParent("1", "3", false);
    pkb.SetParent("1", "4", false);
    REQUIRE(pkb.GetAllFollows("2") == list<string>{"3", "4"});
}

TEST_CASE("Parent Test") {
    PKB pkb;
    pkb.SetParent("1", "2", false);
    pkb.SetParent("2", "3", false);
    pkb.SetParent("3", "4", false);
    pkb.SetParent("1", "5", false);

    REQUIRE(pkb.GetParent("4") == list<string>{"3"});
    REQUIRE(pkb.GetParent("3") == list<string>{"2"});
    REQUIRE(pkb.GetParent("2") == list<string>{"1"});

    REQUIRE(pkb.GetChildren("1") == list<string>{"2", "5"});
    REQUIRE(pkb.GetChildren("2") == list<string>{"3"});
    REQUIRE(pkb.GetChildren("3") == list<string>{"4"});
    REQUIRE(pkb.GetChildren("4").empty());
}

TEST_CASE("Parent* Test") {
    PKB pkb;
    pkb.SetParent("1", "2", false);
    pkb.SetParent("2", "3", false);
    pkb.SetParent("3", "4", false);
    pkb.SetParent("1", "5", false);

    REQUIRE(pkb.GetChildrenOfChildren("1") == list<string>{"2", "3", "4", "5"});
    REQUIRE(pkb.GetChildrenOfChildren("2") == list<string>{"3", "4"});
    REQUIRE(pkb.GetAllParents("4") == list<string>{"1", "2", "3"});
    REQUIRE(pkb.GetAllParents("5") == list<string>{"1"});
}

TEST_CASE("Uses Test") {
    PKB pkb;
    pkb.SetParent("0", "1", false);
    pkb.SetParent("1", "2", false);
    pkb.SetParent("2", "3", false);
    pkb.SetParent("3", "4", false);
    pkb.SetParent("1", "5", false);
    pkb.SetUses("3", "v");
    pkb.SetUses("5", "x");

    list<string> uses = pkb.GetUsesVariables("1");
    uses.sort();
    REQUIRE(uses == list<string>{"v", "x"});

    list<string> usedBy = pkb.GetUsedByStatements("v");
    usedBy.sort();
    REQUIRE(usedBy == list<string>{"1", "2", "3"});
}

TEST_CASE("Modifies Test") {
    PKB pkb;
    pkb.SetParent("0", "1", false);
    pkb.SetParent("1", "2", false);
    pkb.SetParent("2", "3", false);
    pkb.SetParent("3", "4", false);
    pkb.SetParent("1", "5", false);
    pkb.SetModifies("3", "v");
    pkb.SetModifies("5", "x");

    list<string> modifies = pkb.GetModifiesVariables("1");
    modifies.sort();
    REQUIRE(modifies == list<string>{"v", "x"});

    list<string> modifiedBy = pkb.GetModifiedByStatements("v");
    modifiedBy.sort();
    REQUIRE(modifiedBy == list<string>{"1", "2", "3"});
}

TEST_CASE("Variable Test") {
    PKB pkb;
    pkb.InsertVariable("x");
    pkb.InsertVariable("bubbletea");
    pkb.InsertVariable("a1b2c3");
    set<string> testSet = {"x", "a1b2c3", "bubbletea"};
    REQUIRE(pkb.GetAllVariables() == list<string>(testSet.begin(), testSet.end()));
}

TEST_CASE("Procedure Test") {
    PKB pkb;
    pkb.InsertProcedure("eater");
    pkb.InsertProcedure("sleeper");
    pkb.InsertProcedure("waker");
    set<string> testSet = {"sleeper", "eater", "waker"};
    REQUIRE(pkb.GetAllProcedures() == list<string>(testSet.begin(), testSet.end()));
}

TEST_CASE("Shunting Yard String Converter Test") {
    PKB pkb;

    string INPUT_1_1 = "2 + 3";
    string INPUT_1_2 = "      2 + 3";
    string INPUT_1_3 = " 2     +     3           ";
    string INPUT_1_4 = "2+3";
    string OUTPUT_1 = "2 3 +";

    string INPUT_2_1 = "7 - 24 / 8 * 4 + 6";
    string OUTPUT_2 = "7 24 8 / 4 * - 6 +";

    string INPUT_3_1 = "(17 - 6 / 2) + 4 * 3";
    string OUTPUT_3 = "17 6 2 / - 4 3 * +";

    string INPUT_4 = "";
    string OUTPUT_4 = "";

    string INPUT_5 = "x + 001";
    string OUTPUT_5 = "x 1 +";

    string INPUT_6 = "((x+1)+2)*3";
    string OUTPUT_6 = "x 1 + 2 + 3 *";

    string INPUT_7 = "x % 4";
    string OUTPUT_7 = "x 4 %";

    REQUIRE(pkb.getShuntingYardString(INPUT_1_1) == OUTPUT_1);
    REQUIRE(pkb.getShuntingYardString(INPUT_1_2) == OUTPUT_1);
    REQUIRE(pkb.getShuntingYardString(INPUT_1_3) == OUTPUT_1);
    REQUIRE(pkb.getShuntingYardString(INPUT_1_4) == OUTPUT_1);
    REQUIRE(pkb.getShuntingYardString(INPUT_2_1) == OUTPUT_2);
    REQUIRE(pkb.getShuntingYardString(INPUT_3_1) == OUTPUT_3);
    REQUIRE(pkb.getShuntingYardString(INPUT_4) == OUTPUT_4);
    REQUIRE(pkb.getShuntingYardString(INPUT_5) == OUTPUT_5);
    REQUIRE(pkb.getShuntingYardString(INPUT_6) == OUTPUT_6);
    REQUIRE(pkb.getShuntingYardString(INPUT_7) == OUTPUT_7);
}

TEST_CASE("Pattern Test") {

    PKB pkb;
    pkb.InsertVariable("x");
    pkb.InsertVariable("y");
    pkb.SetModifies("7", "x");
    pkb.SetPattern("7", "2 + 3 + 4 + xy");
    pkb.SetModifies("8", "y");
    pkb.SetPattern("8", "abc + 3");

    REQUIRE(pkb.GetPatternStatements("x", "2 + 3", true) == list<string>{"7"});
    REQUIRE_FALSE(pkb.GetPatternStatements("x", "x", true) == list<string>{"7"});
    REQUIRE(pkb.GetPatternStatements("", "x", true).empty());
}

TEST_CASE("Pattern test advanced") {
    //    procedure Example {
//1            x = 1 + 2 + 3;
//2            z = 3;
//3            i = 5;
//4            while (i!=0) {
//5                x = x - 1;
//6                if (x==1) then {
//7                            z = x + 1; }
//                else {
//8                    y = z + x; }
//                z = z + x + i;
//                read x;
//                i = i - 1; }
//            print z; }

    PKB pkb;
    pkb.InsertVariable("x");
    pkb.InsertVariable("y");
    pkb.InsertVariable("z");
    pkb.InsertVariable("i");
    pkb.SetModifies("1", "x");
    pkb.SetStatementType("assign", "1");
    pkb.SetStatementType("assign", "2");
    pkb.SetStatementType("assign", "3");
    pkb.SetStatementType("assign", "5");
    pkb.SetUses("4", "i");
    pkb.SetUses("5", "x");
    pkb.SetModifies("5", "x");
    pkb.SetPattern("5", "x - 1");
    pkb.SetUses("6", "x");
    pkb.SetModifies("7", "z");
    pkb.SetUses("7", "x");
    pkb.SetPattern("7", "x + 1");

    REQUIRE(pkb.GetPatternStatements("_", "x + 1", true) == list<string>{"7"});
    REQUIRE(pkb.GetPatternStatements("x", "", true) == list<string>{"1", "5"});

}