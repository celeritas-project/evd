#-----------------------------------------------------------------------------#
#/*!
# * \file   CMakeLists.txt
# * \author Stefano Tognini
# * \brief  CMakeLists for the Event Display of the Celeritas Project.
# * \note   Copyright (c) 2020 Oak Ridge National Laboratory, UT-Battelle, LLC.
# */
#-----------------------------------------------------------------------------#

#------------------------------------------------------------------------------
# Setting up the project
#------------------------------------------------------------------------------
project(Evd LANGUAGES CXX)

cmake_minimum_required(VERSION 3.9 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 14)

#------------------------------------------------------------------------------
# Setting environmental variables
#------------------------------------------------------------------------------
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#------------------------------------------------------------------------------
# Finding packages
#------------------------------------------------------------------------------
find_package(ROOT REQUIRED)

#------------------------------------------------------------------------------
# Adding the executable, and linking it to the needed libraries
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
# Installing the executable to the current directory
#------------------------------------------------------------------------------
install(TARGETS evd DESTINATION .)
