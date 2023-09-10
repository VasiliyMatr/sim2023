# Describe project build

cmake_minimum_required(VERSION 3.11)

project(sim2023 VERSION 0.1 LANGUAGES CXX)

# Compile commands export for language server
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# C++20 is the default standard
set(CMAKE_CXX_STANDARD 20)

# Add header only simulator module library
function(add_sim_header_module MODULE_NAME)
    add_library(${MODULE_NAME} INTERFACE)
    # Alias for linkage
    add_library(sim::${MODULE_NAME} ALIAS ${MODULE_NAME})

    # Default properties
    target_include_directories(${MODULE_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
endfunction(add_sim_header_module)

# Add simulator module library
function(add_sim_module MODULE_NAME SOURCES)
    add_library(${MODULE_NAME} STATIC ${SOURCES})
    # Alias for linkage
    add_library(sim::${MODULE_NAME} ALIAS ${MODULE_NAME})

    # Default properties
    target_include_directories(${MODULE_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
endfunction(add_sim_module)

add_subdirectory(csr)
add_subdirectory(gpr)
add_subdirectory(hart)
add_subdirectory(memory)
add_subdirectory(simulator)