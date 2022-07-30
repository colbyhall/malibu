#pragma once

#include "array.hpp"

namespace core::containers {
	template <typename K, typename V>
	class HashMap {
		struct Bucket {
			K key;
			V value;
			i32 next;
		};
	public:
		constexpr HashMap() : m_buckets(), m_layout() {}
		NO_COPY(HashMap);

		void reserve(usize amount) {
			m_buckets.reserve(amount);
			m_layout.reserve(amount);
		}

		void insert(K&& key, V&& value) {
			m_buckets.push(Bucket {
				core::forward<K>(key),
				core::forward<V>(value),
				-1
			});

			refresh_layout();
		}

		void insert(const K& key, const V& value) {
			m_buckets.push(Bucket {
				key,
				value,
				-1
			});

			refresh_layout();
		}

		Option<V&> find(const K& key) {
			if (m_buckets.is_empty()) return Option<V&>{};

			const auto index = key_to_layout_index(key);
			const auto mapped = m_layout[index];
			if (mapped == -1) return Option<V&>{};

			Bucket& bucket = m_buckets[mapped];
			for (;;) {
				if (bucket.key == key) return bucket.value;
				if (bucket.next == -1) break;
				bucket = m_buckets[bucket.next];
			}
			return Option<V&>{};
		}

		Option<V const&> find(const K& key) const {
			if (m_buckets.is_empty()) return Option<V const&>{};

			const auto index = key_to_layout_index(key);
			const auto mapped = m_layout[index];
			if (mapped == -1) return Option<V const&>{};

			Bucket const& bucket = m_buckets[mapped];
			for (;;) {
				if (bucket.key == key) return bucket.value;
				if (bucket.next == -1) break;
				bucket = m_buckets[bucket.next];
			}
			return Option<V const&>{};
		}

	private:
		inline usize key_to_layout_index(const K& key) {
			const u64 the_hash = hash(key);
			return the_hash % m_buckets.len();
		}

		void refresh_layout() {
			// Reset the layout array to all be invalid
			m_layout.set_len(0); // FIXME: Clear
			for (usize i = 0; i < m_buckets.len(); ++i) {
				m_layout.push(-1);
			}

			// Layout buckets by hash(key) & buckets.len() and build tree if collision detected
			for (usize i = 0; i < m_buckets.len(); ++i) {
				auto& bucket = m_buckets[i];

				const auto layout_index = key_to_layout_index(bucket.key);
				bucket.next = -1;

				// Check what index lies in the layout array
				auto found = m_layout[layout_index];

				// If its invalid then simply set the bucket index
				if (found == -1) {
					m_layout[layout_index] = (i32)i;
				} else {
					// If its valid then descend the bucket tree until an empty spot is found
					auto& other = m_buckets[found];
					while (other.next != -1) other = m_buckets[(usize)other.next];
					other.next = (i32)i;
				}
			}
		}

		Array<Bucket> m_buckets;
		Array<i32> m_layout;
	};	
}

using core::containers::HashMap;