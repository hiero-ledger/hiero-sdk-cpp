set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)

set(VCPKG_CMAKE_SYSTEM_NAME Linux)

# Use Clang as the compiler
set(VCPKG_C_COMPILER clang)
set(VCPKG_CXX_COMPILER clang++)

# Force libc++ as the standard library
set(VCPKG_CXX_FLAGS "-stdlib=libc++")
set(VCPKG_C_FLAGS "")

# Link against libc++ and libc++abi
set(VCPKG_LINKER_FLAGS "-stdlib=libc++ -lc++abi")
