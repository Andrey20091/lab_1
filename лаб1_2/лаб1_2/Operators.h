#pragma once
#include <functional>
#include <stdexcept>
#include <string>
#include <utility>
#include "Errors.h"

template <class T>
class BinaryOperator {
public:
    using Function = std::function<T(const T&, const T&)>;

    BinaryOperator(std::string symbol, Function function)
        : symbol_(std::move(symbol)), function_(std::move(function))
    {
        if (symbol_.empty()) {
            throw std::invalid_argument("BinaryOperator: empty symbol");
        }
        if (!function_) {
            throw std::invalid_argument("BinaryOperator: empty function");
        }
    }

    const std::string& symbol() const { return symbol_; }

    T apply(const T& left, const T& right) const { return function_(left, right); }

private:
    std::string symbol_;
    Function function_;
};

template <class T>
BinaryOperator<T> addition() {
    return BinaryOperator<T>("+", [](const T& a, const T& b) { return a + b; });
}

template <class T>
BinaryOperator<T> subtraction() {
    return BinaryOperator<T>("-", [](const T& a, const T& b) { return a - b; });
}

template <class T>
BinaryOperator<T> multiplication() {
    return BinaryOperator<T>("*", [](const T& a, const T& b) { return a * b; });
}

template <class T>
BinaryOperator<T> division() {
    return BinaryOperator<T>("/", [](const T& a, const T& b) {
        if (b == T{}) {
            throw DivisionByZeroError();
        }
        return a / b;
    });
}
