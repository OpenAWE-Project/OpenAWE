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

#ifndef COMMON_THREADPOOL_H
#define COMMON_THREADPOOL_H

#include <thread>
#include <atomic>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <functional>

#include "src/common/singleton.h"

namespace Common {

typedef std::function<void()> Runnable;

class ThreadPool : public Common::Singleton<ThreadPool> {
public:
	ThreadPool();
	~ThreadPool();

	void add(Runnable runnable);
	bool empty() const;
	size_t getQueuedTasks() const;
	size_t getNumWorkerThreads() const;

private:
	void run();

	std::atomic_bool _finished;
	std::condition_variable _taskCond;
	mutable std::mutex _taskAccess;
	std::queue<Runnable> _tasks;
	std::vector<std::thread> _threads;
};

}

#define Threads Common::ThreadPool::instance()

#endif // COMMON_THREADPOOL_H
