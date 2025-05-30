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

#ifndef OPENAWE_EXCEPTION_H
#define OPENAWE_EXCEPTION_H

#include <stdexcept>
#include <source_location>
#include <string_view>

#include <fmt/format.h>

#define CreateException(...) Common::Exception("{}:{}: {}", __FILE__, __LINE__, fmt::format(__VA_ARGS__))

namespace Common {

/*!
 * \brief Custom exception class with fmt support
 *
 * This class is a custom exception class to support several features, which
 */
class Exception : public std::exception {
public:
	template<typename FormatString, typename... Args>
	Exception(const FormatString &fmt, Args &&...args);

	const char *what() const noexcept override;

private:
	std::string _message;
};

template<typename FormatString, typename... Args>
Exception::Exception(const FormatString &fmt, Args &&... args) {
	_message = fmt::format(fmt::runtime(fmt), args...);
}

/*!
 * \brief Exception class which prepends the source location to the exception message
 *
 * This class is a custom exception class to support formatting with a prepended source location
 *
 * \tparam FormatString The type of the format string
 * \tparam Args The arguments for the formatting string
 */
template<typename FormatString, typename... Args>
class SourceException : public Exception {
public:
	explicit SourceException(const FormatString &fmt, Args &&...args,
							 const std::source_location &loc = std::source_location::current())
			: Exception(fmt::format("{}:{}: {}", loc.file_name(), loc.line(), fmt), args...) {
	}
};

template<typename FormatString, typename... Args>
SourceException(const FormatString &, Args &&...) -> SourceException<FormatString, Args...>;

} // End of namespace Common

#endif //OPENAWE_EXCEPTION_H
