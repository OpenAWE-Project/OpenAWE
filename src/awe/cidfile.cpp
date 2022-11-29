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

#include <memory>
#include <stdexcept>
#include <zlib.h>

#include "src/common/strutil.h"
#include "src/common/exception.h"

#include "src/awe/cidfile.h"
#include "src/awe/objectbinaryreadstreamv1.h"
#include "src/awe/objectbinaryreadstreamv2.h"

static const uint32_t kDeadBeef   = 0xDEADBEEF;
static const uint32_t kDeadBeefV2 = 0xD34DB33F;

namespace AWE {

CIDFile::CIDFile(Common::ReadStream &cid, ObjectType type, std::shared_ptr<DPFile> dp) : _dp(dp) {
	_version = cid.readUint32LE();
	uint32_t contentType = cid.readUint32LE(); // ?
	uint32_t numElements = cid.readUint32LE();

	cid.skip(4); // Always Zero? Or 64bit num elements?

	testFormat(cid);

	switch (_format) {
		case kSimple: _objectStream = std::make_unique<AWE::ObjectBinaryReadStreamV1>(cid, dp); break;
		case kStructured: _objectStream = std::make_unique<AWE::ObjectBinaryReadStreamV2>(cid, dp); break;
		case kStructuredV2:
			throw CreateException("Structured CID files in version 2, as used by Quantum Break and succeeding games "
								  "are currently not supported");
	}

	_containers.resize(numElements);
	for (auto &container : _containers) {
		container = _objectStream->readObject(type, _version);
	}
}

unsigned int CIDFile::getVersion() const {
	return _version;
}

const std::vector<Object> &CIDFile::getContainers() const {
	return _containers;
}

void CIDFile::testFormat(Common::ReadStream &cid) {
	// Simple test for determining the format of the file
	uint32_t deadbeefTest = cid.readUint32LE();
	switch (deadbeefTest) {
		case kDeadBeef:
			_format = kStructured;
			break;
		case kDeadBeefV2:
			_format = kStructuredV2;
			break;
		default:
			_format = kSimple;
			break;
	}

	cid.skip(-4);
}

} // End of namespace AWE
