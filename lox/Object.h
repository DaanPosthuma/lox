#pragma once

#include <string>

class Object {
public:
    Object(std::string string) : mType(Type::String), mString(string) {}
    Object(double dbl) : mType(Type::Double), mDouble(dbl) {}
    Object(bool boolean) : mType(Type::Boolean), mBoolean(boolean) {}
    Object() : mType(Type::Nil) {}

private:

    enum class Type {
        String, Double, Boolean, Nil
    };

    Type mType;
    std::string mString = {};
    double mDouble = 0.0;
    bool mBoolean = false;
};
