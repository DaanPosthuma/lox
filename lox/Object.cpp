#include "Object.h"
#include <stdexcept>
#include <ranges>

namespace r = std::ranges;
namespace rv = std::ranges::views;

namespace {

    std::string doubleToString(double x) {
        auto s = std::to_string(x);
        s.erase(r::find_if(s | rv::reverse, [](auto ch) {
            return ch != '0';
            }).base(), s.end());
        if (s.back() == '.') s += '0';
        return s;
    }

}

std::string Object::toString() const {
    if (isString()) return '"' + std::get<std::string>(mData) + '"';
    else if (isDouble()) return doubleToString(std::get<double>(mData));
    else if (isBoolean()) return std::get<bool>(mData) ? "true" : "false";
    else if (isNil()) return "Nil";
    else if (isLoxCallable()) {
        auto const name = std::get<LoxCallable>(mData).name();
        if (name.empty()) return "<fn>";
        else return "<fn " + name + ">";
    }
    else if (isLoxClass()) {
        auto const name = std::get<LoxClass>(mData).name();
        if (name.empty()) return "<class>";
        else return "<class " + name + ">";
    }
    else if (isLoxInstance()) {
        auto const className = std::get<LoxInstance>(mData).klass().name();
        return "<" + className + " instance>";
    }
    else return "unknown type";
}

Object::operator std::string() const {
    if (!isString()) throw std::runtime_error("Cannot convert " + typeAsString() + " to String");
    return std::get<std::string>(mData);
}

Object::operator double() const {
    if (!isDouble()) throw std::runtime_error("Cannot convert " + typeAsString() + " to Double");
    return std::get<double>(mData);
}

Object::operator bool() const {
    if (!isBoolean()) throw std::runtime_error("Cannot convert " + typeAsString() + " to Boolean");
    return std::get<bool>(mData);
}

Object::operator LoxCallable() const {
    if (!isLoxCallable()) throw std::runtime_error("Cannot convert " + typeAsString() + " to LoxCallable");
    return std::get<LoxCallable>(mData);
}

Object::operator LoxClass() const {
    if (!isLoxClass()) throw std::runtime_error("Cannot convert " + typeAsString() + " to LoxClass");
    return std::get<LoxClass>(mData);
}

Object::operator LoxInstance() const {
    if (!isLoxInstance()) throw std::runtime_error("Cannot convert " + typeAsString() + " to LoxInstance");
    return std::get<LoxInstance>(mData);
}

bool Object::isString() const {
    return std::holds_alternative<std::string>(mData);
}

bool Object::isDouble() const {
    return std::holds_alternative<double>(mData);
}

bool Object::isBoolean() const {
    return std::holds_alternative<bool>(mData);
}

bool Object::isLoxCallable() const {
    return std::holds_alternative<LoxCallable>(mData);
}

bool Object::isLoxClass() const {
    return std::holds_alternative<LoxClass>(mData);
}

bool Object::isLoxInstance() const {
    return std::holds_alternative<LoxInstance>(mData);
}

bool Object::isNil() const {
    return std::holds_alternative<Nil>(mData);
}

inline std::string Object::typeAsString() const noexcept {
    if (isString()) return "String";
    else if (isDouble()) return "Double";
    else if (isBoolean()) return "Boolean";
    else if (isNil()) return "Nil";
    else if (isLoxCallable()) return "LoxCallable";
    else if (isLoxClass()) return "LoxClass";
    else if (isLoxInstance()) return "LoxInstance";
    else return "Unknown type";
}

bool operator==(Object const& lhs, Object const& rhs) {
    return lhs.mData == rhs.mData;
}

std::ostream& operator<<(std::ostream& os, Object const& object) {
    os << object.toString();
    return os;
}
