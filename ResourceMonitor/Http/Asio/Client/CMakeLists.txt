file(GLOB HTTP_ASIO_CLIENT_SOURCES "*.cpp" "*.h")
add_library(HttpAsioClient STATIC ${HTTP_ASIO_CLIENT_SOURCES})

set_property(TARGET HttpAsioClient PROPERTY CXX_STANDARD 20)

target_include_directories(HttpAsioClient PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParties"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    "${BOOST_ROOT}"
)

target_link_directories(HttpAsioClient PUBLIC "${BOOST_LIB_DIR}")
target_link_libraries(HttpAsioClient PUBLIC RMUtils)