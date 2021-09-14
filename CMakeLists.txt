cmake_minimum_required(VERSION 3.20)


project(scss)


file(GLOB sources src/*.c)

add_library(scss SHARED ${sources})

include_directories(src/include)

include(FetchContent)
function (use_lib LIBNAME URL TAG)
  FetchContent_Declare(
    ${LIBNAME}
    GIT_REPOSITORY ${URL}
    GIT_TAG ${TAG}
  )
  FetchContent_MakeAvailable(${LIBNAME})

  set(varname "${LIBNAME}_SOURCE_DIR")
  make_directory(${CMAKE_BINARY_DIR}/_deps/include/${LIBNAME})
  file(GLOB g ${${varname}}/src/include/*)
  file(COPY ${g}  DESTINATION ${CMAKE_BINARY_DIR}/_deps/include/${LIBNAME})
  include_directories(${CMAKE_BINARY_DIR}/_deps/include)
endfunction (use_lib)


use_lib(list https://github.com/sebbekarlsson/liblist.git master)


target_link_libraries(scss list)
