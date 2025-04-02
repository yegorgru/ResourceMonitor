file(GLOB CLIENT_SOURCES "*.cpp" "*.h")
add_executable (CommandLineClientBeast ${CLIENT_SOURCES})

set_property(TARGET CommandLineClientBeast PROPERTY CXX_STANDARD 20)

target_include_directories(CommandLineClientBeast PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../RMUtils"
    "${BOOST_ROOT}"
)

target_link_directories(CommandLineClientBeast PUBLIC "${BOOST_LIB_DIR}")
target_link_libraries(CommandLineClientBeast PUBLIC RMUtils)