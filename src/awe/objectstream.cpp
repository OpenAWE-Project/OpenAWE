//
// Created by patrick on 28.01.21.
//

#include "objectstream.h"

namespace AWE {

ObjectReadStream::ObjectReadStream(ReadStream &objectStream, unsigned int type, unsigned int version) :
		Common::ReadStream(objectStream),
		_type(type), _version(version) {
}

}