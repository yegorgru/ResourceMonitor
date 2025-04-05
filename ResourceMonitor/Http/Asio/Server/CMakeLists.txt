file(GLOB HTTP_ASIO_SERVER_SOURCES "*.cpp" "*.h")
add_library(HttpAsioServer STATIC ${HTTP_ASIO_SERVER_SOURCES})

set_property(TARGET HttpAsioServer PROPERTY CXX_STANDARD 20)

target_include_directories(HttpAsioServer PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParties"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    "${BOOST_ROOT}"
)

target_link_directories(HttpAsioServer PUBLIC "${BOOST_LIB_DIR}")
target_link_libraries(HttpAsioServer PUBLIC RMUtils)