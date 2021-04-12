cmake_minimum_required(VERSION 3.17)
project(simulations)

set(CMAKE_CXX_STANDARD 20)

# ################################################################# #
#                                Engine                             #
# ################################################################# #
add_subdirectory(engine)


# ################################################################# #
#                        Engine Test (Sandbox)                      #
# ################################################################# #
add_subdirectory(sandbox)

# ################################################################# #
#                            Projects                               #
# ################################################################# #

# firefly simulation
add_subdirectory(fireflies)

# firefly simulation with compute shader
add_subdirectory(fireflies_cs)