file(GLOB SERVER_SOURCES "*.cpp" "*.h")
add_executable (Server ${SERVER_SOURCES})

set_property(TARGET Server PROPERTY CXX_STANDARD 20)

target_include_directories(Server PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../RMUtils"
    "${BOOST_ROOT}"
)

target_link_directories(Server PUBLIC "${BOOST_LIB_DIR}")
target_link_libraries(Server PUBLIC RMUtils)