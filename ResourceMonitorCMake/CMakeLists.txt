cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("ResourceMonitorCMake")

set(BOOST_ROOT "C:/local/boost_1_86_0" CACHE INTERNAL "Path to the Boost installation directory")
set(BOOST_LIB_DIR "C:/local/boost_1_86_0/lib64-msvc-14.3" CACHE INTERNAL "Path to the Boost library directory")

add_subdirectory ("Server")
add_subdirectory ("CommandLineClient")
add_subdirectory ("RMUtils")
