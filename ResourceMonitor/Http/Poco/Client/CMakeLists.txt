file(GLOB HTTP_POCO_CLIENT_SOURCES "*.cpp" "*.h")
add_library(HttpPocoClient STATIC ${HTTP_POCO_CLIENT_SOURCES})

set_property(TARGET HttpPocoClient PROPERTY CXX_STANDARD 20)

find_package(Poco REQUIRED COMPONENTS Net Util Foundation)

target_link_libraries(HttpPocoClient PUBLIC Poco::Net Poco::Util Poco::Foundation)

target_include_directories(HttpPocoClient PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParties"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    "${POCO_INCLUDE_DIRS}"
)

target_link_directories(HttpPocoClient PUBLIC "${POCO_LIB_DIR}")
target_compile_definitions(HttpPocoClient PUBLIC POCO_STATIC)
target_link_libraries(HttpPocoClient PUBLIC RMUtils)