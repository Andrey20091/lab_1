#pragma once
#include <cctype>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>
#include <utility>
#include "Operators.h"

enum class NodeKind { Constant, Variable, Binary };

inline bool is_identifier(const std::string& text) {
    if (text.empty() || std::isdigit(static_cast<unsigned char>(text[0]))) {
        return false;
    }
    for (char ch : text) {
        if (!std::isalnum(static_cast<unsigned char>(ch)) && ch != '_') {
            return false;
        }
    }
    return true;
}

template <class T>
class Node {
public:
    explicit Node(T value)
        : kind_(NodeKind::Constant), value_(std::move(value)) {}

    explicit Node(std::string name)
        : kind_(NodeKind::Variable), name_(std::move(name))
    {
        if (!is_identifier(name_)) {
            throw std::invalid_argument("invalid variable name: '" + name_ + "'");
        }
    }

    Node(BinaryOperator<T> op, std::unique_ptr<const Node> left, std::unique_ptr<const Node> right)
        : kind_(NodeKind::Binary), op_(std::move(op)), left_(std::move(left)), right_(std::move(right))
    {
        if (!left_ || !right_) {
            throw std::invalid_argument("binary operation needs two operands");
        }
    }

    NodeKind kind() const { return kind_; }

    const T& value() const {
        require(NodeKind::Constant);
        return value_;
    }

    const std::string& name() const {
        require(NodeKind::Variable);
        return name_;
    }

    const BinaryOperator<T>& op() const {
        require(NodeKind::Binary);
        return *op_;
    }

    const Node& left() const {
        require(NodeKind::Binary);
        return *left_;
    }

    const Node& right() const {
        require(NodeKind::Binary);
        return *right_;
    }

private:
    NodeKind kind_;
    T value_{};
    std::string name_;
    std::optional<BinaryOperator<T>> op_;
    std::unique_ptr<const Node> left_;
    std::unique_ptr<const Node> right_;

    void require(NodeKind expected) const {
        if (kind_ != expected) {
            throw std::logic_error("wrong node kind");
        }
    }
};

template <class T>
using Expression = std::unique_ptr<const Node<T>>;

template <class T>
Expression<T> constant(T value) {
    return std::make_unique<const Node<T>>(std::move(value));
}

template <class T>
Expression<T> variable(std::string name) {
    return std::make_unique<const Node<T>>(std::move(name));
}

template <class T>
Expression<T> binary(BinaryOperator<T> op, Expression<T> left, Expression<T> right) {
    return std::make_unique<const Node<T>>(std::move(op), std::move(left), std::move(right));
}

template <class T>
Expression<T> add(Expression<T> left, Expression<T> right) {
    return binary(addition<T>(), std::move(left), std::move(right));
}

template <class T>
Expression<T> sub(Expression<T> left, Expression<T> right) {
    return binary(subtraction<T>(), std::move(left), std::move(right));
}

template <class T>
Expression<T> mul(Expression<T> left, Expression<T> right) {
    return binary(multiplication<T>(), std::move(left), std::move(right));
}

template <class T>
Expression<T> divide(Expression<T> left, Expression<T> right) {
    return binary(division<T>(), std::move(left), std::move(right));
}
