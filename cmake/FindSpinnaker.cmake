include(FindPackageHandleStandardArgs)

set(SPINNAKER_DIR "SPINNAKER_DIR" CACHE PATH "SPINNAKER_DIR")

if (WIN32)
    find_path(SPINNAKER_INCLUDE_DIR
              NAMES Spinnaker.h
              PATHS
              ${SPINNAKER_DIR}/include)

    if (CMAKE_GENERATOR MATCHES "(Win64|IA64)")
        set(LIBDIR_POSTFIX "64")
    endif()

    find_library(SPINNAKER_LIBRARY
                 NAMES Spinnaker_v140
                 PATHS
                 ${SPINNAKER_DIR}/lib${LIBDIR_POSTFIX}/vs2015)
else()
    message(FATAL_ERROR "Non Windows platform is currently not supported!")
endif()

find_package_handle_standard_args(
    Spinnaker
    DEFAULT_MSG
    SPINNAKER_INCLUDE_DIR
    SPINNAKER_LIBRARY
)

if (SPINNAKER_FOUND)
    message(STATUS "Spinnaker include: ${SPINNAKER_INCLUDE_DIR}")
    message(STATUS "Spinnaker library: ${SPINNAKER_LIBRARY}")
    set(SPINNAKER_INCLUDE_DIRS "${SPINNAKER_INCLUDE_DIR}")
    set(SPINNAKER_LIBRARIES "${SPINNAKER_LIBRARY}")
endif()
