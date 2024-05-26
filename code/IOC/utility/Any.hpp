#pragma once
#include <memory>
#include <typeindex>
#include <exception>
#include <iostream>

struct Any
{
    Any ( void ) : m_tpIndex ( std::type_index ( typeid ( void ) ) ) {}
    Any ( const Any& that ) : m_ptr ( that.clone() ), m_tpIndex ( that.m_tpIndex ) {}
    Any ( Any&& that ) : m_ptr ( std::move ( that.m_ptr ) ), m_tpIndex ( that.m_tpIndex ) {}

    //创建智能指针时，对于一般的类型，通过std::decay来移除引用和cv符，从而获取原始类型
    template < typename T, class = typename std::enable_if < !std::is_same<typename std::decay<T>::type, Any>::value, T >::type > Any(T && value) : m_ptr(new derived_type < typename std::decay<T>::type>(std::forward<T>(value))),
        m_tpIndex(std::type_index(typeid(typename std::decay<T>::type))) {}

    bool is_nTll() const
    {
        return !bool ( m_ptr );
    }

    template<class T> bool is() const
    {
        return m_tpIndex == std::type_index ( typeid ( T ) );
    }

    //将Any转换为实际的类型
    template<class T> T& any_cast()
    {
        if ( !is<T>() )
        {
            std::cout << "can not cast " << typeid ( T ).name() << " to " << m_tpIndex.name() << std::endl;
            throw std::logic_error{"bad cast"};
        }
        auto derived = dynamic_cast<derived_type<T>*> ( m_ptr.get() );
        return derived->m_value;
    }

    Any& operator= ( const Any& a )
    {
        if ( m_ptr == a.m_ptr )
        {
            return *this;
        }
        m_ptr = a.clone();
        m_tpIndex = a.m_tpIndex;
        return *this;
    }

    Any& operator= ( Any&& a )
    {
        if ( m_ptr == a.m_ptr )
        {
            return *this;
        }
        m_ptr = std::move ( a.m_ptr );
        m_tpIndex = a.m_tpIndex;
        return *this;
    }

private:
    struct base_type;
    using base_ptr =   std::unique_ptr<base_type>;

    struct base_type
    {
        virtual ~base_type() {}
        virtual base_ptr clone() const = 0;
    };

    template<typename T>
    struct derived_type : base_type
    {
        template<typename K>
        derived_type ( K&& value ) : m_value ( std::forward<K> ( value ) )
        {
        }

        base_ptr clone() const
        {
            return base_ptr ( new derived_type<T> ( m_value ) );
        }

        T m_value;
    };

    base_ptr clone() const
    {
        if ( m_ptr != nullptr )
        {
            return m_ptr->clone();
        }

        return nullptr;
    }

private:
    base_ptr m_ptr;

    std::type_index m_tpIndex;
};
