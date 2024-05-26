#include "Any.hpp"
#include "Ioc.hpp"
#include <cxxabi.h>


#define show_class_name(class_this) \
{ \
    int status = 0; \
    char* demangledName = abi::__cxa_demangle(typeid(*class_this).name(), nullptr, nullptr, &status); \
    if (status == 0)  \
    { \
        std::cout << "I am "<<demangledName<<" calling Func" << std::endl; \
        free(demangledName);    \
    } \
}


struct base
{
    virtual void Func() {}
    virtual ~base() {}
};

struct derived_b : public base
{
    derived_b(int a, double b): m_a(a), m_b(b)
    {
    }
    void Func()override
    {
        show_class_name(this);
        std::cout << m_a + m_b << std::endl;
    }
private:
    int m_a;
    double m_b;
};


struct derived_c : public base
{
    void Func()override
    {
        show_class_name(this);
    }
};

struct A
{
    A(base * ptr) : m_ptr(ptr)
    {
    }

    void Func()
    {
        m_ptr->Func();
    }

    ~A()
    {
        if(m_ptr != nullptr)
        {
            delete m_ptr;
            m_ptr = nullptr;
        }
    }

private:
    base * m_ptr;
};

void TestIoc()
{
    ioc_container ioc;
    ioc.register_type<A, derived_c>("C");      //配置依赖关系
    auto c = ioc.resolve_shared<A>("C");
    c->Func();

    ioc.register_type<A, derived_b, int, double>("B");   //注册时要注意derived_b的参数int和double
    auto b = ioc.resolve_shared<A>("B", 1, 2.5); //还要传入参数
    b->Func();
}

int main(void)
{
    std::cout<<"Hello, world!"<<std::endl;
    TestIoc();
    std::cout<<"See You next time!"<<std::endl;

    return 0;

}



