file(GLOB HTTP_ASIO_SOURCES "*.cpp" "*.h")
add_library(HttpAsio STATIC ${HTTP_ASIO_SOURCES})

set_property(TARGET HttpAsio PROPERTY CXX_STANDARD 20)

target_include_directories(HttpAsio PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParties"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    "${BOOST_ROOT}"
)

target_link_directories(HttpAsio PUBLIC "${BOOST_LIB_DIR}")

if(CLIENT_BOOST_ASIO)
    add_subdirectory ("Client")
    target_link_libraries(HttpAsio PUBLIC HttpAsioClient)
endif()
if(SERVER_BOOST_ASIO)
    add_subdirectory ("Server")
    target_link_libraries(HttpAsio PUBLIC HttpAsioServer)
endif()