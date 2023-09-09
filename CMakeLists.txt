# Describe project build

cmake_minimum_required(VERSION 3.11)

project(sim2023 VERSION 0.1 LANGUAGES CXX)

add_subdirectory(csr)
add_subdirectory(gpr)
add_subdirectory(hart)
add_subdirectory(memory)
add_subdirectory(simulator)
