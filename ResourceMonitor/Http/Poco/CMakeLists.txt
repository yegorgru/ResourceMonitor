file(GLOB HTTP_POCO_SOURCES "*.cpp" "*.h")
add_library(HttpPoco STATIC ${HTTP_POCO_SOURCES})

set_property(TARGET HttpPoco PROPERTY CXX_STANDARD 20)

target_include_directories(HttpPoco PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParties"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    "${POCO_INCLUDE_DIRS}"
)

target_link_directories(HttpPoco PUBLIC "${POCO_LIB_DIR}")
target_compile_definitions(HttpPoco PUBLIC POCO_STATIC)

add_subdirectory ("Client")
target_link_libraries(HttpPoco PUBLIC HttpPocoClient)
add_subdirectory ("Server")
target_link_libraries(HttpPoco PUBLIC HttpPocoServer)