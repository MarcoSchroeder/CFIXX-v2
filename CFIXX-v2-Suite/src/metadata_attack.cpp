#include "class_hierarchy.h"

#include <cstring>

int main()
{
        std::printf("Before metadata attack\n");
        std::printf("Dynamic dispatch attack\n");
        Base *p = new Derived1();
        Base *q = new Derived2();

        std::printf("Before vtable override. This should call Derived1::f\n");
        p->f();

        memcpy(p, q, sizeof(void *));
        std::printf("After vtable override. This should call Derived2::f\n");
        p->f();
        std::printf("\n");

        // Metadata attack
        using ptr_t = unsigned long;
        ptr_t derived2_vtable_ptr = *(ptr_t *)q;
        ptr_t *metadata = (ptr_t *)0x7f0104846000UL;
        ptr_t first_index = (ptr_t)p >> 26;
        ptr_t second_index = (ptr_t)p >> 3 & ((1UL << 23) - 1UL);
        ptr_t *secondlevel = (ptr_t *)metadata[first_index];
        secondlevel[second_index] = derived2_vtable_ptr;

        std::printf("After metadata override. This should call Derived2::f\n");
        p->f();
        std::printf("\n");
}