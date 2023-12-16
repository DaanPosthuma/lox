#pragma once
#include <string>
#include <memory>
#include "LoxClass.h"
class Token;

class LoxInstance {
public:
    LoxInstance(LoxClass const& klass);
    LoxClass const& klass() const;
    Object const& get(Token const& name) const;
    void set(Token const& name, Object const& value);

private:
    class Fields;
    LoxClass mClass;
    std::shared_ptr<Fields> mFields;
};
