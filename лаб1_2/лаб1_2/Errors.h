#pragma once
#include <stdexcept>
#include <string>

class UnknownVariableError : public std::runtime_error {
public:
    explicit UnknownVariableError(const std::string& name)
        : std::runtime_error("unknown variable: " + name), name_(name) {}

    const std::string& name() const { return name_; }

private:
    std::string name_;
};

class DivisionByZeroError : public std::runtime_error {
public:
    DivisionByZeroError() : std::runtime_error("division by zero") {}
};
