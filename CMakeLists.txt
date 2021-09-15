cmake_minimum_required(VERSION 3.13)
project(llvm-supermutate
  VERSION 0.1
  DESCRIPTION "Efficient mutation testing and repair for LLVM via supermutants"
  LANGUAGES CXX
)

include(GNUInstallDirs)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM: ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")
message(STATUS "Using LLVM libs: ${LLVM_LIBRARY_DIRS}")

set(LLVMSUPERMUTATE_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include)
install(DIRECTORY ${LLVMSUPERMUTATE_INCLUDE_DIRS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

add_subdirectory(extern)
add_subdirectory(lib)
add_subdirectory(src)
