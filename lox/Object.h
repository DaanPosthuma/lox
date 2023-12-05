#pragma once

#include "LoxCallable.h"
#include <string>
#include <variant>

class Nil {};
inline bool operator == (Nil, Nil) { return true; }

class Object {
public:

    Object(std::string string) : mData(string) {}
    Object(double dbl) : mData(dbl) {}
    Object(bool boolean) : mData(boolean) {}
    Object(LoxCallable const& loxCallable) : mData(loxCallable) {}
    Object() : mData(Nil{}) {}

    std::string toString() const;

    explicit operator std::string() const;
    explicit operator double() const;
    explicit operator bool() const;
    explicit operator LoxCallable() const;
    
    bool isString() const;
    bool isDouble() const;
    bool isBoolean() const;
    bool isLoxCallable() const;
    bool isNil() const;

    friend bool operator == (Object const& lhs, Object const& rhs);

private:
    std::string typeAsString() const noexcept;
    std::variant<std::string, double, bool, Nil, LoxCallable> mData;

};

