cmake_minimum_required(VERSION 3.10)
project(TestRunnerLua C)
add_executable(test_runner_lua test_runner_lua.c)
target_link_libraries(test_runner_lua lua)