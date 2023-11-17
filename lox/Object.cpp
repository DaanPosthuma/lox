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
    switch (mType) {
    case Type::String: return mString;
    case Type::Double: return doubleToString(mDouble);
    case Type::Boolean: return mBoolean ? "true" : "false";
    case Type::Nil: return "nil";
    default: return "unknown type";
    }

}

Object::operator std::string() const {
    if (mType != Type::String) throw std::runtime_error("Cannot convert " + typeAsString() + " to String");
    return mString;
}

Object::operator double() const {
    if (mType != Type::Double) throw std::runtime_error("Cannot convert " + typeAsString() + " to Double");
    return mDouble;
}

Object::operator bool() const {
    if (mType != Type::Boolean) throw std::runtime_error("Cannot convert " + typeAsString() + " to Boolean");
    return mBoolean;
}

bool Object::isString() const {
    return mType == Type::String;
}

bool Object::isDouble() const {
    return mType == Type::Double;
}

bool Object::isBoolean() const {
    return mType == Type::Boolean;
}

bool Object::isNil() const {
    return mType == Type::Nil;
}

inline std::string Object::typeAsString() const noexcept {
    switch (mType) {
    case Type::String: return "String";
    case Type::Double: return "Double";
    case Type::Boolean: return "Boolean";
    case Type::Nil: return "Nil";
    default: return "Unknown type";
    }
}

bool operator==(Object const& lhs, Object const& rhs) {
    return lhs.mType == rhs.mType && (
        lhs.mType == Object::Type::Nil
        || lhs.mType == Object::Type::Double && lhs.mDouble == rhs.mDouble
        || lhs.mType == Object::Type::String && lhs.mString == rhs.mString);
}
