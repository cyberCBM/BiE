#pragma once

#include "stdafx.h"

class Singleton {
public:
    static Singleton& Instance() {
        // Since it's a static variable, if the class has already been created,
        // it won't be created again.
        // And it **is** thread-safe in C++11.
        static Singleton myInstance;

        // Return a reference to our instance.
        return myInstance;
    }

    // delete copy and move constructors and assign operators
    Singleton(Singleton const&) = delete;             // Copy construct
    Singleton(Singleton&&) = delete;                  // Move construct
    Singleton& operator=(Singleton const&) = delete;  // Copy assign
    Singleton& operator=(Singleton &&) = delete;      // Move assign

private:
    Singleton() {
        // Constructor code goes here.
    }

protected:
    ~Singleton() {
        // Destructor code goes here.
    }

    // And any other protected methods.
};

namespace utils
{
    template<typename T>
    class Singleton
    {
    private:
        Singleton<T>(const Singleton<T>&) = delete;
        Singleton<T>& operator = (const Singleton<T>&) = delete;

        Singleton<T>() = default;

        static std::unique_ptr<T> m_instance;
        static std::once_flag m_once;

    public:
        virtual ~Singleton<T>() = default;

        static T* getInstance()
        {
            std::call_once(m_once, []() {
                m_instance.reset(new T);
            });
            return m_instance.get();
        }

        template<typename... Args>
        static T* getInstance2nd(Args&& ...args)
        {
            std::call_once(m_once, [&]() {
                m_instance.reset(new T(std::forward<Args>(args)...));
            });
            return m_instance.get();
        }
    };

    template<typename T> std::unique_ptr<T> Singleton<T>::m_instance;
    template<typename T> std::once_flag Singleton<T>::m_once;

}