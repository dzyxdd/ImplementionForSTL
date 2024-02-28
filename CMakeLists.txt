cmake_minimum_required (VERSION 3.27)

# 如果支持，请为 MSVC 编译器启用热重载。
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_STANDARD 26)

project (ImplementionForSTL LANGUAGES CXX)

file(GLOB sources CONFIGURE_DEPENDS *.cpp)
foreach (source IN ITEMS ${sources})
    get_filename_component(name "${source}" NAME_WLE)
    add_executable(${name} ${name}.cpp)
endforeach()

if(MSVC)
    target_compile_options(${name} PUBLIC "/Zc:__cplusplus")
endif()
