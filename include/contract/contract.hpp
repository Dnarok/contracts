#pragma once

#include <string>
#include <vector>
#include <functional>
#include <iostream>

#include "contract/contract_exception.hpp"

struct contract
{
    using functor_t = std::function<bool()>;

    /// @brief Determines what the contract does on condition failure.
    enum class behavior
    {
        /// @brief The default. The result of the condition failure is ignored.
        ignore,
        /// @brief Logs on condition failure, including the optional descriptive message.
        log,
        /// @brief Throws a contract_exception on condition failure.
        exception,
        /// @brief Calls std::terminate() on condition failure.
        terminate,
        /// @brief Combines log and exception.
        log_and_exception,
        /// @brief Combines log and terminate.
        log_and_terminate,
    };

    /// @brief Determines when the provided functors are called.
    enum class evaluation
    {
        /// @brief The default. The provided functors are never called.
        unevaluated,
        /// @brief The provided functors will be called.
        always,
        /// @brief The provided functors will only be called `#ifndef NDEBUG`.
        debug
    };

    /// @brief 
    /// @param function_name Can be any string, but a good cross-platform choice is `__func__`. 
    /// @param b The contract condition failure behavior. By default, does nothing.
    /// @param e The contract evaluation behavior. By default, the provided functors are not called.
    contract(const std::string& function_name, const behavior b = behavior::ignore, const evaluation e = evaluation::unevaluated) :
        function_name{ function_name },
        b{ b },
        e{ e }
    {};

    contract() = default;
    contract(const contract& other) = default;

    /// @brief Defines a precondition for the contract, and accepts a descriptive message to output on failure.
    /// @param functor A functor that will be evaluated immediately, and determines whether the precondition has been met.
    /// @param message A descriptive message - e.g. if i must be less than size, then "i must be less than size."
    /// @return A reference to this contract to allow for chaining of conditions.
    auto precondition(const functor_t& functor, const std::string& message = "") -> contract&
    {
        evaluate(functor, message, type::precondition);

        return *this;
    };

    /// @brief Defines a postcondition for the contract, and accepts a descriptive message to output on failure.
    /// @param functor A functor that will be evaluated when this contract is destroyed, and determines whether the postcondition has been met.
    /// @param message A descriptive message - e.g. if vec is now empty, then "vec must be empty."
    /// @return A reference to this contract to allow for chaining of conditions.
    auto postcondition(const functor_t& functor, std::string message = "") -> contract&
    {
        postconditions.push_back(post{ functor, message });

        return *this;
    };

    /// @brief Defines a condition (i.e. an assert), and accepts a descriptive message to output on failure.
    /// @param functor A functor that will be evaluated immediately, and determines whether the condition has been met.
    /// @param message A descriptive message - e.g. if ptr is not nullptr, then "ptr is not nullptr."
    /// @return A reference to this contract to allow for chaining of conditions.
    auto condition(const functor_t& functor, const std::string& message = "") -> contract&
    {
        evaluate(functor, message, type::condition);

        return *this;
    };

    /// @brief Evaluates any postconditions previously defined.
    ~contract()
    {
        for (const auto& cond : postconditions)
        {
            evaluate(cond.functor, cond.message, type::postcondition);
        }
    };

private:
    enum class type
    {
        precondition,
        condition,
        postcondition
    };
    struct post
    {
        functor_t functor = {};
        std::string message = {};
    };

    std::vector<post> postconditions = {};
    std::string function_name = "";
    behavior b = behavior::ignore;
    evaluation e = evaluation::unevaluated;

    auto log(const std::string& message, const type t) -> void
    {
        switch (t)
        {
        case type::precondition:
            std::cerr << "precondition failed";
            break;
        default:
            [[fallthrough]];
        case type::condition:
            std::cerr << "condition failed";
            break;
        case type::postcondition:
            std::cerr << "postcondition failed";
            break;
        }

        if (!function_name.empty())
        {
            std::cerr << " in " << function_name;
        }
        if (!message.empty())
        {
            std::cerr << ": " << message;
        }
        
        std::cerr << '\n';
    };

    auto evaluate(const functor_t& functor, const std::string& message, const type t) -> void
    {
        bool result;
        switch (e)
        {
        default:
            [[fallthrough]];
        case evaluation::unevaluated:
            break;
        case evaluation::debug:
            #ifdef NDEBUG
            break;
            #else
            [[fallthrough]];
            #endif
        case evaluation::always:
            result = functor();
            if (!result)
            {
                switch (b)
                {
                default:
                    [[fallthrough]];
                case behavior::ignore:
                    break;
                case behavior::log:
                    log(message, t);
                    break;
                case behavior::exception:
                    throw contract_exception{ message };
                    break;
                case behavior::terminate:
                    std::terminate();
                    break;
                case behavior::log_and_exception:
                    log(message, t);
                    throw contract_exception{ message };
                    break;
                case behavior::log_and_terminate:
                    log(message, t);
                    std::terminate();
                    break;
                }
            }
        }
    };
};