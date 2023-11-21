# Describe simulator mudule build

add_executable(riscv src/riscv.cpp)
target_link_libraries(riscv PRIVATE sim::simulator)