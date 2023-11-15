#pragma once

#include <string>

class Object {
public:

    Object(std::string string) : mType(Type::String), mString(string) {}
    Object(double dbl) : mType(Type::Double), mDouble(dbl) {}
    Object(bool boolean) : mType(Type::Boolean), mBoolean(boolean) {}
    Object() : mType(Type::Nil) {}

    std::string toString() const { 
        switch (mType) {
        case Type::String: return mString;
        case Type::Double: return std::to_string(mDouble);
        case Type::Boolean: return mBoolean ? "true" : "false";
        case Type::Nil: return "nil";
        default: return "unknown type";
        }
        
    }

    friend bool operator == (Object const& lhs, Object const& rhs);

private:

    enum class Type {
        String, Double, Boolean, Nil
    };

    Type mType;
    std::string mString = {};
    double mDouble = 0.0;
    bool mBoolean = false;
};

inline bool operator == (Object const& lhs, Object const& rhs) {
    return lhs.mType == rhs.mType && (
        lhs.mType == Object::Type::Nil
        || lhs.mType == Object::Type::Double && lhs.mDouble == rhs.mDouble
        || lhs.mType == Object::Type::String && lhs.mString == rhs.mString);
}
