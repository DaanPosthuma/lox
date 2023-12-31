#pragma once

#include <functional>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

template <class ReturnType, class BaseType, class... Args>
class Dispatcher {
public:

    template <class... ConcreteTypes>
    Dispatcher(std::string const& name, std::function<ReturnType(ConcreteTypes, Args...)>&&... concreteFunctions) : mName(name)
    {
        (add<ConcreteTypes>(std::forward<decltype(concreteFunctions)>(concreteFunctions)), ...);
    }

    template <class ConcreteType>
    void add(std::function<ReturnType(ConcreteType, Args...)>&& concreteFunction) {
        mDispatcher.emplace(
            typeid(ConcreteType),
            [=](BaseType base, Args&&... args) {
                return concreteFunction(dynamic_cast<ConcreteType>(base), std::forward<Args>(args)...);
            });
    }

    ReturnType dispatch(BaseType object, Args... args) const {

        if (auto it = mDispatcher.find(typeid(object)); it != mDispatcher.end()) {
            return it->second(object, args...);
        }
        else {
            auto const errorMessage = std::string("No ") + mName + " dispatcher found for type " + typeid(object).name();
            throw std::runtime_error(errorMessage);
        }
    }

private:
    std::string mName;
    std::unordered_map<std::type_index, std::function<ReturnType(BaseType, Args...)>> mDispatcher;
};
