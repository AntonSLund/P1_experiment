cmake_minimum_required(VERSION 4.2)
project(P1_experiment C)

set(CMAKE_C_STANDARD 17)

add_executable(P1_experiment main.c)
target_compile_options(P1_experiment PRIVATE -fsanitize=address)
target_link_options(P1_experiment PRIVATE -fsanitize=address)