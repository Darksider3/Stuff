#ifndef CREATIONAL_H
#define CREATIONAL_H
#include <functional>
#include <iostream>
#include <memory>

namespace Li {
namespace creational {
template<typename creationT>
struct construct_unique {

    static const bool is_unique_constructible { true };
    static const bool is_shared_constructible { false };
    static const bool is_weak_constructible { false };
    static const bool is_plainptr_constructible { false };

    static std::unique_ptr<creationT> create()
    {
        return std::move(std::make_unique<creationT>());
    }

    template<typename F>
    static std::unique_ptr<creationT> create(F var)
    {
        return std::move(std::make_unique<creationT>(std::forward<F>(var)));
    }

    template<typename... F>
    static std::unique_ptr<creationT> create(F... variables)
    {
        return std::move(std::make_unique<creationT>(std::forward<F>(variables)...));
    }
};

template<typename creationT>
struct construct_shared {
    static const bool is_unique_constructible { false };
    static const bool is_shared_constructible { true };
    static const bool is_weak_constructible { false };
    static const bool is_plainptr_constructible { false };
};

template<typename creationT>
struct construct_weak {
    static const bool is_unique_constructible { false };
    static const bool is_shared_constructible { false };
    static const bool is_weak_constructible { true };
    static const bool is_plainptr_constructible { false };

    static std::weak_ptr<creationT> create()
    {
        std::weak_ptr<creationT> ret;
        return std::move(ret);
    }

    template<typename F>
    static std::weak_ptr<creationT> create(F var)
    {
        std::weak_ptr<creationT> ret(std::forward<F>(var));
        return std::move(ret);
    }

    template<typename... F>
    static std::weak_ptr<creationT> create(F... variables)
    {
        std::weak_ptr<creationT> ret(std::forward<F>(variables)...);
    }
};

template<typename creationT>
struct createablePointer_Plain {

    static const bool is_plainptr_constructible { true };

    static creationT* create()
    {
        return new creationT();
    }

    template<typename F>
    static creationT* create(F var)
    {
        return new creationT(std::forward<F>(var));
    }

    template<typename... F>
    static creationT* create(F... variables)
    {
        return new creationT(std::forward<F>(variables)...);
    }
};

}
}
#endif // CREATIONAL_H
