#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <functional>
#include <Any.hpp>

using namespace std;

class ioc_container
{
public:
    ioc_container(void) {}
    ioc_container(const ioc_container&) = delete; // deleted
    ioc_container& operator = (const ioc_container&) = delete; // deleted  
    ~ioc_container(void) {}

    template<class T, typename Depend, typename... Args>
    void register_type(const string& str_key)
    {
        //通过闭包擦除了参数类型
        std::function<T* (Args...)> function = [](Args... args)
        {
            return new T(new Depend(args...));
        };
        register_type(str_key, function);
    }

    template<class T, typename... Args>
    T* resolve(const string& str_key, Args... args)
    {
        if (m_creator_map.find(str_key) == m_creator_map.end())
        {
            return nullptr;
        }

        Any resolver = m_creator_map[str_key];
        std::function<T* (Args...)> function = resolver.any_cast<std::function<T* (Args...)>>();

        return function(args...);
    }

    template<class T, typename... Args>
    std::shared_ptr<T> resolve_shared(const string& str_key, Args... args)
    {
        T* t = resolve<T>(str_key, args...);
        return std::shared_ptr<T>(t);
    }

private:
    void register_type(const string& str_key, Any constructor)
    {
        if (m_creator_map.find(str_key) != m_creator_map.end())
        {
            throw std::invalid_argument("this key has already exist!");
        }

        //通过Any擦除了不同类型的构造器
        m_creator_map.emplace(str_key, constructor);
    }

private:
    unordered_map<string, Any> m_creator_map;
};

