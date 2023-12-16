#include "LoxInstance.h"
#include "LoxInstance.h"
#include "LoxInstance.h"
#include "LoxClass.h"
#include "RuntimeError.h"
#include "Token.h"
#include <cassert>
#include <unordered_map>

class LoxInstance::Fields {
public:
    Object const& get(Token const& name) const {
        if (auto const it = mFields.find(name.lexeme()); it != mFields.end()) {
            return it->second;
        }
        throw RuntimeError{ name, "Undefined property '" + name.lexeme() + "'." };
    }
    void set(Token const& name, Object const& object) {
        mFields[name.lexeme()] = object;
    }
private:
    std::unordered_map<std::string, Object> mFields;
};

LoxInstance::LoxInstance(LoxClass const& klass) : mClass(klass), mFields(new LoxInstance::Fields()) {}

LoxClass const& LoxInstance::klass() const { 
    return mClass;
}

Object const& LoxInstance::get(Token const& name) const {
    return mFields->get(name);
}

void LoxInstance::set(Token const& name, Object const& object) {
    mFields->set(name, object);
}
