#include <cassert>
#include <cstring>
#include <typeinfo>

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

int main()
{

        Base *p = new Derived1();
        Base *q = new Derived2();

        using ptr_t = unsigned long;
        ptr_t derived2_vtable_ptr = *(ptr_t *)q;
        ptr_t *metadata = (ptr_t *)0x7f0104846000UL;
        ptr_t first_index = (ptr_t)p >> 26;
        ptr_t second_index = (ptr_t)p >> 3 & ((1UL << 23) - 1UL);
        ptr_t *secondlevel = (ptr_t *)metadata[first_index];
        secondlevel[second_index] = derived2_vtable_ptr;
        assert(nullptr != dynamic_cast<Derived2 *>(p));
}