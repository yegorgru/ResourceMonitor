file(GLOB HTTP_SOURCES "*.cpp" "*.h")
add_library(Http STATIC ${HTTP_SOURCES})

set_property(TARGET Http PROPERTY CXX_STANDARD 20)

target_include_directories(Http PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}../ThirdParties"
)

option(CLIENT_BOOST_ASIO "Use Boost.Asio for client-side networking" OFF)
option(SERVER_BOOST_ASIO "Use Boost.Asio for server-side networking" OFF)

option(CLIENT_BOOST_BEAST "Use Boost.Beast for client-side networking" ON)
option(SERVER_BOOST_BEAST "Use Boost.Beast for server-side networking" ON)

if(CLIENT_BOOST_ASIO OR SERVER_BOOST_ASIO)
    add_subdirectory(Asio)
    target_link_libraries(Http PUBLIC HttpAsio)
endif()

if(CLIENT_BOOST_BEAST OR SERVER_BOOST_BEAST)
    add_subdirectory(Beast)
    target_link_libraries(Http PUBLIC HttpBeast)
endif()

if(CLIENT_BOOST_ASIO)
    target_compile_definitions(Http PUBLIC CLIENT_BOOST_ASIO)
endif()
if(SERVER_BOOST_ASIO)
    target_compile_definitions(Http PUBLIC SERVER_BOOST_ASIO)
endif()
if(CLIENT_BOOST_BEAST)
    target_compile_definitions(Http PUBLIC CLIENT_BOOST_BEAST)
endif()
if(SERVER_BOOST_BEAST)
    target_compile_definitions(Http PUBLIC SERVER_BOOST_BEAST)
endif()