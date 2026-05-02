add_executable(reduct-cli reduct.c)
set_target_properties(reduct-cli PROPERTIES OUTPUT_NAME reduct)

target_link_libraries(reduct-cli PRIVATE reduct)

target_include_directories(reduct-cli PRIVATE 
    "${CMAKE_BINARY_DIR}"
)

if(REDUCT_USE_SANITIZERS AND NOT WIN32)
    target_compile_options(reduct-cli PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(reduct-cli PRIVATE -fsanitize=address,undefined)
endif()

if(REDUCT_USE_FUZZER AND NOT (WIN32 OR APPLE))
    add_executable(fuzz fuzz.c)
    target_link_libraries(fuzz PRIVATE reduct_static)
    target_include_directories(fuzz PRIVATE "${CMAKE_BINARY_DIR}")
    target_compile_options(fuzz PRIVATE
        -fsanitize=fuzzer,address,undefined
        -fno-omit-frame-pointer
        -g
    )
    target_link_options(fuzz PRIVATE -fsanitize=fuzzer,address,undefined)
endif()

if(NOT WIN32)
    target_compile_options(reduct-cli PRIVATE -Wall -Wextra -Werror -Wno-uninitialized)
endif()

install(TARGETS reduct-cli 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)