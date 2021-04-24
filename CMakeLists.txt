cmake_minimum_required(VERSION 3.17)
project(simulations)

set(CMAKE_CXX_STANDARD 20)

# ################################################################# #
#                                Engine                             #
# ################################################################# #
add_subdirectory(engine)

# append cmake utilities to CMAKE_MODULE_PATH
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/engine/cmake_modules)
include(add_resources REQUIRED)


# ################################################################# #
#                        Engine Test (Sandbox)                      #
# ################################################################# #
add_subdirectory(sandbox)

# ################################################################# #
#                            Projects                               #
# ################################################################# #
add_subdirectory(projects)