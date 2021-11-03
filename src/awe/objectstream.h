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

#ifndef OPENAWE_OBJECTSTREAM_H
#define OPENAWE_OBJECTSTREAM_H

#include <memory>
#include <variant>
#include <any>

#include "src/common/readstream.h"
#include "src/common/types.h"

#include "src/awe/object.h"
#include "src/awe/dpfile.h"
#include "src/awe/types.h"

namespace AWE {

typedef std::any Object;

class ObjectStream {
protected:
	void resourceID(rid_t &);
	void boundBox(Common::BoundBox &aabb);
	void staticObject(Templates::StaticObject &staticObject);
	void dynamicObject(Templates::DynamicObject &dynamicObject, unsigned int version);
	void dynamicObjectScript(Templates::DynamicObjectScript &dynamicObjectScript, unsigned int version);
	void cellInfo(Templates::CellInfo &cellInfo);
	void animation(Templates::Animation &animation, unsigned int version);
	void skeleton(Templates::Skeleton &skeleton);
	void skeletonSetup(Templates::SkeletonSetup &skeletonSetup);
	void notebookPage(Templates::NotebookPage &notebookPage);
	void sound(Templates::Sound &sound);
	void character(Templates::Character &character, unsigned int version);
	void characterScript(Templates::CharacterScript &characterScript);
	void characterClass(Templates::CharacterClass &characterClass, unsigned int version);
	void taskDefinition(Templates::TaskDefinition &taskDefinition, unsigned int version);
	void taskContent(Templates::TaskContent &taskContent);
	void scriptVariables(Templates::ScriptVariables &script, unsigned int version);
	void script(Templates::Script &script);
	void scriptInstance(Templates::ScriptInstance &scriptInstance);
	void pointLight(Templates::PointLight &pointLight, unsigned int version);
	void ambientLightInstance(Templates::AmbientLightInstance ambientLightInstance);
	void floatingScript(Templates::FloatingScript floatingScript);
	void trigger(Templates::Trigger &trigger, unsigned int version);
	void areaTrigger(Templates::AreaTrigger &areaTrigger);
	void attachmentResources(Templates::AttachmentResource &attachmentResource);
	void waypoint(Templates::Waypoint &waypoint);
	void animationParameters(Templates::AnimationParameters &animationParameters);
	void keyFramedObject(Templates::KeyFramedObject &keyFramedObject, unsigned int version);
	void keyFramer(Templates::KeyFramer &keyFramer);
	void keyFrameAnimation(Templates::KeyFrameAnimation &keyFrameAnimation);
	void keyFrame(Templates::KeyFrame &keyFrame);

	void readFileInfoMetadata(Templates::FileInfoMetadata &fileInfoMetadata);
	void foliageMeshMetadata(Templates::FoliageMeshMetadata foliageMeshMetadata);
	void havokAnimationMetadata(Templates::HavokAnimationMetadata &havokAnimationMetadata);
	void readMeshMetadata(Templates::MeshMetadata &meshMetadata);
	void readParticleSystemMetadata(Templates::ParticleSystemMetadata particleSystemMetadata);
	void textureMetadata(Templates::TextureMetadata &textureMetadata);

	virtual void skip(size_t s) = 0;

	virtual void variable(const std::string &name, bool &value) = 0;
	virtual void variable(const std::string &name, int32_t &value) = 0;
	virtual void variable(const std::string &name, uint32_t &value, bool bigEndian = false) = 0;
	virtual void variable(const std::string &name, float &value) = 0;
	virtual void variable(const std::string &name, std::string &value, bool dp) = 0;
	virtual void variable(const std::string &name, glm::vec3 &value) = 0;
	virtual void variable(const std::string &name, glm::mat3 &value) = 0;
	virtual void variable(const std::string &name, GID &value) = 0;
	virtual void variable(const std::string &name, ObjectID &value) = 0;
	virtual void variable(const std::string &name, std::vector<bool> &value, size_t fixedSize) = 0;
	virtual void variable(const std::string &name, std::vector<int32_t> &value) = 0;
	virtual void variable(const std::string &name, std::vector<uint32_t> &value, bool dp) = 0;
	virtual void variable(const std::string &name, std::vector<uint32_t> &value, size_t fixedSize) = 0;
	virtual void variable(const std::string &name, std::vector<rid_t> &value) = 0;
	virtual void variable(const std::string &name, std::vector<glm::vec2> &value) = 0;
	virtual void variable(const std::string &name, std::vector<float> &value) = 0;
	virtual void variable(const std::string &name, std::vector<ObjectID> &value) = 0;
	virtual void variable(const std::string &name, std::vector<std::string> &value) = 0;
	virtual void variable(const std::string &name, std::vector<std::string> &value, size_t fixedSize) = 0;

	void object(Object &value, ObjectType type, unsigned int version);
	virtual void object(const std::string &name, Object &value, ObjectType type) = 0;
	virtual void objects(const std::string &name, std::vector<Object> &value, ObjectType type) = 0;

private:
	template<typename T> T& as(Object &o) {
		if (!o.has_value())
			o = std::make_any<T>();
		return std::any_cast<T&>(o);
	}

	template<typename T> void object(const std::string &name, T &value, ObjectType type) {
		Object o = value;
		if (!o.has_value())
			o = std::make_any<T>();
		object(name, o, type);
		value = std::any_cast<T>(o);
	}

	template<typename T> void objects(const std::string &name, std::vector<T> &values, ObjectType type) {
		std::vector<Object> os;
		std::copy(values.begin(), values.end(), std::back_inserter(os));
		objects(name, os, type);
	}
};

class ObjectReadStream : public ObjectStream {
public:
	virtual Object readObject(ObjectType type, unsigned int version = 0) = 0;

protected:
	ObjectReadStream();
};

}

#endif //OPENAWE_OBJECTSTREAM_H

