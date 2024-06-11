//
// Created by bjoern on 5/31/24.
//

#ifndef SINGLETON_HPP
#define SINGLETON_HPP
#include "spdlog/spdlog.h"
#include <memory>
#include <mutex>

/**
 *<p>The <b>Singleton</b> design pattern: There can only be one.
 * However, this implementation of the Singleton design pattern is not Thread safe, so the PhysicsEngine should not be
 * created in parallel threads.
 * We accept this implementation, because the PhysicsEngine will be created in the game loop or before, where no
 * concurrency is intended.</p>
 */

template <typename T> class Singleton {

  public:
    /**
     * Singletons should not be cloneable.
     */
    Singleton(T &other) = delete;

    /**
     * Singletons should not be assignable.
     */
    void operator=(const T &) = delete;

    /**
     * This is the static method that controls the access to the singleton
     * instance. On the first run, it creates the Singleton object and places it
     * into the static field. On subsequent runs, it returns the client existing
     * object stored in the static field.
     */
    static T &getInstance() {
        static std::once_flag flag;
        if (!isInit) {
            std::call_once(flag, []() { instance.reset(new T()); });
        }
        return *instance;
    };

  private:
    struct Deleter {
        void operator()(const T *ptr) const { delete ptr; }
    };

    static std::unique_ptr<T, Deleter> instance;
    static bool isInit;

  protected:
    Singleton() {
        isInit = true;
        spdlog::info("A singleton instance of type {} was created.", typeid(T).name());
    };
    ~Singleton() {
        isInit = false;
        spdlog::info("A singleton instance of type {} was destroyed.", typeid(T).name());
    };
};

template <typename T> std::unique_ptr<T, typename Singleton<T>::Deleter> Singleton<T>::instance = nullptr;

template <typename T> bool Singleton<T>::isInit = false;

#endif // SINGLETON_HPP
