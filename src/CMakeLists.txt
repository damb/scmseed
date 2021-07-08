set(MSEED_TARGET scmseed)

set(MSEED_SOURCES
  app.cpp
  main.cpp
)

if ("${CMAKE_BUILD_TYPE}" EQUAL "DEBUG")
  add_compile_options(-fsanitize=address)
  add_link_options(-fsanitize=address)
endif()

sc_add_executable(MSEED ${MSEED_TARGET})
sc_link_libraries_internal(${MSEED_TARGET} core client)
