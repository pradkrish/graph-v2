include(FetchContent)
set(FETCHCONTENT_QUIET ON)

message(STATUS "Cloning External Project: docopt.cpp")

FetchContent_Declare(
    docopt
    GIT_REPOSITORY https://github.com/docopt/docopt.cpp.git
    GIT_TAG        v0.6.3
)

FetchContent_GetProperties(docopt)
if(NOT docopt_POPULATED)
  FetchContent_Populate(
    docopt
  )
endif()

set(DOCOPT_SOURCE_DIR "${docopt_SOURCE_DIR}")
set(DOCOPT_INCLUDE_DIR "${docopt_INCLUDE_DIR}")

FetchContent_MakeAvailable(docopt)