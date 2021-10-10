#include "catch.hpp"
#include "QueryEvaluator.h"

TEST_CASE("Remove Duplicates Test") {
    list<string> stringList;
    stringList.emplace_back("1");
    stringList.emplace_back("1");
    stringList.emplace_back("2");
    stringList.emplace_back("2");
    stringList.emplace_back("3");
    stringList.emplace_back("3");
    stringList.emplace_back("4");
    stringList.emplace_back("5");
    stringList.emplace_back("5");
    stringList.emplace_back("5");
    QueryEvaluator::RemoveDuplicates(stringList);
    REQUIRE(stringList.size() == 5);
}

TEST_CASE("Merge Combinations Test") {
    map<string, string> base, empty, populatedWithCommonValues, populatedWithoutCommonValues, result, expected;
    base = {{"a", "1"}, {"a", "2"}, {"b", "3"},
            {"b", "4"}, {"c", "5"}, {"c", "6"}};
    empty = {};
    populatedWithCommonValues = {{"a", "1"}, {"a", "2"}, {"b", "3"},
                                 {"b", "4"}, {"d", "7"}, {"d", "8"}};
    populatedWithoutCommonValues = {{"a", "99"}, {"a", "98"}, {"x", "97"},
                                    {"x", "96"}, {"z", "95"}, {"z", "94"}};

    SECTION("Populated map with empty map") {
        result = QueryEvaluator::MergeCombinations(base, empty);
        expected = base;
        REQUIRE(equal(result.begin(), result.end(), expected.begin(), expected.end()));
    }

    SECTION("Empty map with populated map") {
        result = QueryEvaluator::MergeCombinations(empty, base);
        expected = base;
        REQUIRE(equal(result.begin(), result.end(), expected.begin(), expected.end()));
    }

    SECTION("Two populated maps with common values") {
        result = QueryEvaluator::MergeCombinations(base, populatedWithCommonValues);
        expected = {{"a", "1"}, {"a", "2"}, {"b", "3"},
                    {"b", "4"}, {"c", "5"}, {"c", "6"},
                    {"d", "7"}, {"d", "8"}};
        REQUIRE(equal(result.begin(), result.end(), expected.begin(), expected.end()));
    }

    SECTION("Two populated maps with no common values") {
        result = QueryEvaluator::MergeCombinations(base, populatedWithoutCommonValues);
        expected = empty;
        REQUIRE(equal(result.begin(), result.end(), expected.begin(), expected.end()));
    }
}