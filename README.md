# CFIXX
Attackers try to hijack the control-flow of a victim’s process by exploiting a run-time vulnerability. Vtable hijacking is a state-of-the-art technique adversaries use to conduct control-flow hijacking attacks. It abuses the reliance of language constructs related to polymorphism on dynamic type information. The Control Flow Integrity (CFI) security policy is a well-established solution designed to prevent attacks that corrupt the control-flow. Deployed defense mechanisms based on CFI are often generic, which means that they do not consider high-level programming language semantics. This makes them vulnerable to vtable hijacking attacks. Object Type Integrity (OTI) is an orthogonal security policy that specifically addresses vtable hijacking. CFIXX is a Clang compiler extension that enforces OTI in the context of dynamic dispatch, which prevents vtable hijacking in this setting. However, this extension does not enforce OTI in context of polymorphism. The contribution of this work is a practical implementation to enable OTI in the context of C++’s run-time type information for the dynamic_cast expressions and the typeid operator.

Link to publication: https://journals.ub.uni-heidelberg.de/index.php/emcl-pp/article/view/94354

For details, see the paper CFIXX: Object Type Integrity for C++ Virtual
Disaptch at NDSS '18.

# Setup

Dependencies:
cmake
ninja
wget
binutils-dev

Assumes the system linker is the gold linker.  Recent versions (e.g. current release) of llvm, cmake, ninja
preferred.  

To build:
./configure.sh
make



**cfixxv2:**

Some files from the original repo needed to be fixed to prevent compilation errors. They can be found in `CFIXX-v2-Build-Fix`

compile programs using the clang++ from release install and set the following flags:

-O2 // required by cfixx

-fuse-ld=gold // use gold linker

-stdlib=libc++ // link against libc++ standard library, which causes the abi library that the standard library was linked against to be linked: libc++abi in this case

it is necessary to include the dir containing the standard library in the ld_library_path, which is searched to find dynamic dependencies at application startup


`ldd` may be used to check dynamic dependencies of a binary
