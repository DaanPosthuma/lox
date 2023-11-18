#pragma once

#include <functional>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

template <class ReturnType, class BaseType, class... Args>
class Dispatcher {
public:

    template <class... ConcreteTypes>
    Dispatcher(std::function<ReturnType(ConcreteTypes const&, Args...)>&&... concreteFunctions)
    {
        (add<ConcreteTypes>(concreteFunctions), ...);
    }

    template <class ConcreteType>
    void add(std::function<ReturnType(ConcreteType const&, Args...)> concreteFunction) {
        mDispatcher.emplace(
            typeid(ConcreteType const&),
            [=](BaseType const& base, Args... args) {
                return concreteFunction(dynamic_cast<ConcreteType const&>(base), args...);
            });
    }

    ReturnType dispatch(BaseType const& object, Args... args) const {

        if (auto it = mDispatcher.find(typeid(object)); it != mDispatcher.end()) {
            return it->second(object, args...);
        }
        else {
            auto const errorMessage = std::string("No dispatcher found for type ") + typeid(object).name();
            throw std::runtime_error(errorMessage);
        }
    }

private:
    std::unordered_map<std::type_index, std::function<ReturnType(BaseType const&, Args...)>> mDispatcher;
};
