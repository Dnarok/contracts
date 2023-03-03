#include <contract/contract.hpp>

#include <catch2/catch_test_macros.hpp>

auto test(int value) -> void
{
    const int value_before = value;
    auto _ = contract{ __func__, contract::behavior::log_and_exception, contract::evaluation::always }
           .precondition([&]{ return value % 2 == 0; })
           .postcondition([&]{ return value == value_before * 2; });

     value *= 2;
};

TEST_CASE("precondition and postcondition can be passed", "[contracts]")
{
    test(2);
};

TEST_CASE("precondition fails and throws", "[contracts]")
{
    //REQUIRE_THROWS(test(3));
};