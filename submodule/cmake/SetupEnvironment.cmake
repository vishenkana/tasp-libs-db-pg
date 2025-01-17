message(STATUS "Generator: ${CMAKE_GENERATOR}")

if("${OS_ID}" STREQUAL "astra" AND "${OS_VERSION_ID}" STREQUAL "1.6")
    if(NOT CMAKE_CXX_COMPILER)
        if(EXISTS "/usr/bin/clang++-9")
            set(CMAKE_CXX_COMPILER "/usr/bin/clang++-9")
        else()
            message(FATAL_ERROR "Not found C++ compiler clang++-9 (Clang 9.0).")
        endif()
    endif(NOT CMAKE_CXX_COMPILER)

    if("${CMAKE_CXX_COMPILER}" MATCHES ".*clang.*")
        if(EXISTS "/usr/lib/gcc-mozilla")
            set(CMAKE_CXX_COMPILER_EXTERNAL_TOOLCHAIN "/usr/lib/gcc-mozilla")
        else()
            message(FATAL_ERROR "Not found toolchain gcc-mozilla (GCC 7.5.0).")
        endif()
    endif()
endif()

if("${OS_ID}" STREQUAL "astra" AND "${OS_VERSION_ID}" STREQUAL "1.7_x86-64")
    if(NOT CMAKE_CXX_COMPILER)
        if(EXISTS "/usr/lib/gcc-astra/bin/g++")
        set(CMAKE_CXX_COMPILER "/usr/lib/gcc-astra/bin/g++")
        else()
            message(FATAL_ERROR "Not found C++ compiler gcc-10.")
        endif()
    endif(NOT CMAKE_CXX_COMPILER)
endif()

if(NOT CMAKE_BUILD_TYPE)
   set(CMAKE_BUILD_TYPE Release CACHE STRING "" FORCE)
endif()
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
