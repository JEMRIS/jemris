cmake_minimum_required (VERSION 2.8) 

project (jemris) 
set (jemris_VERSION_MAJOR 2)
set (jemris_VERSION_MINOR 7)
set (jemris_VERSION_PATCH 0)
set (jemris_VERSION "${jemris_VERSION_MAJOR}.${jemris_VERSION_MINOR}.${jemris_VERSION_PATCH}")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

macro(set_config_option VARNAME STRING)
  set(${VARNAME} TRUE)
  list(APPEND CONFIG_OPTIONS ${STRING})
  message(STATUS "Found " ${STRING})
endmacro(set_config_option)

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

configure_file (
  "${PROJECT_SOURCE_DIR}/cmake/config.h.in"
  "${PROJECT_SOURCE_DIR}/src/config.h"
  )

include(ConfigureChecks.cmake)
add_definitions(-DHAVE_CONFIG_H)

# OS type -----------------------------------------------------------------------
if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(MACOSX TRUE)
endif()

# Architecture ------------------------------------------------------------------
include (VcMacros)
include (OptimizeForArchitecture)
find_package (OpenMP)
if(OPENMP_FOUND)
  message("OPENMP FOUND")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
  message ("${OpenMP_CXX_FLAGS}")
  message ("${CMAKE_CXX_FLAGS}")
endif()

# C++ flags ---------------------------------------------------------------------
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi -DTIXML_USE_STL -fPIC -O3") 
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTIXML_USE_STL -O3 -Itr1")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /DTIXML_USE_STL /EHsc /Ox
  /nologo /MT /wd4267 /wd4244 /wd4190 /wd4996 /LD /MD /DEXP_STL") 
  set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
  set (CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:LIBCMT")
endif()

include(CheckTypeSize)
include(CheckFunctionExists)
include(CheckIncludeFile)

enable_testing()

if (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set(WINDOWS TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  set(LINUX TRUE)
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  set(MACOSX TRUE)
endif()

include (VcMacros)
include (OptimizeForArchitecture)

check_type_size("void*" SIZEOF_VOID_P)
if(SIZEOF_VOID_P EQUAL 8)
  set_config_option(HAVE_64BIT_SIZE_T "Have64BitSizeT")
endif(SIZEOF_VOID_P EQUAL 8)

find_package(Boost 1.36.0)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  set (HAVE_BOOST ON CACHE BOOL "Found boost")
endif()

find_package(XercesC REQUIRED)

find_package(GiNaC REQUIRED)
include_directories (${GINAC_INCLUDE_DIRS})

find_package(Sundials COMPONENTS CVODE REQUIRED)
include_directories (${SUNDIALS_INCLUDE_DIR})

find_package(MPI)
include_directories (${MPI_INCLUDE_PATH})

find_package(HDF5 COMPONENTS CXX REQUIRED)
include_directories (${HDF5_INCLUDE_DIRS})
link_directories (${HDF5_LIBRARY_DIRS})
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} hdf5.lib hdf5_cpp.lib")
  set(ZLIB_LIBRARY "${HDF5_DIR}/../../lib/zlib.lib")
  set(ZLIB_INCLUDE_DIR "${HDF5_DIR}/../../include")
  set(ZLIB_FOUND "1")
else()
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} -lhdf5 -lhdf5_cpp")
  find_package(ZLIB REQUIRED)
endif()

add_subdirectory (src) 
add_subdirectory (share) 

# Packaging ---------------------------------------------------------------------

include(InstallRequiredSystemLibraries)

set(jemris_DESCRIPTION_SUMMARY "jemris mri simulator")
set(jemris_VENDOR "jemris")
set(jemris_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/Readme.txt")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(jemris_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(jemris_EXECUTABLES "jemris" "jemris executable")

#set(CPACK_GENERATOR "DEB")
#set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Kaveh Vahedipour") #required

include(CPack)
