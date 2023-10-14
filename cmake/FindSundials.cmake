# From https://github.com/OpenCMISS/cm
find_path (SUNDIALS_DIR include/sundials/sundials_config.h HINTS ENV SUNDIALS_DIR PATHS $ENV{HOME}/sundials DOC "Sundials Directory")

IF(EXISTS ${SUNDIALS_DIR}/include/sundials/sundials_config.h)
  SET(SUNDIALS_FOUND YES)
  SET(SUNDIALS_INCLUDES ${SUNDIALS_DIR})
  find_path (SUNDIALS_INCLUDE_DIR sundials_config.h HINTS "${SUNDIALS_DIR}" PATH_SUFFIXES include/sundials NO_DEFAULT_PATH)
  list(APPEND SUNDIALS_INCLUDES ${SUNDIALS_INCLUDE_DIR})
  find_library(SUNDIALS_LIBRARIES_CVODE "sundials_cvode" DOC "sundials libraries cvode")
  find_library(SUNDIALS_LIBRARIES_NVEC "sundials_nvecserial" DOC "sundials libraries nvec")
  list(APPEND SUNDIALS_LIBRARIES ${SUNDIALS_LIBRARIES_CVODE} ${SUNDIALS_LIBRARIES_NVEC})
  mark_as_advanced(SUNDIALS_LIBRARIES)
  # DB did not work properly for me
  #FILE(GLOB SUNDIALS_LIBRARIES RELATIVE "${SUNDIALS_DIR}/lib" "${SUNDIALS_DIR}/lib/libsundials*.a")
ELSE(EXISTS ${SUNDIALS_DIR}/include/sundials/sundials_config.h)
  SET(SUNDIALS_FOUND NO)
  message(FATAL_ERROR "Cannot find SUNDIALS!")
ENDIF(EXISTS ${SUNDIALS_DIR}/include/sundials/sundials_config.h)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Sundials
                                  FOUND_VAR SUNDIALS_FOUND
                                  REQUIRED_VARS SUNDIALS_LIBRARIES
                                                SUNDIALS_INCLUDE_DIR
                                  FAIL_MESSAGE "Failed to find Sundials")
