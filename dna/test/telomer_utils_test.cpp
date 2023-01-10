#include "catch.hpp"
#include "fake_stream.hpp"
#include <telomer_utils.hpp>

TEST_CASE("No telomers found", "[telomer]") {
    int rawdata[] = {
        0x07, // AACT
        0x09  // AAGC
	};

	std::vector<std::byte> data(2);
	for (int i = 0; i < 2; i++)
		data[i] = static_cast<std::byte>(rawdata[i]);

    fake_stream stream(std::move(data), 2);
    auto skip = dna::skip_telomer_left(stream);
    REQUIRE(skip == 0);
}

TEST_CASE("Partial left telomers found", "[telomer]") {
    int rawdata[] = {
        0x2a, // AGGG - partial telomer
        0x00, 0x00, 0x00
	};

	std::vector<std::byte> data(4);
	for (int i = 0; i < 4; i++)
		data[i] = static_cast<std::byte>(rawdata[i]);
        
    fake_stream stream(std::move(data), 4);
    auto skip = dna::skip_telomer_left(stream); 
    REQUIRE(skip == 4);
}

TEST_CASE("Partial left telomers and full size telomers found", "[telomer]") {
    int rawdata[] = {
        0x2a, // AGGG - partial telomer
        0xf2, 0xaf, 0x2a, // 2x TTAGGG
        0x00, 0x00, 0x00
	};

	std::vector<std::byte> data(7);
	for (int i = 0; i < 7; i++)
		data[i] = static_cast<std::byte>(rawdata[i]);
        
    fake_stream stream(std::move(data), 4);
    auto skip = dna::skip_telomer_left(stream); 
    REQUIRE(skip == 16);
}

TEST_CASE("Telomers split between 2 buffers", "[telomer]") {
    int rawdata[] = {
        0xf2, 0xaf, 0x2a, // 2x TTAGGG
        0xf2, 0xaf, 0x2a, // 2x TTAGGG
        0x00, 0x00, 0x00
	};

	std::vector<std::byte> data(9);
	for (int i = 0; i < 9; i++)
		data[i] = static_cast<std::byte>(rawdata[i]);
        
    fake_stream stream(std::move(data), 4);
    auto skip = dna::skip_telomer_left(stream); 
    REQUIRE(skip == 20);
}