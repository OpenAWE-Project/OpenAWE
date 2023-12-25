//
// Created by patrick on 08.12.23.
//

#ifndef OPENAWE_BINFOLLOADER_H
#define OPENAWE_BINFOLLOADER_H

#include "binmshloader.h"

namespace Graphics {

class BINFOLLoader : public MeshLoader {
public:
	BINFOLLoader();

	MeshPtr load(Common::ReadStream &stream, std::initializer_list<std::string> stages) const override;
};

} // End of namespace Graphics

#endif //OPENAWE_BINFOLLOADER_H
