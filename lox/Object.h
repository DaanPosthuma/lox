#pragma once

#include "LoxCallable.h"
#include "LoxClass.h"
#include "LoxInstance.h"
#include <string>
#include <variant>
#include <iostream>

class Nil {};
inline bool operator == (Nil, Nil) { return true; }

class Object {
public:

    Object(std::string string) : mData(string) {}
    Object(double dbl) : mData(dbl) {}
    Object(bool boolean) : mData(boolean) {}
    Object(LoxCallable const& loxCallable) : mData(loxCallable) {}
    Object(LoxClass const& loxClass) : mData(loxClass) {}
    Object(LoxInstance const& loxInstance) : mData(loxInstance) {}
    Object() : mData(Nil{}) {}
    Object(char const*) = delete;

    std::string toString() const;

    explicit operator std::string() const;
    explicit operator double() const;
    explicit operator bool() const;
    explicit operator LoxCallable() const;
    explicit operator LoxClass() const;
    explicit operator LoxInstance() const;
    
    bool isString() const;
    bool isDouble() const;
    bool isBoolean() const;
    bool isLoxCallable() const;
    bool isLoxClass() const;
    bool isLoxInstance() const;
    bool isNil() const;

    friend bool operator == (Object const& lhs, Object const& rhs);

private:
    std::string typeAsString() const noexcept;
    std::variant<std::string, double, bool, Nil, LoxCallable, LoxClass, LoxInstance> mData;

};

inline std::ostream& operator<<(std::ostream& os, Object const& object) {
    os << object.toString();
    return os;
}
