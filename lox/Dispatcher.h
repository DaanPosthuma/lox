#pragma once

#include <functional>
#include <unordered_map>
#include <typeindex>

template <class ReturnType, class BaseType>
class Dispatcher {
public:

    template <class... ConcreteTypes>
    Dispatcher(std::function<ReturnType(ConcreteTypes const&)>&&... concreteFunctions)
    {
        (add<ConcreteTypes>(concreteFunctions), ...);
    }

    template <class ConcreteType>
    void add(std::function<ReturnType(ConcreteType const&)> concreteFunction) {
        mDispatcher.emplace(
            typeid(ConcreteType const&),
            [=](BaseType const& base) {
                return concreteFunction(dynamic_cast<ConcreteType const&>(base));
            });
    }

    ReturnType dispatch(BaseType const& object) const {

        if (auto it = mDispatcher.find(typeid(object)); it != mDispatcher.end()) {
            return it->second(object);
        }
        else {
            auto const errorMessage = std::string("No dispatcher found for type ") + typeid(object).name();
            throw std::exception(errorMessage.c_str());
        }
    }

private:
    std::unordered_map<std::type_index, std::function<ReturnType(BaseType const&)>> mDispatcher;
};
