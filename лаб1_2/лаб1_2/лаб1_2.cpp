#include <iostream>
#include <set>
#include <string>
#include "Algorithms.h"
#include "Errors.h"
#include "Node.h"
#include "Tests.h"

using Number = double;

std::string join(const std::set<std::string>& names) {
    std::string result;
    for (const auto& name : names) {
        if (!result.empty()) {
            result += ", ";
        }
        result += name;
    }
    return result.empty() ? "(none)" : result;
}

void describe(const std::string& title, const Node<Number>& formula) {
    std::cout << title << "\n"
        << "  text:       " << to_string(formula) << "\n"
        << "  variables:  " << join(variables(formula)) << "\n"
        << "  depth:      " << depth(formula) << "\n"
        << "  nodes:      " << node_count(formula) << "\n"
        << "  simplified: " << to_string(*simplify(formula)) << "\n\n";
}

void print_value(const Node<Number>& formula, const Variables<Number>& values) {
    std::cout << "  " << to_string(formula) << " = ";
    try {
        std::cout << evaluate(formula, values) << "\n";
    }
    catch (const UnknownVariableError& error) {
        std::cout << "error: " << error.what() << "\n";
    }
    catch (const DivisionByZeroError& error) {
        std::cout << "error: " << error.what() << "\n";
    }
}

void run_demo() {
    std::cout << "\n===== Demo =====\n\n";

    const auto c = [](Number value) { return constant(value); };
    const auto v = [](const std::string& name) { return variable<Number>(name); };

    const auto only_constants = mul(add(c(2), c(3)), c(4));
    const auto one_variable = sub(mul(add(c(2), c(3)), v("x")), c(1));
    const auto many_variables = add(mul(v("price"), v("count")), v("delivery"));
    const auto nested = divide(
        mul(sub(v("price"), divide(c(10), c(4))), add(v("count"), c(1))),
        sub(v("delivery"), sub(c(2), c(1))));

    describe("1) Constants only", *only_constants);
    describe("2) One variable", *one_variable);
    describe("3) Several variables", *many_variables);
    describe("4) Several levels of nesting", *nested);

    std::cout << "Evaluation of the same formula with different values:\n";
    print_value(*many_variables, { { "price", 10 }, { "count", 3 }, { "delivery", 5 } });
    print_value(*many_variables, { { "price", 99.5 }, { "count", 2 }, { "delivery", 0 } });

    std::cout << "\nUnknown variable (delivery is not set):\n";
    print_value(*many_variables, { { "price", 10 }, { "count", 3 } });

    std::cout << "\nDivision by zero:\n";
    const auto risky = divide(v("x"), sub(v("count"), c(2)));
    print_value(*risky, { { "x", 6 }, { "count", 2 } });

    const auto constant_zero = add(divide(c(1), c(0)), v("x"));
    std::cout << "  simplify " << to_string(*constant_zero) << " -> "
        << to_string(*simplify(*constant_zero)) << "\n";
    print_value(*constant_zero, { { "x", 6 } });
}

int main() {
    const int failed = run_all_tests();
    run_demo();
    return failed == 0 ? 0 : 1;
}
