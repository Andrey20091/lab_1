#pragma once
#include <iostream>
#include <memory>
#include <set>
#include <stdexcept>
#include <string>
#include "Algorithms.h"
#include "Errors.h"
#include "Node.h"
#include "Operators.h"

// Журнал тестов: печатает [OK] или [FAIL] и считает результаты
class TestReport {
public:
    void check(bool condition, const std::string& name) {
        std::cout << (condition ? "[OK]   " : "[FAIL] ") << name << "\n";
        if (condition) {
            ++passed_;
        }
        else {
            ++failed_;
        }
    }

    int passed() const { return passed_; }
    int failed() const { return failed_; }

private:
    int passed_ = 0;
    int failed_ = 0;
};

// Проверка, что код бросает именно нужное исключение
template <class Error, class Action>
bool throws(Action action) {
    try {
        action();
    }
    catch (const Error&) {
        return true;
    }
    catch (...) {
        return false;
    }
    return false;
}

// ===== Тесты операций (Operators.h) =====
inline void test_operators(TestReport& report) {
    // У каждой операции правильный символ
    report.check(addition<int>().symbol() == "+", "operator: + symbol");
    report.check(subtraction<int>().symbol() == "-", "operator: - symbol");
    report.check(multiplication<int>().symbol() == "*", "operator: * symbol");
    report.check(division<int>().symbol() == "/", "operator: / symbol");

    // Каждая операция правильно считает, для int и для double
    report.check(addition<int>().apply(7, 2) == 9, "operator: 7 + 2");
    report.check(subtraction<int>().apply(7, 2) == 5, "operator: 7 - 2");
    report.check(multiplication<int>().apply(7, 2) == 14, "operator: 7 * 2");
    report.check(division<int>().apply(7, 2) == 3, "operator: int 7 / 2 truncates");
    report.check(division<double>().apply(7.0, 2.0) == 3.5, "operator: double 7 / 2");

    // Деление на ноль бросает ошибку
    report.check(throws<DivisionByZeroError>([] { division<int>().apply(1, 0); }),
        "operator: int division by zero throws");
    report.check(throws<DivisionByZeroError>([] { division<double>().apply(1.0, 0.0); }),
        "operator: double division by zero throws");

    // Нельзя создать операцию без символа или без правила
    report.check(throws<std::invalid_argument>([] {
        BinaryOperator<int>("", [](const int& a, const int&) { return a; });
    }), "operator: empty symbol throws");
    report.check(throws<std::invalid_argument>([] {
        BinaryOperator<int>("?", nullptr);
    }), "operator: empty function throws");
}

// ===== Тесты ошибок (Errors.h) =====
inline void test_errors(TestReport& report) {
    // Ошибка помнит имя переменной и даёт правильный текст
    const UnknownVariableError unknown("price");
    report.check(unknown.name() == "price", "error: unknown variable keeps the name");
    report.check(std::string(unknown.what()) == "unknown variable: price", "error: unknown variable message");

    // Наши ошибки ловятся как общая std::runtime_error
    report.check(throws<std::runtime_error>([] { throw DivisionByZeroError(); }),
        "error: division by zero is a runtime_error");
    report.check(throws<std::runtime_error>([] { throw UnknownVariableError("x"); }),
        "error: unknown variable is a runtime_error");
}

// ===== Тесты узлов (Node.h) =====
inline void test_nodes(TestReport& report) {
    // Узел-число хранит значение
    const auto number = constant(5);
    report.check(number->kind() == NodeKind::Constant && number->value() == 5, "node: constant keeps value");

    // Узел-переменная хранит имя
    const auto price = variable<int>("price");
    report.check(price->kind() == NodeKind::Variable && price->name() == "price", "node: variable keeps name");

    // Узел-операция хранит операцию и обоих детей
    const auto sum = add(constant(1), variable<int>("x"));
    report.check(sum->kind() == NodeKind::Binary && sum->op().symbol() == "+", "node: binary keeps operation");
    report.check(sum->left().value() == 1 && sum->right().name() == "x", "node: binary keeps both operands");

    // Правило имён переменных: хорошие проходят, плохие нет
    report.check(is_identifier("x") && is_identifier("_tmp") && is_identifier("price2"), "node: valid identifiers");
    report.check(!is_identifier("") && !is_identifier("2x") && !is_identifier("a b") && !is_identifier("a-b"),
        "node: invalid identifiers");

    // Нельзя создать переменную с плохим именем
    report.check(throws<std::invalid_argument>([] { variable<int>(""); }), "node: empty variable name throws");
    report.check(throws<std::invalid_argument>([] { variable<int>("2x"); }), "node: bad variable name throws");

    // Нельзя создать операцию без операнда
    report.check(throws<std::invalid_argument>([] { add<int>(nullptr, constant(1)); }),
        "node: binary without left operand throws");
    report.check(throws<std::invalid_argument>([] { add<int>(constant(1), nullptr); }),
        "node: binary without right operand throws");

    // Нельзя спросить у узла чужие данные
    report.check(throws<std::logic_error>([&] { number->name(); }), "node: constant has no name");
    report.check(throws<std::logic_error>([&] { price->value(); }), "node: variable has no value");
    report.check(throws<std::logic_error>([&] { number->left(); }), "node: constant has no operands");
    report.check(throws<std::logic_error>([&] { price->op(); }), "node: variable has no operation");
}

// ===== Тесты количества узлов, глубины и переменных (для int и double) =====
template <class T>
void test_structure(TestReport& report, const std::string& type) {
    // Помощники: c(5) создаёт число, v("a") создаёт переменную
    const auto c = [](int value) { return constant<T>(T(value)); };
    const auto v = [](const std::string& name) { return variable<T>(name); };

    // Дерево из одного листа
    const auto leaf = c(5);
    report.check(node_count(*leaf) == 1, type + ": leaf has 1 node");
    report.check(depth(*leaf) == 1, type + ": leaf has depth 1");

    // Простое дерево из трёх узлов
    const auto sum = add(v("a"), v("b"));
    report.check(node_count(*sum) == 3, type + ": a + b has 3 nodes");
    report.check(depth(*sum) == 2, type + ": a + b has depth 2");

    // Несбалансированное дерево: глубина по самой длинной ветке
    const auto nested = sub(mul(add(v("a"), v("b")), v("c")), v("d"));
    report.check(node_count(*nested) == 7, type + ": ((a + b) * c) - d has 7 nodes");
    report.check(depth(*nested) == 4, type + ": depth is the longest branch");

    // Переменные: нет ни одной, повтор, порядок по алфавиту
    report.check(variables(*add(c(1), c(2))).empty(), type + ": constants only -> no variables");
    report.check(variables(*add(mul(v("a"), v("a")), v("a"))) == std::set<std::string>{ "a" },
        type + ": repeated variable listed once");
    report.check(variables(*sub(add(v("d"), mul(v("b"), v("a"))), v("c"))) == std::set<std::string>{ "a", "b", "c", "d" },
        type + ": variables are unique and sorted");
}

// ===== Тесты текстового представления (для int и double) =====
template <class T>
void test_to_string(TestReport& report, const std::string& type) {
    const auto c = [](int value) { return constant<T>(T(value)); };
    const auto v = [](const std::string& name) { return variable<T>(name); };

    // Листья печатаются без скобок
    report.check(to_string(*c(2)) == "2", type + ": text of constant");
    report.check(to_string(*v("a")) == "a", type + ": text of variable");

    // Скобки показывают порядок действий
    report.check(to_string(*add(mul(v("price"), v("count")), v("delivery"))) == "((price * count) + delivery)",
        type + ": text of nested formula");
    report.check(to_string(*mul(add(c(2), c(3)), v("a"))) == "((2 + 3) * a)", type + ": brackets keep the order");
    report.check(to_string(*sub(v("a"), sub(v("b"), v("c")))) == "(a - (b - c))", type + ": right operand in brackets");
}

// ===== Тесты вычисления (для int и double) =====
template <class T>
void test_evaluate(TestReport& report, const std::string& type) {
    const auto c = [](int value) { return constant<T>(T(value)); };
    const auto v = [](const std::string& name) { return variable<T>(name); };

    // Одна формула при разных значениях переменных
    const auto formula = add(mul(v("price"), v("count")), v("delivery"));
    Variables<T> values{ { "price", T(10) }, { "count", T(3) }, { "delivery", T(5) } };
    report.check(evaluate(*formula, values) == T(35), type + ": price * count + delivery = 35");

    values["count"] = T(4);
    report.check(evaluate(*formula, values) == T(45), type + ": same formula, other values = 45");

    // Формулы из одних чисел и порядок действий
    const Variables<T> empty;
    report.check(evaluate(*mul(c(2), c(3)), empty) == T(6), type + ": constants need no variables");
    report.check(evaluate(*sub(sub(c(10), c(4)), c(3)), empty) == T(3), type + ": (10 - 4) - 3 = 3");

    // Неизвестная переменная: ошибка с её именем
    const Variables<T> partial{ { "price", T(1) }, { "count", T(1) } };
    bool right_name = false;
    try {
        evaluate(*formula, partial);
    }
    catch (const UnknownVariableError& error) {
        right_name = (error.name() == "delivery");
    }
    report.check(right_name, type + ": unknown variable is reported by name");

    // Деление на ноль числом
    report.check(throws<DivisionByZeroError>([&] { evaluate(*divide(c(1), c(0)), empty); }),
        type + ": division by constant zero throws");

    // Деление на ноль, которое получается из значений переменных
    const auto risky = divide(v("x"), sub(v("y"), c(2)));
    const Variables<T> zero{ { "x", T(6) }, { "y", T(2) } };
    report.check(throws<DivisionByZeroError>([&] { evaluate(*risky, zero); }),
        type + ": division by zero from variables throws");

    // Вычисление не меняет формулу
    const std::string before = to_string(*formula);
    evaluate(*formula, values);
    report.check(to_string(*formula) == before, type + ": evaluate does not change the tree");
}

// ===== Тесты упрощения (для int и double) =====
template <class T>
void test_simplify(TestReport& report, const std::string& type) {
    const auto c = [](int value) { return constant<T>(T(value)); };
    const auto v = [](const std::string& name) { return variable<T>(name); };

    // Пример из задания: (2 + 3) * price -> 5 * price
    const auto example = mul(add(c(2), c(3)), v("price"));
    const auto simplified = simplify(*example);
    report.check(to_string(*simplified) == "(5 * price)", type + ": (2 + 3) * price -> 5 * price");
    report.check(node_count(*simplified) == 3, type + ": simplified tree is smaller");

    // Исходное дерево не изменилось
    report.check(to_string(*example) == "((2 + 3) * price)", type + ": original text unchanged");
    report.check(node_count(*example) == 5, type + ": original size unchanged");

    // Формула из одних чисел сворачивается в одно число
    const auto only_constants = simplify(*mul(add(c(1), c(2)), add(c(3), c(4))));
    report.check(to_string(*only_constants) == "21" && node_count(*only_constants) == 1,
        type + ": constants only -> one constant");

    // Разные случаи: часть сворачивается, нечего сворачивать, без перестановок
    report.check(to_string(*simplify(*add(v("x"), mul(c(2), c(3))))) == "(x + 6)", type + ": constant part folded");
    report.check(to_string(*simplify(*mul(v("x"), v("price")))) == "(x * price)", type + ": nothing to fold");
    report.check(to_string(*simplify(*v("x"))) == "x", type + ": variable stays");
    report.check(to_string(*simplify(*add(add(v("x"), c(2)), c(3)))) == "((x + 2) + 3)", type + ": no reordering");

    // Деление на ноль не сворачивается, но ошибка остаётся при вычислении
    const auto kept = simplify(*mul(divide(c(1), c(0)), v("x")));
    report.check(to_string(*kept) == "((1 / 0) * x)", type + ": division by zero is not folded");
    const Variables<T> one{ { "x", T(1) } };
    report.check(throws<DivisionByZeroError>([&] { evaluate(*kept, one); }),
        type + ": kept division by zero still throws on evaluate");

    // Упрощённая формула даёт тот же ответ, что и исходная
    const auto formula = sub(mul(add(c(2), c(3)), v("x")), divide(c(8), c(2)));
    const Variables<T> seven{ { "x", T(7) } };
    report.check(evaluate(*simplify(*formula), seven) == evaluate(*formula, seven), type + ": simplify keeps the value");
}

// ===== Тест расширяемости: новая операция % без правок в остальном коде =====
inline BinaryOperator<int> remainder_operator() {
    return BinaryOperator<int>("%", [](const int& a, const int& b) {
        if (b == 0) {
            throw DivisionByZeroError();
        }
        return a % b;
    });
}

inline void test_extension(TestReport& report) {
    // Новая операция печатается, вычисляется, считается и упрощается
    const auto rest = binary(remainder_operator(), variable<int>("x"), constant(3));
    const Variables<int> values{ { "x", 10 } };
    report.check(to_string(*rest) == "(x % 3)", "extension: new operation is printed");
    report.check(evaluate(*rest, values) == 1, "extension: new operation is evaluated");
    report.check(node_count(*rest) == 3 && depth(*rest) == 2, "extension: size and depth work");
    report.check(to_string(*simplify(*binary(remainder_operator(), constant(10), constant(4)))) == "2",
        "extension: new operation is simplified");
}

// ===== Запуск всех тестов =====
inline int run_all_tests() {
    TestReport report;

    // Тесты без шаблона: запускаются один раз
    test_operators(report);
    test_errors(report);
    test_nodes(report);

    // Шаблонные тесты: запускаются для int и для double
    test_structure<int>(report, "int");
    test_structure<double>(report, "double");
    test_to_string<int>(report, "int");
    test_to_string<double>(report, "double");
    test_evaluate<int>(report, "int");
    test_evaluate<double>(report, "double");
    test_simplify<int>(report, "int");
    test_simplify<double>(report, "double");

    // Разница типов: для int 7 / 2 = 3, для double 3.5
    report.check(to_string(*simplify(*divide(constant(7), constant(2)))) == "3", "int: 7 / 2 simplifies to 3");
    report.check(to_string(*simplify(*divide(constant(7.0), constant(2.0)))) == "3.5", "double: 7 / 2 simplifies to 3.5");

    // Расширяемость
    test_extension(report);

    // Итог: сколько прошло и сколько упало
    std::cout << "\nPassed: " << report.passed() << ", failed: " << report.failed() << "\n";
    return report.failed();
}
