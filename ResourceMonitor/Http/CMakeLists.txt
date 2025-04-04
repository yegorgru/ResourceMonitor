file(GLOB HTTP_SOURCES "*.cpp" "*.h")
add_library(Http STATIC ${HTTP_SOURCES})

set_property(TARGET Http PROPERTY CXX_STANDARD 20)

target_include_directories(Http PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
)

option(USE_BOOST_ASIO "Use Boost.Asio for networking" ON)

if(USE_BOOST_ASIO)
    add_subdirectory(Asio)
    target_link_libraries(Http PUBLIC HttpAsio)
    target_compile_definitions(Http PUBLIC USE_BOOST_ASIO)
else()
    add_subdirectory(Beast)
    target_link_libraries(Http PUBLIC HttpBeast)
endif()