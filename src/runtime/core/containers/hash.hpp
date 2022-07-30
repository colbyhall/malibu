#pragma once

#include "slice.hpp"

namespace core::hash {
	const u64 FNV_offset_basic = 0xcbf29ce484222325;
	const u64 FNV_prime = 0x100000001b3;

	inline u64 fnv1_hash(Slice<u8 const> bytes) {
		u64 hash = FNV_offset_basic;
		for (usize i = 0; i < bytes.len(); i++) {
			hash *= FNV_prime;
			hash = hash ^ bytes[i];
		}
		return hash;
	}
}