file(GLOB SERVER_SOURCES "*.cpp" "*.h")
add_executable (Server ${SERVER_SOURCES})

set_property(TARGET Server PROPERTY CXX_STANDARD 20)

target_include_directories(Server PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../SingleInclude"
    "${CMAKE_CURRENT_SOURCE_DIR}/../RMUtils"
    "C:/local/boost_1_86_0"
)

target_link_directories(Server PUBLIC "C:/local/boost_1_86_0/lib64-msvc-14.3")
target_link_libraries(Server PUBLIC RMUtils)