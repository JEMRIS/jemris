cmake_minimum_required (VERSION 2.8) 

project (jemris) 
set (jemris_VERSION_MAJOR 2)
set (jemris_VERSION_MINOR 9)
set (jemris_VERSION_PATCH 1)
set (jemris_VERSION "${jemris_VERSION_MAJOR}.${jemris_VERSION_MINOR}.${jemris_VERSION_PATCH}")

set(CMAKE_BUILD_TYPE "Release")

set (CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

macro(set_config_option VARNAME STRING)
  set(${VARNAME} TRUE)
  list(APPEND CONFIG_OPTIONS ${STRING})
  message(STATUS "Found " ${STRING})
endmacro(set_config_option)

include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1)

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
# include (VcMacros) DB: not needed, right?
include (OptimizeForArchitecture)
#find_package (OpenMP)
#if(OPENMP_FOUND)
  #message("-- Found OpenMP")
  #set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  #set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif()

# C++ flags ---------------------------------------------------------------------
# THis should be highly discouraged as it directly sets the compiler flags - this should be handled by cmake to allow
# proper RELEASE, DEBUG, ... builds
set(CMAKE_CXX_COMPILER "mpicxx")
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-psabi -DTIXML_USE_STL -fPIC -O3") 
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DTIXML_USE_STL -O3 -Itr1 -std=c++0x -stdlib=libc++")
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

include (VcMacros)
include (OptimizeForArchitecture)

check_type_size("void*" SIZEOF_VOID_P)
if(SIZEOF_VOID_P EQUAL 8)
  set_config_option(HAVE_64BIT_SIZE_T "Have64BitSizeT")
endif(SIZEOF_VOID_P EQUAL 8)

find_package(Boost)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
  set (HAVE_BOOST ON CACHE BOOL "Found boost")
endif()

find_package(Xerces 1.3.0 REQUIRED)
include_directories (${Xerces_INCLUDE_DIRS})

find_package(GiNaC REQUIRED)
include_directories (${GINAC_INCLUDE_DIRS})

# Hmm does not support components...
find_package(Sundials REQUIRED)
include_directories (${SUNDIALS_INCLUDE_DIR})
# Handle Nvector stuff... here we need to deal with the actual version
if (SUNDIALS_FOUND)
   add_definitions(-DHAVE_CVODE_CVODE_H)
   add_definitions(-DHAVE_NVECTOR_NVECTOR_SERIAL_H)
   add_definitions(-DHAVE_CVODE_CVODE_DIAG_H)
endif(SUNDIALS_FOUND)

find_package(MPI)
if(MPI_C_FOUND)
  include_directories (${MPI_INCLUDE_PATH})
  set (HAVE_MPI_THREADS ON CACHE BOOL "Found MPI with threads")
elseif(MPI_C_FOUND)
    set(MPIRUN "MPIRUN-UNAVAILABLE")
endif()


find_package(HDF5 COMPONENTS C CXX REQUIRED)

if (NOT DEFINED HDF5_CXX_LIBRARIES)
   MESSAGE(STATUS "HDF5 CXX Libraries not set but library present... using alternative! Probably everything is fine!")
   LIST(APPEND HDF5_CXX_LIBRARIES ${HDF5_hdf5_cpp_LIBRARY})
   LIST(APPEND HDF5_CXX_LIBRARIES ${HDF5_hdf5_LIBRARY})
   LIST(APPEND HDF5_CXX_LIBRARIES ${HDF5_z_LIBRARY})
   LIST(APPEND HDF5_CXX_LIBRARIES ${HDF5_sz_LIBRARY})
   LIST(APPEND HDF5_CXX_LIBRARIES ${HDF5_m_LIBRARY})
   LIST(APPEND HDF5_CXX_LIBRARIES ${HDF5_hdf5_hl_LIBRARY})
endif(NOT DEFINED HDF5_CXX_LIBRARIES)

include_directories (${HDF5_INCLUDE_DIR})

# ISMRMRD
find_package(ISMRMRD REQUIRED)
link_directories(${ISMRMRD_LIBRARY_DIRS})
include_directories(${ISMRMRD_INCLUDE_DIRS})

# Docker image
#message( NOTICE "pulling docker image for reconstruction server...")
#execute_process(COMMAND "docker" "pull" "mavel101/bart-reco-server" RESULT_VARIABLE ret OUTPUT_FILE CMD_OUTPUT)
#if(ret EQUAL "1")
    #message( WARNING "Docker image for reconstruction server cannot be pulled. Docker may not be installed.")
#else()
    #message( NOTICE "Succesfully pulled Docker image.")
#endif()

# Client conda environment
if ( NOT DEFINED SKIP_CONDA)
  message( NOTICE "creating conda environment for reconstruction client.")
  configure_file(${CMAKE_SOURCE_DIR}/ismrmrd_client.yml ${CMAKE_CURRENT_BINARY_DIR}/ismrmrd_client.yml)
  execute_process(COMMAND "conda" "list" "-n" "ismrmrd_client" RESULT_VARIABLE ret OUTPUT_QUIET ERROR_QUIET)
  if(ret EQUAL "1")
    execute_process(COMMAND "conda" "env" "create" "-f" "ismrmrd_client.yml" RESULT_VARIABLE ret)
    if(ret EQUAL "1")
        message( WARNING "Conda environment for reconstruction client cannot be installed. Python may not be installed.")
    else()
        message( NOTICE "Succesfully installed conda environment.")
    endif()
  else()
        message( NOTICE "Conda environment already installed. Environment will be updated if necessary.")
        execute_process(COMMAND "conda" "run" "-n" "ismrmrd_client" "conda" "env" "update" "--file" "ismrmrd_client.yml" RESULT_VARIABLE ret)
  endif()
else()
  message( NOTICE "Skipping creation of conda environment.")
endif()

# Is this here really necessary?
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_LINK_FLAGS "${CMAKE_LINK_FLAGS} hdf5.lib hdf5_cpp.lib")
  set(ZLIB_LIBRARY "${HDF5_DIR}/../../lib/zlib.lib")
  set(ZLIB_INCLUDE_DIR "${HDF5_DIR}/../../include")
  set(ZLIB_FOUND "1")
endif()

SET(prefix "${CMAKE_INSTALL_PREFIX}")

configure_file (
        "cmake/config.h.in"
  "${PROJECT_SOURCE_DIR}/src/config.h"
        @ONLY
  )



add_subdirectory (src) 
add_subdirectory (share) 

# Packaging ---------------------------------------------------------------------

# All
include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set (CPACK_GENERATOR "TBZ2")
set (CPACK_PACKAGE_VERSION "${jemris_VERSION_MAJOR}.${jemris_VERSION_MINOR}.${jemris_VERSION_PATCH}")
set (CPACK_PACKAGE_NAME "jemris")
set (CPACK_PACKAGE_CONTACT "Tony Stoecker <tony.stoecker@dzne.de>")
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY "JEMRIS is a general purpose MRI simulator. Visit http://www.jemris.org for more details.")

# DEB
set (CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set (CPACK_DEBIAN_PACKAGE_DEPENDS "ginac-tools, openmpi-bin, libsundials-cvode4, libxerces-c3.2, libhdf5-cpp-103")

include (CPack)
