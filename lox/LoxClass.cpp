#include "LoxClass.h"
#include "LoxClass.h"
#include "LoxClass.h"
#include "Object.h"
#include "LoxCallable.h"
#include <cassert>

class LoxClass::Methods {
public:
    Methods(std::unordered_map<std::string, LoxCallable> const& methods, std::optional<LoxClass> const& superclass) 
        : mMethods(methods), mSuperclassMethods(superclass ? superclass->mMethods : nullptr) {}

    Object findMethod(std::string const& name) const {
        if (mMethods.contains(name)) return mMethods.at(name);
        if (mSuperclassMethods) return mSuperclassMethods->findMethod(name);
        return Object();
    }
private:
    std::unordered_map<std::string, LoxCallable> mMethods;
    std::shared_ptr<const Methods> mSuperclassMethods;
};

LoxClass::LoxClass(std::string const& name, std::optional<LoxClass> const& superclass, std::unordered_map<std::string, LoxCallable> const& methods)
    : mName(name), mMethods(new Methods(methods, superclass)) {
}

int LoxClass::arity() const { 
    auto const initializer = findMethod("init");
    if (initializer.isLoxCallable()) {
        return static_cast<LoxCallable>(initializer).arity();
    }
    return 0;
}

LoxInstance LoxClass::operator()(std::vector<Object> const& arguments) const {
    auto const instance = LoxInstance(*this);
    auto const initializer = findMethod("init");
    if (initializer.isLoxCallable()) {
        static_cast<LoxCallable>(initializer).bind(instance)(arguments);
    }
    return instance;
}

Object LoxClass::findMethod(std::string const& name) const {
    return mMethods->findMethod(name);
}
