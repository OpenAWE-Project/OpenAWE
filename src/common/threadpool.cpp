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

#include <iostream>
#include <functional>
#include <fmt/format.h>

#include "src/common/threadpool.h"

namespace Common {

ThreadPool::ThreadPool() : _threads(std::max<int>(std::thread::hardware_concurrency() - 1, 1)) {
	_finished.store(false);

	unsigned int i = 0;
	for (auto &thread : _threads) {
		thread = std::thread(std::bind(&ThreadPool::run, this));

#ifdef OS_LINUX
		pthread_setname_np(
				thread.native_handle(),
				fmt::format("Worker Thread {}", i++).c_str()
		);
#endif
	}
}

ThreadPool::~ThreadPool() {
	_finished.store(true);
	_taskCond.notify_all();
	for (auto &thread : _threads) {
		if (thread.joinable())
			thread.join();
	}
}

void ThreadPool::add(Runnable runnable) {
	std::lock_guard<std::mutex> l(_taskAccess);
	_tasks.push(runnable);
}

bool ThreadPool::empty() const {
	std::lock_guard<std::mutex> l(_taskAccess);
	return _tasks.empty();
}

size_t ThreadPool::getQueuedTasks() const {
	std::lock_guard<std::mutex> l(_taskAccess);
	return _tasks.size();
}

size_t ThreadPool::getNumWorkerThreads() const {
	return _threads.size();
}

void ThreadPool::run() {
	while (!_finished) {
		_taskAccess.lock();
		if (_tasks.empty()) {
			_taskAccess.unlock();
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			continue;
		}

		Runnable runnable = _tasks.front();
		_tasks.pop();
		_taskAccess.unlock();

		runnable();
	}
}

}