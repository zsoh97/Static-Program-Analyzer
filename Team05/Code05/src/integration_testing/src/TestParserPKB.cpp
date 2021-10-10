//
// Created by zsoh9 on 19/9/2020.
//

#include "catch.hpp"
#include "Parser.cpp"

TEST_CASE("invalid source syntax") {
    SECTION("statement declaration outside of procedure"){
        PKB TestPKB;
        std::string TestInput = "x=1;";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("Procedure declaration without space between identifier and name") {
        PKB TestPKB;
        std::string TestInput = "procedureABC{";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("No 'if' statements found before else"){
        PKB TestPKB;
        std::string TestInput = "procedure ABC{{}else{x=x+1;}}";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("No 'else' after 'if' block") {
        PKB TestPKB;
        std::string TestInput = "procedure ABC{if(x==1)then{x=x+1;}}";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("Procedure in Procedure"){
        PKB TestPKB;
        std::string TestInput = "procedure ABC{procedure XYZ{x=1;}};";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }

    SECTION("Missing 'then' in 'if' block") {
        PKB TestPKB;
        std::string TestInput = "procedure ABC{if(x==1){x=x+1;}else{x=x-1;}}";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("No procedure given") {
        PKB TestPKB;
        std::string TestInput = "ABC{";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("Read identifier but var name not separated by space"){
        PKB TestPKB;
        std::string TestInput = "procedure ABC{ready;};";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("Call identifier but var name not separated by space"){
        PKB TestPKB;
        std::string TestInput = "procedure ABC{callme;};";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("print identifier but var name not separated by space"){
        PKB TestPKB;
        std::string TestInput = "procedure ABC{printf;};";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("unknown statements") {
        PKB TestPKB;
        std::string TestInput = "procedure ABC{x1;}";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
    SECTION("unmatched brackets") {
        PKB TestPKB;
        std::string TestInput = "procedure ABC{x1;}}";
        REQUIRE(ParseString(TestInput, TestPKB) == -1);
    }
}

TEST_CASE("all statement types are populated correctly") {
    std::string TestInput = "procedure ABC{read y;while(y>x){call XYZ;}"
                            "if(y>=2)then{y=y-1;}else{print y;}}";
    PKB TestPKB;
    REQUIRE(ParseString(TestInput, TestPKB) == 0);
    //Check values in PKB
}

TEST_CASE("if-if") {
    PKB TestPKB;
    std::string TestInput = "procedure ifInsideIf {a = 1;if (b == 2) then "
                            "{c = 3;if (d == 4) then {e = 5;} else {f = 6;}"
                            "g = 7;} else {h = 8;}i = 9;}";
    REQUIRE(ParseString(TestInput, TestPKB) == 0);
    //Check values in PKB
}

TEST_CASE("while-if") {
    PKB TestPKB;
    std::string TestInput = "procedure ifInsideWhile {r = 18;while (s == 19) "
                            "{t = 20;if (u == 21) then {v = 22;} else {w = 23;}"
                            "x = 24;}y = 25;}";
    REQUIRE(ParseString(TestInput, TestPKB) == 0);
    //Check values in PKB
}

TEST_CASE("if-while") {
    PKB TestPKB;
    std::string TestInput = "procedure whileInsideIf {j = 10;if (k == 11) then "
                            "{l = 12;while (m == 13) {n = 14;}o = 15;} else "
                            "{p = 16;}q = 17;}";
    REQUIRE(ParseString(TestInput, TestPKB) == 0);
    //Check values in PKB
}

TEST_CASE("while-while") {
    PKB TestPKB;
    std::string TestInput = "procedure whileInsideWhile {z = 26;while "
                            "(aa == 27) {ab = 28;while (ac == 29) {ad = 30;}"
                            "ae = 31;}af = 32;}";
    REQUIRE(ParseString(TestInput, TestPKB) == 0);
    //Check values in PKB
}

