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

#include <CLI/CLI.hpp>
#include <entt/entt.hpp>

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include "3rdparty/tinygltf/tiny_gltf.h"

#include "src/common/readfile.h"
#include "src/common/memwritestream.h"
#include "src/common/strutil.h"
#include "src/common/exception.h"

#include "src/awe/havokfile.h"
#include "src/awe/resman.h"
#include "src/awe/types.h"

namespace AWE {

GID parseGID(const std::string &val) {
	const auto split = Common::split(val, std::regex(":"));
	assert(split.size() == 2);

	GID gid{
			static_cast<uint32_t>(std::stoul(split[0])),
			static_cast<uint32_t>(std::stoul(split[1], nullptr, 16))
	};

	return gid;
}

}

void loadSkeleton(tinygltf::Model &model, const AWE::Templates::Skeleton &skeleton1, std::map<std::string, size_t> &boneToNode) {
	std::unique_ptr<Common::ReadStream> havokStream(ResMan.getResource(skeleton1.rid));
	AWE::HavokFile havok(*havokStream);

	const auto skeleton = havok.getSkeleton(havok.getAnimationContainer().skeletons[0]);

	tinygltf::Skin skin;
	skin.name = skeleton1.name;

	tinygltf::Node skeletonNode;
	skeletonNode.name = skeleton1.name;
	skeletonNode.skin = 0;
	model.nodes.emplace_back(skeletonNode);

	std::vector<tinygltf::Node> parentNodes;
	for (const auto &bone: skeleton.bones) {
		tinygltf::Node boneNode;

		boneNode.name = bone.name;
		boneNode.translation = {bone.position.x, bone.position.y, bone.position.z};
		boneNode.rotation = {bone.rotation.x, bone.rotation.y, bone.rotation.z, bone.rotation.w};
		boneNode.scale = {bone.scale.x, bone.scale.y, bone.scale.z};

		if (bone.parentIndex > -1)
			parentNodes[bone.parentIndex].children.emplace_back(parentNodes.size() + 1);

		boneToNode[bone.name] = parentNodes.size() + 1;
		skin.joints.emplace_back(parentNodes.size() + 1);
		parentNodes.emplace_back(boneNode);
	}

	for (const auto &node: parentNodes) {
		model.nodes.emplace_back(node);
	}

	model.skins.emplace_back(skin);
}

void loadAnimation(tinygltf::Model &model, const AWE::Templates::Animation &animation1, std::map<std::string, size_t> &boneToNode) {
	std::unique_ptr<Common::ReadStream> havokStream(ResMan.getResource(animation1.rid));
	if (!havokStream)
		return;

	AWE::HavokFile havok(*havokStream);

	const auto animation = havok.getAnimation(havok.getAnimationContainer().animations[0]);

	const auto nextBuffer = model.buffers.size();
	const auto nextBufferView = model.bufferViews.size();
	const auto nextAccessor = model.accessors.size();

	tinygltf::Animation anim;
	anim.name = animation1.name;

	unsigned int numKeyframes = 0;
	unsigned int numTracks = 0;
	for (const auto &block: animation.tracks) {
		unsigned int maxKeyframes = 0;
		for (const auto &track: block) {
			maxKeyframes = std::max<unsigned int>(maxKeyframes, track.positions.size());
			maxKeyframes = std::max<unsigned int>(maxKeyframes, track.rotations.size());
		}

		numTracks = std::max<unsigned int>(numTracks, block.size());

		numKeyframes += maxKeyframes;
	}

	Common::DynamicMemoryWriteStream timeData(true);
	for (unsigned int i = 0; i < numKeyframes; ++i) {
		timeData.writeIEEEFloatLE(i * animation.frameDuration);
	}
	timeData.writeIEEEFloatLE(animation.duration);

	tinygltf::Buffer timeBuffer;
	timeBuffer.data.resize(timeData.getLength());
	std::memcpy(timeBuffer.data.data(), timeData.getData(), timeData.getLength());

	tinygltf::BufferView timeBufferView;
	timeBufferView.buffer = nextBuffer;
	timeBufferView.byteLength = timeData.getLength();
	timeBufferView.byteOffset = 0;

	tinygltf::Accessor timeBufferAccessor;
	timeBufferAccessor.bufferView = nextBufferView;
	timeBufferAccessor.count = numKeyframes;
	timeBufferAccessor.type = TINYGLTF_TYPE_SCALAR;
	timeBufferAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
	const auto timeAccessorId = nextAccessor;

	model.buffers.emplace_back(timeBuffer);
	model.bufferViews.emplace_back(timeBufferView);
	model.accessors.emplace_back(timeBufferAccessor);

	Common::DynamicMemoryWriteStream animationData(true);

	std::map<size_t, std::string> trackToBoneName;
	for (const auto &toTrack : animation.boneToTrack) {
		trackToBoneName[toTrack.second] = toTrack.first;
	}

	unsigned int bufferViewCounter = nextBufferView + 1;
	unsigned int accessorCounter = nextAccessor + 1;
	unsigned int samplerCounter = 0;
	for (unsigned int i = 0; i < numTracks; ++i) {
		const auto hasPositions = !animation.tracks[0][i].positions.empty();
		const auto hasRotations = !animation.tracks[0][i].rotations.empty();
		const auto staticPosition = animation.tracks[0][i].positions.size() == 1;
		const auto staticRotation = animation.tracks[0][i].rotations.size() == 1;

		if (!hasPositions && !hasRotations)
			continue;

		const auto offset = animationData.getLength();

		for (const auto &block: animation.tracks) {
			const auto numPositions = block[i].positions.size();
			const auto numRotations = block[i].rotations.size();

			for (unsigned int j = 0; j < numKeyframes; ++j) {
				if (hasPositions) {
					const auto position = block[i].positions[j % numPositions];
					animationData.writeIEEEFloatLE(position.x);
					animationData.writeIEEEFloatLE(position.y);
					animationData.writeIEEEFloatLE(position.z);
				}

				if (hasRotations) {
					const auto rotation = block[i].rotations[j % numRotations];
					animationData.writeIEEEFloatLE(rotation.x);
					animationData.writeIEEEFloatLE(rotation.y);
					animationData.writeIEEEFloatLE(rotation.z);
					animationData.writeIEEEFloatLE(rotation.w);
				}
			}
		}

		const auto endOffset = animationData.getLength();

		tinygltf::BufferView dataView;
		dataView.byteOffset = offset;
		dataView.byteLength = endOffset - offset;
		dataView.byteStride = 0;
		if (hasPositions)
			dataView.byteStride += 12;
		if (hasRotations)
			dataView.byteStride += 16;
		dataView.buffer = nextBuffer + 1;

		tinygltf::Accessor positionAccessor, rotationAccessor;

		model.bufferViews.emplace_back(dataView);

		tinygltf::AnimationSampler positionSampler, rotationSampler;
		tinygltf::AnimationChannel positionChannel, rotationChannel;

		if (hasPositions) {
			positionAccessor.bufferView = bufferViewCounter;
			positionAccessor.type = TINYGLTF_TYPE_VEC3;
			positionAccessor.count = numKeyframes;
			positionAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
			positionAccessor.byteOffset = 0;
			const auto positionsAccessorId = accessorCounter;
			++accessorCounter;

			model.accessors.emplace_back(positionAccessor);

			positionSampler.input = timeAccessorId;
			positionSampler.interpolation = "LINEAR";
			positionSampler.output = positionsAccessorId;
			const auto positionsSamplerId = samplerCounter++;

			positionChannel.target_node = boneToNode.at(trackToBoneName.at(i));
			positionChannel.target_path = "translation";
			positionChannel.sampler = positionsSamplerId;

			anim.samplers.emplace_back(positionSampler);
			anim.channels.emplace_back(positionChannel);
		}

		if (hasRotations) {
			rotationAccessor.bufferView = bufferViewCounter;
			rotationAccessor.type = TINYGLTF_TYPE_VEC4;
			rotationAccessor.count = numKeyframes;
			rotationAccessor.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
			if (hasPositions)
				rotationAccessor.byteOffset = 12;
			else
				rotationAccessor.byteOffset = 0;
			const auto rotationsAccessorId = accessorCounter;
			++accessorCounter;

			model.accessors.emplace_back(rotationAccessor);

			rotationSampler.input = timeAccessorId;
			rotationSampler.interpolation = "LINEAR";
			rotationSampler.output = rotationsAccessorId;
			const auto rotationsSamplerId = samplerCounter++;

			rotationChannel.target_node = boneToNode.at(trackToBoneName.at(i));
			rotationChannel.target_path = "rotation";
			rotationChannel.sampler = rotationsSamplerId;

			anim.samplers.emplace_back(rotationSampler);
			anim.channels.emplace_back(rotationChannel);
		}

		bufferViewCounter++;
	}

	tinygltf::Buffer animationBuffer;
	animationBuffer.data.resize(animationData.getLength());
	std::memcpy(animationBuffer.data.data(), animationData.getData(), animationData.getLength());

	model.buffers.emplace_back(animationBuffer);

	model.animations.emplace_back(anim);
}

int main(int argc, char** argv) {
	CLI::App app("Convert a havok file to a gltf file", "havok2gltf");

	std::string characterClassString;
	std::string resourcePath;
	std::string meshFile;

	app.add_option("class", characterClassString, "The gid of the character class to generate the gltf file for")
			->check(GIDValidator())
			->required();

	app.add_option("path", resourcePath, "The path to initialize the resources")
			->check(CLI::ExistingPath)
			->required();

	CLI11_PARSE(app, argc, argv);

	GID characterClassID = AWE::parseGID(characterClassString);
	std::vector<std::string> identifiers;

	ResMan.setRootPath(resourcePath);

	// Index rmdp archives
	for (const auto &path : std::filesystem::directory_iterator(resourcePath)) {
		if (!path.is_regular_file())
			continue;

		if (!std::regex_match(path.path().string(), std::regex(".+\\.rmdp$")))
			continue;

		std::string rmdpFile = path.path().string();
		std::string binFile = std::regex_replace(rmdpFile, std::regex("\\.rmdp$"), ".bin");

		identifiers.emplace_back(path.path().filename().stem().string());
		ResMan.indexArchive(binFile, rmdpFile);
	}

	for (const auto &identifier : identifiers) {
		ResMan.indexPackmeta(identifier + ".packmeta");
	}

	// Load all Skeletons, animations
	std::unique_ptr<Common::ReadStream> cidSkeletonStream(ResMan.getResource("global/cid_skeleton.bin"));
	std::unique_ptr<Common::ReadStream> cidAnimationStream(ResMan.getResource("global/cid_animation.bin"));
	std::unique_ptr<Common::ReadStream> cidCharacterClassStream(ResMan.getResource("global/cid_characterclass.bin"));
	std::shared_ptr<DPFile> dpFile = std::make_shared<DPFile>(ResMan.getResource("global/dp_global.bin"));

	AWE::CIDFile cidSkeleton(*cidSkeletonStream, kSkeleton, dpFile);
	AWE::CIDFile cidAnimation(*cidAnimationStream, kAnimation, dpFile);
	AWE::CIDFile cidCharacterClass(*cidCharacterClassStream, kCharacterClass, dpFile);

	std::map<GID, AWE::Templates::Skeleton> skeletons;
	std::vector<AWE::Templates::Animation> animations;
	std::map<GID, AWE::Templates::CharacterClass> characterClasses;
	for (const auto &container: cidSkeleton.getContainers()) {
		const auto skeleton = std::any_cast<AWE::Templates::Skeleton>(container);
		skeletons[skeleton.gid] = skeleton;
	}
	for (const auto &container: cidAnimation.getContainers()) {
		const auto animation = std::any_cast<AWE::Templates::Animation>(container);
		animations.emplace_back(animation);
	}
	for (const auto &container: cidCharacterClass.getContainers()) {
		const auto characterClass = std::any_cast<AWE::Templates::CharacterClass>(container);
		characterClasses[characterClass.gid] = characterClass;
	}

	const auto characterClass = characterClasses.at(characterClassID);
	const auto skeleton = skeletons.at(characterClass.skeletonGid);

	std::map<std::string, size_t> boneNameToNode;

	tinygltf::TinyGLTF gltf;
	tinygltf::Scene scene;
	tinygltf::Model model;

	loadSkeleton(model, skeleton, boneNameToNode);

	for (const auto &animationObject: characterClass.animations) {
		loadAnimation(model, animations[animationObject.getID()], boneNameToNode);
	}

	scene.nodes.emplace_back(0);
	model.scenes.emplace_back(scene);

	gltf.WriteGltfSceneToFile(&model, fmt::format("{}.gltf", characterClass.name), false, true, true);
}