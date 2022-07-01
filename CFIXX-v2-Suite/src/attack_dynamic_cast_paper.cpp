//#include "class_hierarchy.h"

#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <cstring>
#include <cassert>
#include <cstdio>

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

void dynamic_cast_attack()
{
        Base *p, *q;
        p = new Derived1;
        q = new Derived2;
        assert(0 != dynamic_cast<Derived1 *>(p));
        assert(0 == dynamic_cast<Derived2 *>(p));
        memcpy(p, q, sizeof(void *));
        assert(0 != dynamic_cast<Derived1 *>(p));
        assert(0 == dynamic_cast<Derived2 *>(p));
}
