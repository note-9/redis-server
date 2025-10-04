#pragma once

#include <stdint.h>
#include <stddef.h>


// intrusive data structure
#include <cstddef>
#include <type_traits>

template <typename Container, typename Member>
Container* container_of(Member* ptr, Member Container::*member) {
    static_assert(std::is_standard_layout<Container>::value, "Container must be standard layout");
    const char* member_addr = reinterpret_cast<const char*>(&(reinterpret_cast<Container*>(0)->*member));
    return reinterpret_cast<Container*>(reinterpret_cast<char*>(ptr) - (member_addr - reinterpret_cast<const char*>(0)));
}

// FNV hash
inline uint64_t str_hash(const uint8_t *data, size_t len) {
    uint32_t h = 0x811C9DC5;
    for (size_t i = 0; i < len; i++) {
        h = (h + data[i]) * 0x01000193;
    }
    return h;
}
