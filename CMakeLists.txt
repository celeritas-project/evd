#-----------------------------------------------------------------------------#
# \file   CMakeLists.txt
# \author Stefano Tognini
# \brief  Evd CMakeLists
# \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
#-----------------------------------------------------------------------------#

#------------------------------------------------------------------------------
# Set up the project
#------------------------------------------------------------------------------
project(Evd LANGUAGES CXX)

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)

#------------------------------------------------------------------------------
# Find packages
#------------------------------------------------------------------------------
find_package(ROOT)

#------------------------------------------------------------------------------
# Add the executable and link it to the needed libraries
#------------------------------------------------------------------------------
add_executable(evd main.cpp src/Evd.cpp src/Evd.hh)

target_include_directories(evd PRIVATE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
)

target_link_libraries(evd PRIVATE
  ROOT::Core
  ROOT::Eve
  ROOT::Rint
)

#------------------------------------------------------------------------------
# Install the executable to the current directory
#------------------------------------------------------------------------------
install(TARGETS evd DESTINATION .)
