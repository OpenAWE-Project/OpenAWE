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

#include <string>

#include <gtest/gtest.h>

#include "src/awe/path.h"

TEST(Path, getNormalizedPath) {
    std::string testString = "this/is_correct/path";

    std::string testResult = AWE::getNormalizedPath("this\\is_correct\\path");
    EXPECT_STREQ(testString.c_str(), testResult.c_str());
    testResult = AWE::getNormalizedPath("This/is_CORRECT/paTh");
    EXPECT_STREQ(testString.c_str(), testResult.c_str());
    testResult = AWE::getNormalizedPath("runtimedata\\pc\\data\\this\\is_correct\\path");
    EXPECT_STREQ(testString.c_str(), testResult.c_str());
    testResult = AWE::getNormalizedPath("d:/data/this/is_correct/path");
    EXPECT_STREQ(testString.c_str(), testResult.c_str());
}
