add_library(llvmtosource MODULE
  LLVMToSourceMappingPass.cpp
)

target_include_directories(llvmtosource
  PRIVATE
    ${LLVM_INCLUDE_DIRS}
    ${LLVMSUPERMUTATE_INCLUDE_DIRS}
    nlohmann_json::nlohmann_json
)

target_compile_definitions(llvmtosource
  PUBLIC
    ${LLVM_DEFINITIONS}
)

target_compile_options(llvmtosource
  PRIVATE
    -Wall -Werror -fno-rtti -g
)

target_link_libraries(llvmtosource
  PUBLIC
    llvmsupermutate
    nlohmann_json::nlohmann_json
)

install(TARGETS llvmtosource
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
)
