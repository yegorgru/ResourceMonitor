file(GLOB HTTP_POCO_SERVER_SOURCES "*.cpp" "*.h")
add_library(HttpPocoServer STATIC ${HTTP_POCO_SERVER_SOURCES})

set_property(TARGET HttpPocoServer PROPERTY CXX_STANDARD 20)

find_package(Poco REQUIRED COMPONENTS Net Util Foundation)

target_link_libraries(HttpPocoServer PUBLIC Poco::Net Poco::Util Poco::Foundation)

target_include_directories(HttpPocoServer PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParties"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    "${POCO_INCLUDE_DIRS}"
)

target_link_directories(HttpPocoServer PUBLIC "${POCO_LIB_DIR}")
target_compile_definitions(HttpPocoServer PUBLIC POCO_STATIC)
target_link_libraries(HttpPocoServer PUBLIC RMUtils)