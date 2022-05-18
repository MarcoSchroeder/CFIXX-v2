#include "class_hierarchy.h"

#include <cstdlib>
#include <ctime>
#include <cstdint>
#include <cstring>

void dynamic_cast_attack()
{
        std::printf("Dynamic cast attack\n");

        Base *p, *q;

        srand(time(NULL));
        if (rand() % 10 < 5) // Prevent compiler from optimizing out dynamic_cast. Only important thing is that p and q point to different classes
        {
                std::printf("Object has dynamic type Derived1\n");
                p = new Derived1();
                q = new Derived2();
        }
        else
        {
                std::printf("Object has dynamic type Derived2\n");
                p = new Derived2();
                q = new Derived1();
        }

        std::printf("Before vtable override. Dynamic cast should return nullptr for invalid cast\n");
        std::printf("Cast to Derived1: %ul\n", (std::uint64_t) dynamic_cast<Derived1 *>(p));
        std::printf("Cast to Derived2: %ul\n", (std::uint64_t) dynamic_cast<Derived2 *>(p));

        memcpy(p, q, sizeof(void *));
        std::printf("After vtable override. Dynamic cast should return nullptr for previously valid cast\n");
        std::printf("Cast to Derived1: %ul\n", (std::uint64_t) dynamic_cast<Derived1 *>(p));
        std::printf("Cast to Derived2: %ul\n", (std::uint64_t) dynamic_cast<Derived2 *>(p));
        std::printf("\n");
}