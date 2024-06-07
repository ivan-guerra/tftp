cmake_minimum_required(VERSION 3.28)

# Set compile and link options.
set(COMMON_FLAGS -std=c++23)
set(WARNING_FLAGS -Wall -Wextra -Werror -pedantic)
set(DEBUG_FLAGS ${COMMON_FLAGS} ${WARNING_FLAGS} -O0 -g3 -ggdb)
set(RELEASE_FLAGS ${COMMON_FLAGS} -O2)

option(ENABLE_ASAN "enable ASAN support" ON)
if(ENABLE_ASAN)
  set(SANITIZER_FLAGS
      -fsanitize=address,undefined,float-divide-by-zero,nullability
      -fno-omit-frame-pointer -fno-sanitize-recover -fno-common)
  list(APPEND DEBUG_FLAGS ${SANITIZER_FLAGS})
  add_link_options("$<$<CONFIG:Debug>:${SANITIZER_FLAGS}>")
endif()

add_compile_options("$<$<CONFIG:Release>:${RELEASE_FLAGS}>"
                    "$<$<CONFIG:Debug>:${DEBUG_FLAGS}>")
