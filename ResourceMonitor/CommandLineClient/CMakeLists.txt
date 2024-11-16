file(GLOB CLIENT_SOURCES "*.cpp" "*.h")
add_executable (CommandLineClient ${CLIENT_SOURCES})

set_property(TARGET CommandLineClient PROPERTY CXX_STANDARD 20)

target_include_directories(CommandLineClient PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../RMUtils"
    "${BOOST_ROOT}"
)

target_link_directories(CommandLineClient PUBLIC "${BOOST_LIB_DIR}")
target_link_libraries(CommandLineClient PUBLIC RMUtils)