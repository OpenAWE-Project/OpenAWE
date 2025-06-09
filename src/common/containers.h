/* OpenAWE - A reimplementation of Remedys Alan Wake Engine
 *
 * OpenAWE is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * OpenAWE is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * OpenAWE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenAWE. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OPENAWE_CONTAINERS_H
#define OPENAWE_CONTAINERS_H

#include <memory>
#include <vector>
#include <set>
#include <deque>
#include <stack>
#include <map>

#include <tracy/Tracy.hpp>

namespace Common {

template<typename T>
struct Allocator {
	typedef T value_type;

	Allocator() = default;

	[[nodiscard]] T* allocate(std::size_t n) {
		T* ptr = reinterpret_cast<T*>(std::malloc(n * sizeof(T)));
		TracyAlloc(ptr, n);
		return ptr;
	}

	void deallocate(T* ptr, std::size_t) {
		std::free(ptr);
		TracyFree(ptr);
	}
};

template<typename T>
using Vector = std::vector<T, Allocator<T>>;

template<typename Key, typename T, typename Compare = std::less<Key>>
using Map = std::map<Key, T, Compare, Allocator<std::pair<const Key, T>>>;

template<typename T>
using Deque = std::deque<T, Allocator<T>>;

template<typename T>
using Stack = std::stack<T, Deque<T>>;

template<typename Key, typename Compare = std::less<Key>>
using Set = std::set<Key, Compare, Allocator<Key>>;

}

#endif //OPENAWE_CONTAINERS_H
