//
// Created by zsoh9 on 8/9/2020.
//
#include <Validator.h>
#include "catch.hpp"

TEST_CASE(
       "NAMES must be alphanumeric and start with a letter") { // optional second param Is a tag to assign to the test
   REQUIRE(Validator::IsNAME("A") == true);
   REQUIRE(Validator::IsNAME("a") == true);
   REQUIRE(Validator::IsNAME("AB") == true);
   REQUIRE(Validator::IsNAME("Ab") == true);
   REQUIRE(Validator::IsNAME("A2") == true);
   REQUIRE(Validator::IsNAME("2B") == false);
   REQUIRE(Validator::IsNAME("#B") == false);
   REQUIRE(Validator::IsNAME("A#") == false);
}

TEST_CASE("Assignment Statements") {
   std::string toTest = "x=1";
   REQUIRE(Validator::IsValidAssignmentStmt(toTest));
   SECTION("adding a '+' operator to the expression") {
       SECTION("Not providing a term to add") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "+") == false);
       }
       SECTION("Adding another constant") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "+2") == true);
       }
       SECTION("Adding a variable") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "+y") == true);
       }
       SECTION("Attempting to increment a var") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "y++") == false);
       }
   }

   SECTION("adding a '-' operator to the expression") {
       SECTION("Not providing a term to subtract") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "-") == false);
       }
       SECTION("Adding another constant") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "-2") == true);
       }
       SECTION("Adding a variable") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "-y") == true);
       }
       SECTION("Attempting to increment a var") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "y--") == false);
       }
   }

   SECTION("adding a '*' operator to the expression") {
       SECTION("Not providing a term to multiply") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "*") == false);
       }
       SECTION("Adding another constant") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "*2") == true);
       }
       SECTION("Adding a variable") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "*y") == true);
       }
   }

   SECTION("adding a '/' operator to the expression") {
       SECTION("Not providing a term to add") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "/") == false);
       }
       SECTION("Adding another constant") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "/2") == true);
       }
       SECTION("Adding a variable") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "/y") == true);
       }
   }

   SECTION("adding a '%' operator to the expression") {
       SECTION("Not providing a term to add") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "%") == false);
       }
       SECTION("Adding another constant") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "%2") == true);
       }
       SECTION("Adding a variable") {
           REQUIRE(Validator::IsValidAssignmentStmt(toTest + "%y") == true);
       }
   }
}

TEST_CASE("Conditional Statements Test", "[Statement type validation]") {

   std::string toTest = "()";
   SECTION("empty conditional expression") {
       REQUIRE(Validator::IsValidConditionalStmt(toTest) == false);
   }
   toTest = "(x==1)";
   REQUIRE(Validator::IsValidConditionalStmt(toTest) == true);
   SECTION(" adding !") {
       toTest = "(!" + toTest + ")";
       REQUIRE(Validator::IsValidConditionalStmt(toTest) == true);
   }
   SECTION("adding && and another conditional") {
       std::string otherCond = "(y>=2)";
       REQUIRE(Validator::IsValidConditionalStmt(
               "(" + toTest + "&&" + otherCond + ")") == true);
       REQUIRE(Validator::IsValidConditionalStmt(
               "(" + toTest + "&" + otherCond + ")") == false);
   }
   SECTION("adding || and another conditional") {
       std::string otherCond = "(y<=2)";
       REQUIRE(Validator::IsValidConditionalStmt(
               "(" + toTest + "||" + otherCond + ")") == true);
       REQUIRE(Validator::IsValidConditionalStmt(
               "(" + toTest + "|" + otherCond + ")") == false);
   }

}

TEST_CASE("Relation expressions test") {
   SECTION("Test on operators") {
       REQUIRE(Validator::IsValidRelationExpression("x>=2") == true);
       REQUIRE(Validator::IsValidRelationExpression("x>2") == true);
       REQUIRE(Validator::IsValidRelationExpression("x<=2") == true);
       REQUIRE(Validator::IsValidRelationExpression("x<2") == true);
       REQUIRE(Validator::IsValidRelationExpression("x!=2") == true);
       REQUIRE(Validator::IsValidRelationExpression("x!2") == false);
       REQUIRE(Validator::IsValidRelationExpression("x==2") == true);
       REQUIRE(Validator::IsValidRelationExpression("x=2") == false);
   }
   SECTION("Test on rel_factor") {
       SECTION("Both sides are var_names") {
           std::string LHS = "x";
           std::string RHS = "y";
           REQUIRE(Validator::IsNAME(LHS));
           REQUIRE(Validator::IsNAME(RHS));
           REQUIRE(Validator::IsValidRelationExpression(LHS + ">=" + RHS) ==
                   true);
       }
       SECTION("Both sides are consts") {
           std::string LHS = "2";
           std::string RHS = "1";
           REQUIRE(Validator::IsConst(LHS));
           REQUIRE(Validator::IsConst(RHS));
           REQUIRE(Validator::IsValidRelationExpression(LHS + ">=" + RHS) ==
                   true);
       }
       SECTION("Both sides are expressions") {
           std::string LHS = "x+1*y";
           std::string RHS = "((p+a)+3)*a+p*(ifs)+(p*w)";
           REQUIRE(Validator::IsExpr(LHS));
           REQUIRE(Validator::IsExpr(RHS));
           REQUIRE(Validator::IsValidRelationExpression(LHS + ">=" + RHS) ==
                   true);
       }
       SECTION("Combinations of NAMES/CONSTS/EXPR") {
           std::string name = "x";
           std::string c = "1";
           std::string expr = "p*a*a+p*(ifs)+(p*w)";
           REQUIRE(Validator::IsNAME(name));
           REQUIRE(Validator::IsConst(c));
           REQUIRE(Validator::IsValidRelationExpression(name + ">=" + c) ==
                   true);
           REQUIRE(Validator::IsValidRelationExpression(name + ">=" + expr) ==
                   true);
           REQUIRE(Validator::IsValidRelationExpression(c + ">=" + name) ==
                   true);
           REQUIRE(Validator::IsValidRelationExpression(c + ">=" + expr) ==
                   true);
           REQUIRE(Validator::IsValidRelationExpression(expr + ">=" + name) ==
                   true);
           REQUIRE(Validator::IsValidRelationExpression(expr + ">=" + c) ==
                   true);
       }
   }
   SECTION("multiple nested conditions") {
       std::string conditional1 = "(x>=1)";
       REQUIRE(Validator::IsValidConditionalStmt(conditional1));
       SECTION("multiple brackets nesting") {
           REQUIRE(Validator::IsValidConditionalStmt(
                   "(((((" + conditional1 + ")))))"));
       }
       std::string conditional2 = "(y!=2)";
       std::string conditional3 = "(z<=3)";
       REQUIRE(Validator::IsValidConditionalStmt(conditional2));
       REQUIRE(Validator::IsValidConditionalStmt(conditional3));
       SECTION("&& nested in ||") {
           REQUIRE(Validator::IsValidConditionalStmt(
                   "((" + conditional1 + "&&" + conditional2 + ")" + "||"
                   + conditional3 + ")"));
       }

   }
}
