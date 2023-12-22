#pragma once

class Object;
class LoxInstance;
class LoxCallable;

#include <string>
#include <vector>
#include <optional>
#include <memory>
#include <unordered_map>

class LoxClass {
public:
    LoxClass(std::string const& name, std::optional<LoxClass> const& superclass, std::unordered_map<std::string, LoxCallable> const& methods);
    std::string const& name() const { return mName; }
    int arity() const;
    LoxInstance operator()(std::vector<Object> const& arguments) const;
    Object findMethod(std::string const& name) const;

private:
    class Methods;
    std::string mName;
    std::shared_ptr<const Methods> mMethods;

};
