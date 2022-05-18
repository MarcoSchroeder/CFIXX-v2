#include "class_hierarchy.h"

#include <cstring>
#include <typeinfo>

void typeid_attack()
{
        std::printf("typeid attack\n");
        Base *p = new Derived1();
        Base *q = new Derived2();

        std::printf("Before vtable override. typeid.name() should be Derived1\n");
        std::printf("%s\n", typeid(*p).name());

        memcpy(p, q, sizeof(void *));
        std::printf("After vtable override. typeid.name() should be Derived2\n");
        std::printf("%s\n", typeid(*p).name());
        std::printf("\n");
}