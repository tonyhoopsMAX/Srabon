cmake_minimum_required(VERSION 3.24)
project(SrabonTsf LANGUAGES CXX)

if(NOT WIN32)
  message(FATAL_ERROR "Srabon.Tsf is a native Windows Text Services Framework server.")
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(Srabon.Tsf SHARED text_service.cpp converter.cpp Srabon.Tsf.def)
target_compile_definitions(Srabon.Tsf PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0A00)
target_link_libraries(Srabon.Tsf PRIVATE ole32 advapi32 uuid)
set_target_properties(Srabon.Tsf PROPERTIES OUTPUT_NAME "Srabon.Tsf")

add_executable(Srabon.Tsf.ConverterTest converter_test.cpp converter.cpp)
target_compile_definitions(Srabon.Tsf.ConverterTest PRIVATE UNICODE _UNICODE NOMINMAX)
enable_testing()
add_test(NAME Srabon.Tsf.ConverterRegression COMMAND Srabon.Tsf.ConverterTest)

add_executable(Srabon.Tsf.Activate activate_profile.cpp)
target_compile_definitions(Srabon.Tsf.Activate PRIVATE UNICODE _UNICODE WIN32_LEAN_AND_MEAN NOMINMAX _WIN32_WINNT=0x0A00)
target_link_libraries(Srabon.Tsf.Activate PRIVATE ole32 uuid)
