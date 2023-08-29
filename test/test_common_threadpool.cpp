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

#include "src/common/threadpool.h"

TEST(ThreadPool, multipleJobs) {
	std::atomic_int test(0), test2(0);
	for (int i = 0; i < 100; i++)
		Threads.add([&](){test++;});
	for (int i = 0; i < 120; i++)
		Threads.add([&](){test2++;});

	EXPECT_LE(Threads.getQueuedTasks(), 220);

	while (!Threads.empty())
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	EXPECT_EQ(test.load(), 100);
	EXPECT_EQ(test2.load(), 120);
}

TEST(ThreadPool, numThreads) {
	EXPECT_EQ(Threads.getNumWorkerThreads(), std::max<size_t>(std::thread::hardware_concurrency() - 1, 1));
}
