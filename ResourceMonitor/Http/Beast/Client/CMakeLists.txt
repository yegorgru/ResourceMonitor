file(GLOB HTTP_BEAST_CLIENT_SOURCES "*.cpp" "*.h")
add_library(HttpBeastClient STATIC ${HTTP_BEAST_CLIENT_SOURCES})

set_property(TARGET HttpBeastClient PROPERTY CXX_STANDARD 20)

target_include_directories(HttpBeastClient PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../ThirdParties"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}/../"
    "${CMAKE_CURRENT_SOURCE_DIR}/../../"
    "${BOOST_ROOT}"
)

target_link_directories(HttpBeastClient PUBLIC "${BOOST_LIB_DIR}")
target_link_libraries(HttpBeastClient PUBLIC RMUtils)