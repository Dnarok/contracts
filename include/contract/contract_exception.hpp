#pragma once

#include <stdexcept>

struct contract_exception : std::logic_error
{
    contract_exception(const std::string& message) :
        std::logic_error{ message }
    {};
};