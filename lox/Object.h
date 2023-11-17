#pragma once

#include <string>

class Object {
public:

    Object(std::string string) : mType(Type::String), mString(string) {}
    Object(double dbl) : mType(Type::Double), mDouble(dbl) {}
    Object(bool boolean) : mType(Type::Boolean), mBoolean(boolean) {}
    Object() : mType(Type::Nil) {}

    std::string toString() const;

    explicit operator std::string() const;
    explicit operator double() const;
    explicit operator bool() const;
    
    bool isString() const;
    bool isDouble() const;
    bool isBoolean() const;
    bool isNil() const;

    friend bool operator == (Object const& lhs, Object const& rhs);

private:

    enum class Type {
        String, Double, Boolean, Nil
    };

    std::string typeAsString() const noexcept;

    Type mType;
    std::string mString = {};
    double mDouble = 0.0;
    bool mBoolean = false;
};

