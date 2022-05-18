#include "class_hierarchy.h"

#include <cstring>

void dynamic_dispatch_attack()
{
        std::printf("Dynamic dispatch attack\n");
        Base *p = new Derived1();
        Base *q = new Derived2();

        std::printf("Before vtable override. This should call Derived1::f\n");
        p->f();

        memcpy(p, q, sizeof(void *));
        std::printf("After vtable override. This should call Derived2::f\n");
        p->f();
        std::printf("\n");
}