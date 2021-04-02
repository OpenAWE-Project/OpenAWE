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

#ifndef SRC_COMMON_SINGLETON_H
#define SRC_COMMON_SINGLETON_H

#include "src/common/types.h"

namespace Common {

template<typename T> class Singleton : Common::Noncopyable {
public:
	static T &instance();

	Singleton() {};
	Singleton(const Singleton &s) = delete;
	virtual ~Singleton();

};

template<typename T> T &Singleton<T>::instance() {
	static T t;
	return t;
}

template<typename T>
Singleton<T>::~Singleton() {

}

}

#endif //SRC_COMMON_SINGLETON_H
