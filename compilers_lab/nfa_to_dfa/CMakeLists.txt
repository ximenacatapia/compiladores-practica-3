cmake_minimum_required(VERSION 3.16)
project(regex_to_nfa CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(core
    ./src/nfa.cpp
    ./src/regex.cpp
    ./src/dfa.cpp
)

enable_testing()

target_include_directories(core PUBLIC src)

add_executable(regex_to_nfa ./src/main.cpp)
target_link_libraries(regex_to_nfa PRIVATE core)

add_executable(test_e_closure ./tests/test_epsilon_closure.cpp)
target_link_libraries(test_e_closure PRIVATE core)
add_test(NAME PruebaEpsilon COMMAND test_e_closure)

add_executable(test_subset ./tests/test_subset_construction.cpp)
target_link_libraries(test_subset PRIVATE core)
add_test(NAME PruebaSubset COMMAND test_subset)

add_executable(test_move ./tests/test_move.cpp)
target_link_libraries(test_move PRIVATE core)
add_test(NAME PruebaMove COMMAND test_move)