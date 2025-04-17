file(GLOB HTTP_BEAST_SOURCES "*.cpp" "*.h")
add_library(HttpBeast STATIC ${HTTP_BEAST_SOURCES})

set_property(TARGET HttpBeast PROPERTY CXX_STANDARD 20)

target_include_directories(HttpBeast PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../ThirdParties"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    "${BOOST_ROOT}"
)

target_link_directories(HttpBeast PUBLIC "${BOOST_LIB_DIR}")

add_subdirectory ("Client")
target_link_libraries(HttpBeast PUBLIC HttpBeastClient)
add_subdirectory ("Server")
target_link_libraries(HttpBeast PUBLIC HttpBeastServer)