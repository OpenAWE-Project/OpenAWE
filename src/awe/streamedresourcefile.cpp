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

#include <regex>
#include <iostream>

#include "streamedresourcefile.h"
#include "src/awe/objectbinaryreadstreamv1.h"

static const uint32_t kBadFoood = 0xBADF000D;

StreamedResourceFile::StreamedResourceFile(Common::ReadStream &streamedResource) {
	const uint32_t magicId = streamedResource.readUint32LE();
	if (magicId != kBadFoood) {
		throw std::runtime_error("Invalid Streamed resource file");
	}

	const uint32_t version = streamedResource.readUint32LE();
	if (version != 1) {
		throw std::runtime_error("Streamed resource file version not supported");
	}

	const uint32_t v1 = streamedResource.readUint32LE();
	const uint32_t v2 = streamedResource.readUint32LE();
	const uint32_t numResources = streamedResource.readUint32LE();

	AWE::ObjectBinaryReadStreamV1 objectStream(streamedResource);

	std::vector<rid_t> rids;
	std::vector<uint32_t> pathOffsets;
	rids.resize(numResources);
	pathOffsets.resize(numResources);
	for (int i = 0; i < numResources; ++i) {
		std::vector<AWE::Object> objects;
		rid_t rid = streamedResource.readUint32BE();
		uint32_t pathOffset = streamedResource.readUint32LE();

		rids[i] = rid;
		pathOffsets[i] = pathOffset;

		objects.emplace_back(objectStream.readObject(kFileInfoMetadata));

		if (v1 == 4 && v2 == 0x00000024)
			streamedResource.skip(4);
		else if (v1 == 5 && v2 == 0x00000064)
			objects.emplace_back(objectStream.readObject(kFoliageMeshMetadata));
		else if (v1 == 6 && v2 == 160)
			objects.emplace_back(objectStream.readObject(kHavokAnimationMetadata));
		else if (v1 == 7 && v2 == 0x000000c8)
			objects.emplace_back(objectStream.readObject(kMeshMetadata));
		else if (v1 == 5 && v2 == 0x00000044)
			objects.emplace_back(objectStream.readObject(kParticleSystemMetadata));
		else if (v1 == 10 && v2 == 0x00000064)
			objects.emplace_back(objectStream.readObject(kTextureMetadata));
	}

	uint32_t nameSize = streamedResource.readUint32LE();
	for (int i = 0; i < rids.size(); ++i) {
		uint32_t pathOffset = pathOffsets[i];

		streamedResource.seek(
				-static_cast<int>(nameSize) + static_cast<int>(pathOffset),
				Common::ReadStream::END
		);

		std::string fileName = streamedResource.readNullTerminatedString();
		fileName = std::regex_replace(fileName, std::regex("\\\\"), "/");
		fileName = std::regex_replace(fileName, std::regex("data/"), "");

		//spdlog::trace rids[i] << " " << fileName << std::endl;

		_resources.insert(std::make_pair(rids[i], fileName));
	}
}
