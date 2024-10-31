#pragma once

#include <memory>
#include <stdexcept>

template <typename T>
class Singleton {
public:
    Singleton(const Singleton& other) = delete;
    Singleton(Singleton&& other) = delete;
    Singleton& operator=(const Singleton& other) = delete;
    Singleton& operator=(Singleton&& other) = delete;
    virtual ~Singleton() = default;
public:
    template <typename... Args>
    static void Init(Args&... args) {
        mInstance = std::make_unique<T>(args...);
    }
    static T& Get() {
        if (!mInstance) {
            throw std::runtime_error("Singleton is not initialized");
        }
        return *mInstance;
    }
protected:
    using Instance = std::unique_ptr<T>;
protected:
    Singleton() = default;
protected:
    static Instance mInstance;
};