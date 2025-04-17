file(GLOB HTTP_SOURCES "*.cpp" "*.h")
add_library(Http STATIC ${HTTP_SOURCES})

set_property(TARGET Http PROPERTY CXX_STANDARD 20)

if(USE_ASIO)
    message(STATUS "Using Boost.Asio implementation")
elseif(USE_BEAST)
    message(STATUS "Using Boost.Beast implementation")
elseif(USE_POCO)
    message(STATUS "Using POCO implementation")
else()
    message(FATAL_ERROR "No networking implementation selected. Please enable one of: USE_ASIO, USE_BEAST, or USE_POCO")
endif()

target_include_directories(Http PUBLIC 
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${CMAKE_CURRENT_SOURCE_DIR}../RMUtils"
    "${CMAKE_CURRENT_SOURCE_DIR}../ThirdParties"
)

target_link_libraries(Http PUBLIC RMUtils)

if(USE_ASIO)
    add_subdirectory(Asio)
    target_compile_definitions(Http PUBLIC USE_ASIO)
    target_link_libraries(Http PRIVATE HttpAsio)
elseif(USE_BEAST)
    add_subdirectory(Beast)
    target_compile_definitions(Http PUBLIC USE_BEAST)
    target_link_libraries(Http PRIVATE HttpBeast)
elseif(USE_POCO)
    add_subdirectory(Poco)
    target_compile_definitions(Http PUBLIC USE_POCO)
    target_link_libraries(Http PRIVATE HttpPoco)
endif()