file(GLOB HTTP_BEAST_SERVER_SOURCES "*.cpp" "*.h")
add_library(HttpBeastServer STATIC ${HTTP_BEAST_SERVER_SOURCES})

set_property(TARGET HttpBeastServer PROPERTY CXX_STANDARD 20)

target_include_directories(HttpBeastServer PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParties"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    "${BOOST_ROOT}"
)

target_link_directories(HttpBeastServer PUBLIC "${BOOST_LIB_DIR}")
target_link_libraries(HttpBeastServer PUBLIC RMUtils)