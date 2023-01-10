#pragma once

#include "person.hpp"

namespace dna{
const size_t telomer_len = 6;
const std::array<dna::base, telomer_len> telomer{dna::T, dna::T, dna::A, dna::G, dna::G, dna::G};

template<typename BinaryBuffer>
size_t find_telomer_left(BinaryBuffer& buf, size_t bufIdx, size_t telomerIdx, bool canMoveTelomerIdx = true) {
    if (telomerIdx == telomer_len){
        return bufIdx;
    }

    if (buf[bufIdx] == dna::C){
        return 0;
    }

    if (buf[bufIdx] != telomer[telomerIdx] && bufIdx == 0 && canMoveTelomerIdx) {
        return find_telomer_left(buf, bufIdx, telomerIdx + 1);
    } else if (buf[bufIdx] == telomer[telomerIdx]) {
        auto skip = find_telomer_left(buf, bufIdx + 1, telomerIdx + 1, canMoveTelomerIdx);
        if (skip) {
            return skip;
        } else if (bufIdx == 0 && canMoveTelomerIdx) {
            return find_telomer_left(buf, bufIdx, telomerIdx + 1, canMoveTelomerIdx);
        }
    } 

    return 0;
}

template<typename BinaryBuffer>
bool is_telomer(BinaryBuffer& buf, size_t pos) {
    for (size_t i = 0; i < telomer_len && pos + i < buf.size(); ++i) {
        if (buf[pos + i] != telomer[i]) {
            return false;
        }
    }

    return true;
}

template<HelixStream S>
size_t skip_telomer_left(S& stream) {
    size_t total_skip = 0;

    auto buf = stream.read();
    size_t buf_skip = find_telomer_left<decltype(buf)>(buf, 0, 0);
    size_t telomer_tail = 0;
    if (buf_skip) {
        for (;;) {
            if (telomer_tail && find_telomer_left(buf, 0, telomer_len - telomer_tail, false)) {
                buf_skip += telomer_len - telomer_tail;
            }
            telomer_tail = 0;

            while (buf_skip + telomer_len <= buf.size() && is_telomer(buf, buf_skip)) {
                buf_skip += telomer_len;
            } 

            if (buf_skip == 0) {
                break;
            }

            if (buf.size() - buf_skip < telomer_len && is_telomer(buf, buf_skip)) {
                telomer_tail = telomer_len - (buf.size() - buf_skip);
                total_skip += buf.size();
                buf_skip = 0;
            }

            if (buf_skip == 0) {
                buf = stream.read();
            } else {
                break;
            }
        }
    }

    return total_skip + buf_skip;
}
}
