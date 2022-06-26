#include <cstdint>
#include <cstring>
#include <string>

class Identifier {
public:
  virtual void assign(std::string const& s) = 0;
  virtual void print() = 0;
  // ...
};

class DefaultIdentifier : public Identifier {
  unsigned size = 10;
  char *data = new char[20]; // Y's Are overflow canaries

public:
        DefaultIdentifier(){
                unsigned i = 0;
                while (i < 10)
                {
                        data[i++] = 'X';
                }

                while (i < 20)
                {
                        data[i++] = 'Y';
                }

        }
  void assign(std::string const& s) override {
          for (unsigned i = 0; i < size; ++i)
          {
                  data[i] = s[i];
          }

  }

  void print() override {
          std::string s(data, 20);
          std::printf("DefaultIdentifier::print:\t%s\n", s.c_str());
  }
  // ...
};

class SpecialIdentifier : public Identifier {
  unsigned flags;
  char data[10];

  static unsigned constexpr SOME_FLAG_CONSTANT = 0x14;

  public:
  void setSpecificFlags() { flags = SOME_FLAG_CONSTANT; }

  void assign(std::string const& s) override {
    for (unsigned i = 0; i < 10; ++i) {
      data[i] = s[i];
    }
  }

  void print() override {

          std::string s1(data, 10);
          std::printf("SpecialIdentifier::print:\tdata: %s\n", data);
  }

  // ...
};

void g(Identifier* s) {
  SpecialIdentifier *p = dynamic_cast<SpecialIdentifier *>(s);
  if (p) {
    p->setSpecificFlags();
  }

  // Complicated algorithm ...
}

int main()
{
  DefaultIdentifier id;
  SpecialIdentifier id2;
  std::size_t vtable_ptr = (std::size_t)&id;

  // A's are 10 chars, then 10 chars B
  // As wrong size used for id2 is 10 too much this should overflow B's into the overflow buffer
  std::string A_10_B10 = std::string(10, 'A') + std::string(10, 'B');



  // Before attack
  std::printf("After construction. Should be X(10)Y(10)\n");
  id.print();
  g(&id);
  id.assign(A_10_B10);
  std::printf("After assignment. Should be A(10)Y(10)\n");
  id.print();


  // id's vtable ptr is replaced with a SpecialIdentifier vtable ptr
  memcpy(&id, &id2, sizeof(void *));

  g(&id);

 id.assign(A_10_B10);

   std::printf("After attack. Should be A(10)B(10)\n");
   id.print();
}

