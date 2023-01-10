#pragma once

#include "base.hpp"
#include "person.hpp"
#include "telomer_utils.hpp"
#include <set>
#include <vector>

namespace dna {

struct interesting_distinction{
	size_t size{0};
	size_t idx1{0};
	size_t idx2{0};

};

bool operator<(const interesting_distinction& d1, const interesting_distinction& d2) {
    return (d1.idx1 < d2.idx1) ||
        (d1.idx1 == d2.idx1 && d1.idx2 < d2.idx2);
}

template <HelixStream S>
size_t find_first_distinct_char(S& s1, S& s2) {
    size_t pos = 0;
    auto b1 = s1.read();
    auto b2 = s2.read();

    for (size_t start = 0; start == 0 && b1.size() && b2.size(); ) {
        for (; start < b1.size() && start < b2.size() && b1[start] == b2[start]; ++start);
        
        if (start == b1.size() || start == b2.size()) {
            // the whole buffer is the same for both people, 
            // move to the next one
            pos += std::min(b1.size(), b2.size());
            start = 0;
            b1 = s1.read();
            b2 = s2.read();
        } else {
            pos += start;
        }
    }

    return pos;
}

template <HelixStream S>
size_t find_first_same_char(S& s1, S& s2) {
    size_t pos = 0;
    auto b1 = s1.read();
    auto b2 = s2.read();

    for (size_t start = 0; start == 0 && b1.size() && b2.size(); ) {
        for (; start < b1.size() && start < b2.size() && b1[start] != b2[start]; ++start);
        
        if (start == b1.size() || start == b2.size()) {
            // the whole buffer is different, 
            // move to the next one
            pos += std::min(b1.size(), b2.size());
            start = 0;
            b1 = s1.read();
            b2 = s2.read();
        } else {
            pos += start;
        }
    }

    return pos;
}

template<Person P>
std::set<interesting_distinction> compare(P& person1, P& person2) {
    std::set<interesting_distinction> res;

    // TODO run in thread
    // TODO special logic for chromosome 23
    for (int chromosome = 0; chromosome < 23; ++chromosome) {
        // skip telemeres at the beginning
        auto stream1 = person1.chromosome(chromosome);
        auto idx1 = skip_telomer_left(stream1);
        stream1.seek(idx1);

        auto stream2 = person2.chromosome(chromosome);
        auto idx2 = skip_telomer_left(stream2);
        stream2.seek(idx2);

        auto start = find_first_distinct_char(stream1, stream2);
        stream1.seek(idx1 + start);
        stream2.seek(idx2 + start);

        auto end = find_first_same_char(stream1, stream2);

        // TODO: skip telomeres on right

        if (end != start) {
            interesting_distinction d{.size = end - start, .idx1 = idx1 + start, .idx2 = idx2 + start};
            res.insert(d);
        }
    }

    return res;
}

template<Person P>
std::set<interesting_distinction> compare(size_t person1idx, size_t size, std::vector<P>& people){
    std::set<interesting_distinction> res;

    return res;

}
}
