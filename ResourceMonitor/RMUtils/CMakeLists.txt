file(GLOB RMUTILS_SOURCES "*.cpp" "*.h")
add_library(RMUtils STATIC ${RMUTILS_SOURCES})

set_property(TARGET RMUtils PROPERTY CXX_STANDARD 20)

target_include_directories(RMUtils PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}" 
    "${CMAKE_CURRENT_SOURCE_DIR}/../ThirdParties"
    "${BOOST_ROOT}"
)

target_link_directories(RMUtils PUBLIC "${BOOST_LIB_DIR}")