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

#ifndef AWE_DPFILE_H
#define AWE_DPFILE_H

#include <memory>
#include <vector>

#include "src/common/readstream.h"

#include "src/awe/types.h"

/*!
 * \brief DP file reader for variaous data
 *
 * This class reads dp_ prefixed files which contains various
 * data associated with elements from the cid files.
 */
class DPFile {
public:
	struct ScriptMetadata {
		uint32_t offset;
		uint32_t name;
	};

	struct ScriptSignal {
		GID gid;
		uint32_t nameOffset;
	};

	struct ScriptDebugEntry {
		uint32_t id;
		uint32_t type;
		uint32_t nameOffset;
	};

	struct TaskData1 {
		uint32_t count, hash;
	};

	/*!
	 * Create a new dp file class by giving the dp file
	 * stream as parameter
	 *
	 * \param dp the dp file stream
	 */
	explicit DPFile(Common::ReadStream *dp);

	bool hasString(uint32_t offset);
	std::string getString(uint32_t offset);
	std::vector<uint32_t> getValues(uint32_t offset, unsigned int count);
	std::vector<glm::vec2> getPositions2D(uint32_t offset, unsigned int count);
	std::vector<ScriptMetadata> getScriptMetadata(uint32_t offset, unsigned int count);
	std::vector<ScriptSignal> getScriptSignals(uint32_t offset, unsigned int count);
	std::vector<ScriptDebugEntry> getScriptDebugEntries(uint32_t offset, unsigned int count);
	Common::ReadStream * getStream(uint32_t offset, unsigned int length);
	void readTaskData1(uint32_t offset, unsigned int count);

private:
	enum HeaderType {
		kHeaderV1,
		kHeaderV2,
		kHeaderV3
	};

	/**
	 * Test for the type of the header.
	 */
	void testHeader();

	HeaderType _headerType;

	uint32_t _dataSize;

	std::vector<uint32_t> _bytecodeOffsets;
	std::vector<uint32_t> _valueOffsets;
	std::vector<uint32_t> _stringOffsets;

	std::unique_ptr<Common::ReadStream> _dp;
};


#endif //AWE_DPFILE_H
