#pragma once
#include <algorithm>
#include <cstddef>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include "Errors.h"
#include "Node.h"

template <class T>
using Variables = std::map<std::string, T>;

template <class T>
std::size_t node_count(const Node<T>& node) {
    switch (node.kind()) {
    case NodeKind::Constant:
    case NodeKind::Variable:
        return 1;
    case NodeKind::Binary:
        return node_count(node.left()) + node_count(node.right()) + 1;
    }
    throw std::logic_error("unknown node kind");
}

template <class T>
std::size_t depth(const Node<T>& node) {
    switch (node.kind()) {
    case NodeKind::Constant:
    case NodeKind::Variable:
        return 1;
    case NodeKind::Binary:
        return std::max(depth(node.left()), depth(node.right())) + 1;
    }
    throw std::logic_error("unknown node kind");
}

template <class T>
void collect_variables(const Node<T>& node, std::set<std::string>& names) {
    switch (node.kind()) {
    case NodeKind::Constant:
        return;
    case NodeKind::Variable:
        names.insert(node.name());
        return;
    case NodeKind::Binary:
        collect_variables(node.left(), names);
        collect_variables(node.right(), names);
        return;
    }
    throw std::logic_error("unknown node kind");
}

template <class T>
std::set<std::string> variables(const Node<T>& node) {
    std::set<std::string> names;
    collect_variables(node, names);
    return names;
}

template <class T>
T evaluate(const Node<T>& node, const Variables<T>& values) {
    switch (node.kind()) {
    case NodeKind::Constant:
        return node.value();
    case NodeKind::Variable: {
        const auto it = values.find(node.name());
        if (it == values.end()) {
            throw UnknownVariableError(node.name());
        }
        return it->second;
    }
    case NodeKind::Binary: {
        const T left = evaluate(node.left(), values);
        const T right = evaluate(node.right(), values);
        return node.op().apply(left, right);
    }
    }
    throw std::logic_error("unknown node kind");
}

template <class T>
std::string to_string(const Node<T>& node) {
    switch (node.kind()) {
    case NodeKind::Constant: {
        std::ostringstream out;
        out << node.value();
        return out.str();
    }
    case NodeKind::Variable:
        return node.name();
    case NodeKind::Binary:
        return "(" + to_string(node.left()) + " " + node.op().symbol() + " " + to_string(node.right()) + ")";
    }
    throw std::logic_error("unknown node kind");
}

template <class T>
Expression<T> simplify(const Node<T>& node) {
    switch (node.kind()) {
    case NodeKind::Constant:
        return constant(node.value());
    case NodeKind::Variable:
        return variable<T>(node.name());
    case NodeKind::Binary: {
        Expression<T> left = simplify(node.left());
        Expression<T> right = simplify(node.right());
        if (left->kind() == NodeKind::Constant && right->kind() == NodeKind::Constant) {
            try {
                return constant(node.op().apply(left->value(), right->value()));
            }
            catch (const DivisionByZeroError&) {
            }
        }
        return binary(node.op(), std::move(left), std::move(right));
    }
    }
    throw std::logic_error("unknown node kind");
}
