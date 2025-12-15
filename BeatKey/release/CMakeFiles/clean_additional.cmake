# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Release")
  file(REMOVE_RECURSE
  "BeatKey_autogen"
  "CMakeFiles\\BeatKey_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\BeatKey_autogen.dir\\ParseCache.txt"
  )
endif()
