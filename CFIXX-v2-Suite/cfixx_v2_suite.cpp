#include <cstdio>
#include <cstdint>
#include <cstring>
#include <typeinfo>

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

void print_vtable_ptr(void *p, char const *name)
{
  std::printf("%s vtable ptr:\t%u\n", name, *reinterpret_cast<std::uint64_t *>(p));
}

void attack1_dynamic_dispatch()
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

void attack2_dynamic_cast()
{
  std::printf("Dynamic cast attack\n");
  Base *p = new Derived1();
  Base *q = new Derived2();

  std::printf("Before vtable override. Dynamic cast should return nullptr\n");
  std::printf("%ul\n", (std::uint64_t) dynamic_cast<Derived2 *>(p));

  memcpy(p, q, sizeof(void *));
  std::printf("After vtable override. Dynamic cast should return a valid ptr\n");
  std::printf("%ul\n", (std::uint64_t) dynamic_cast<Derived2 *>(p));
  std::printf("\n");
}

void attack3_typeid_operator()
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

int main()
{
  attack1_dynamic_dispatch();
  attack2_dynamic_cast();
  attack3_typeid_operator();
}