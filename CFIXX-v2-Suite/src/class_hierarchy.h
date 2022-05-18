#include <cstdio>

class Base
{
public:
        virtual void f()
        {
                std::printf("Base::f\n");
        }
};

class Derived1 : public Base
{
public:
        void f() override
        {
                std::printf("Derived1::f\n");
        }
};

class Derived2 : public Base
{
public:
        void f() override
        {
                std::printf("Derived2::f\n");
        }
};