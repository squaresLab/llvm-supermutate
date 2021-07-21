cmake_minimum_required(VERSION 3.13)
project(llvm-supermutate
  VERSION 0.1
  DESCRIPTION "Efficient mutation testing and repair for LLVM via supermutants"
  LANGUAGES CXX
)

# set(CMAKE_CXX_STANDARD 14)
# set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM: ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using LLVM libs: ${LLVM_LIBRARY_DIRS}")

add_subdirectory(extern)
add_subdirectory(src)
