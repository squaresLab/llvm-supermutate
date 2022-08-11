add_library(llvmsupermutator MODULE
  LLVMSupermutatorPass.cpp
)

target_include_directories(llvmsupermutator
  PRIVATE
    ${LLVM_INCLUDE_DIRS}
    ${LLVMSUPERMUTATE_INCLUDE_DIRS}
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

target_compile_definitions(llvmsupermutator
  PUBLIC
    ${LLVM_DEFINITIONS}
)

target_compile_options(llvmsupermutator
  PRIVATE
    -Wall -Werror -fno-rtti
)

target_link_libraries(llvmsupermutator
  PUBLIC
    llvmsupermutate
    nlohmann_json::nlohmann_json
    spdlog::spdlog
)

install(TARGETS llvmsupermutator
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
