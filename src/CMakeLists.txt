add_definitions(-Wextra -Wall -std=c++11 -g)

aux_source_directory(. dotSources)

add_executable(sptr_test ${dotSources})
