

set(TESTS
		fake_stream.cpp
		fake_stream_test.cpp
		person_utils_test.cpp
		sequence_buffer_test.cpp
		telomer_utils_test.cpp
)

add_executable(dna_test ${TESTS} main.cpp)
target_link_libraries(dna_test cogdna)
