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

#include <cstring>

#include <chrono>
#include <random>

#include "src/awe/script/functions.h"

namespace AWE::Script {

void Functions::getRand01(Functions::Context &ctx) {
    std::uniform_real_distribution<float> distribution(0.0, 1.0);
    std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());

    uint32_t value;
    float fValue = distribution(generator);
    std::memcpy(&value, &fValue, 4);
    ctx.ret = value;
}

void Functions::getRand(Functions::Context &ctx) {
    float upperBound = ctx.getFloat(0);
    float lowerBound = ctx.getFloat(1);

    std::uniform_real_distribution<float> distribution(lowerBound, upperBound);
    std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());

    uint32_t value;
    float fValue = distribution(generator);
    std::memcpy(&value, &fValue, 4);

    ctx.ret = value;
}

void Functions::getRandInt(Functions::Context &ctx) {
    uint32_t upperBound = ctx.getInt(0);
    uint32_t lowerBound = ctx.getInt(1);

    std::uniform_int_distribution<uint32_t> distribution(lowerBound, upperBound);
    std::mt19937 generator(std::chrono::system_clock::now().time_since_epoch().count());

    ctx.ret = distribution(generator);
}

}
