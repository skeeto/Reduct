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
    foreach(fuzzer fuzz fuzz_parse fuzz_eval)
        add_executable(${fuzzer} ${fuzzer}.c)
        target_link_libraries(${fuzzer} PRIVATE reduct_static)
        target_include_directories(${fuzzer} PRIVATE "${CMAKE_BINARY_DIR}")
        target_compile_options(${fuzzer} PRIVATE
            -fsanitize=fuzzer,address,undefined
            -fno-omit-frame-pointer
            -g
        )
        target_link_options(${fuzzer} PRIVATE -fsanitize=fuzzer,address,undefined)
    endforeach()
endif()

if(NOT WIN32)
    target_compile_options(reduct-cli PRIVATE -Wall -Wextra -Werror -Wno-uninitialized)
endif()

install(TARGETS reduct-cli 
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)