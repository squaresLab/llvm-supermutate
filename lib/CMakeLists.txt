add_library(llvmsupermutate SHARED
  MutationEngine.cpp
)
target_include_directories(llvmsupermutate
  PUBLIC
    ${CLANG_INCLUDE_DIRS}
    ${LLVM_INCLUDE_DIRS}
    ${LLVMSUPERMUTATE_INCLUDE_DIRS}
    nlohmann_json::nlohmann_json
)

target_compile_definitions(llvmsupermutate
  PUBLIC
    ${CLANG_DEFINITIONS}
    ${LLVM_DEFINITIONS}
)
