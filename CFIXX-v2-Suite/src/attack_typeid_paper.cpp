//#include "class_hierarchy.h"

#include <cstring>
#include <typeinfo>
#include <cassert>

class Base
{
        virtual void f() {}
};

class Derived1 : public Base
{
};

class Derived2 : public Base
{
};

void typeid_attack()
{
        Base *p = new Derived1();
        Base *p2 = new Derived1();
        Base *q = new Derived2();
        assert(typeid(p2) == typeid(p));
        memcpy(p, q, sizeof(void *));
        assert(typeid(p2) == typeid(p));
}