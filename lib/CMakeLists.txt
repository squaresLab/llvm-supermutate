add_library(llvmsupermutate SHARED
  Mapping/File.cpp
  Mapping/Function.cpp
  Mapping/Global.cpp
  Mapping/LLVMToSourceMapping.cpp
  Mapping/Instruction.cpp
  Mapping/Value.cpp
  Mapping/Variable.cpp
  Mutation.cpp
  MutationEngine.cpp
)

target_include_directories(llvmsupermutate
  PUBLIC
    ${LLVM_INCLUDE_DIRS}
    ${LLVMSUPERMUTATE_INCLUDE_DIRS}
    nlohmann_json::nlohmann_json
)

target_compile_definitions(llvmsupermutate
  PUBLIC
    ${LLVM_DEFINITIONS}
)

target_compile_options(llvmsupermutate
  PRIVATE
    -Wall -Werror -fno-rtti
)

target_link_libraries(llvmsupermutate PRIVATE nlohmann_json::nlohmann_json)
