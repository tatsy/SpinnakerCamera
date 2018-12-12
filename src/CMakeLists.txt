set(BUILD_TARGET example01)

file(GLOB SOURCE_FILES "*.cpp" "*.h")

include_directories(${SPINNAKER_INCLUDE_DIRS})
add_executable(${BUILD_TARGET} ${SOURCE_FILES})
target_link_libraries(${BUILD_TARGET} ${SPINNAKER_LIBRARIES})

source_group("Source Files" FILES ${SOURCE_FILES})

if (MSVC)
    set(CMAKE_CXX_FLAGS "/Zi /EHsc")
    set_property(TARGET ${BUILD_TARGET} APPEND PROPERTY "/DEBUG /PROFILE")
endif()
