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
    
    // trim from end (in place)
    static inline void rtrim(std::string& s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
            return !std::isspace(ch);
            }).base(), s.end());
    }

    // trim from end (copying)
    static inline std::string rtrim_copy(std::string s) {
        rtrim(s);
        return s;
    }

}

std::string Object::toString() const {
    if (std::holds_alternative<std::string>(mData)) return std::get<std::string>(mData);
    else if (std::holds_alternative<double>(mData)) return doubleToString(std::get<double>(mData));
    else if (std::holds_alternative<bool>(mData)) return std::get<bool>(mData) ? "true" : "false";
    else if (std::holds_alternative<Nil>(mData)) return "Nil";
    else if (std::holds_alternative<LoxCallable>(mData)) {
        auto const name = std::get<LoxCallable>(mData).name();
        if (name.empty()) return "<fn>";
        else return "<fn " + name + ">";
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

bool Object::isNil() const {
    return std::holds_alternative<Nil>(mData);
}

inline std::string Object::typeAsString() const noexcept {
    if (isString()) return "String";
    else if (isDouble()) return "Double";
    else if (isBoolean()) return "Boolean";
    else if (isNil()) return "Nil";
    else if (isLoxCallable()) return "LoxCallable";
    else return "Unknown type";
}

bool operator==(Object const& lhs, Object const& rhs) {
    return lhs.mData == rhs.mData;
}
