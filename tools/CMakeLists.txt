# OpenAWE - A reimplementation of Remedy's Alan Wake Engine
#
# OpenAWE is the legal property of its developers, whose names
# can be found in the AUTHORS file distributed with this source
# distribution.
#
# OpenAWE is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 3
# of the License, or (at your option) any later version.
#
# OpenAWE is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with OpenAWE. If not, see <http://www.gnu.org/licenses/>.

add_executable(unrmdp unrmdp.cpp)
target_link_libraries(
        unrmdp
        awe_common
        awe_lib
)

add_executable(unbin unbin.cpp)
target_link_libraries(
        unbin
        awe_common
        awe_lib
        ${ZLIB_LIBRARIES}
)

add_executable(unobj unobj.cpp)
target_link_libraries(
        unobj
        awe_common
        awe_lib
)

add_executable(packmeta2xml packmeta2xml.cpp)
target_link_libraries(
        packmeta2xml
        awe_common
        awe_lib
        tinyxml2::tinyxml2
)

add_executable(streamedresources2xml streamedresources2xml.cpp)
target_link_libraries(
        streamedresources2xml
        awe_common
        awe_lib
        tinyxml2::tinyxml2
)

add_executable(cid2xml cid2xml.cpp)
target_link_libraries(
        cid2xml
        awe_common
        awe_lib
        tinyxml2::tinyxml2
)

add_executable(tex2dds tex2dds.cpp)
target_link_libraries(
        tex2dds
        awe_common
        awe_graphics
)

add_executable(fsb2wav fsb2wav.cpp)
target_link_libraries(
        fsb2wav
        awe_common
        awe_sound
)

add_executable(havok2gltf havok2gltf.cpp)
target_link_libraries(
        havok2gltf
        awe_common
        awe_lib
)

if (LZ4_FOUND)
    add_executable(unrmdblob unrmdblob.cpp)
    target_link_libraries(
            unrmdblob
            awe_common
            awe_sound
    )

    install(
            TARGETS unrmdblob
            DESTINATION ${CMAKE_INSTALL_BINDIR}
            RUNTIME_DEPENDENCIES
            POST_EXCLUDE_REGEXES ld-linux*
    )
endif ()

install(
        TARGETS
        unrmdp
        unbin
        unobj
        packmeta2xml
        streamedresources2xml
        cid2xml
        tex2dds
        fsb2wav

        DESTINATION ${CMAKE_INSTALL_BINDIR}
        RUNTIME_DEPENDENCIES
        POST_EXCLUDE_REGEXES ld-linux*
)
