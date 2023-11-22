# Describe sim_app build

add_executable(sim_app src/sim_app.cpp)
target_link_libraries(sim_app PRIVATE sim::simulator)
