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

# Macro for compiling a certain technique file and all its associated shaders into various permutations
macro(compile_technique target glslc_executable technique_path)
    get_filename_component(technique_file ${technique_path} NAME)
    message(STATUS "Reading technique file ${technique_file}")

    file(READ ${technique_path} technique_data)
    string(JSON technique_name GET ${technique_data} "name")
    string(JSON property_sets_data GET ${technique_data} "property_sets")
    string(JSON num_property_sets LENGTH ${technique_data} "property_sets")

    if(${num_property_sets} GREATER 0)
        math(EXPR num_property_sets "${num_property_sets} - 1")
    endif()

    set(generated_shaders)

    # Go over property sets
    foreach(property_set_index RANGE ${num_property_sets})
        string(JSON property_set_data GET ${property_sets_data} ${property_set_index})
        string(JSON properties_array GET ${property_set_data} "properties")
        string(JSON stages_array GET ${property_set_data} "stages")
        string(JSON num_stages LENGTH ${property_set_data} "stages")

        if(${num_stages} GREATER 0)
            math(EXPR num_stages "${num_stages} - 1")
        endif()

        # Get all names of the stages, on which the properties are applies
        set(STAGE_NAMES)
        foreach(stage_index RANGE ${num_stages})
            string(JSON stage GET ${stages_array} ${stage_index})
            list(APPEND STAGE_NAMES ${stage})
        endforeach()

        # Get the names and flags
        set(PROPERTY_NAMES)
        set(PROPERTY_FLAGS)
        set(PROPERTY_PERMUTATIONS_NAMES "/")
        set(PROPERTY_PERMUTATIONS_FLAGS "0")
        string(JSON num_properties LENGTH ${property_set_data} "properties")
        if(${num_properties} GREATER 0)
            math(EXPR num_properties "${num_properties} - 1")
            foreach(properties_index RANGE ${num_properties})
                string(JSON property_data GET ${properties_array} ${properties_index})
                string(JSON property_name GET ${property_data} "name")
                string(JSON property_flag GET ${property_data} "flag")
                list(APPEND PROPERTY_NAMES ${property_name})
                list(APPEND PROPERTY_FLAGS ${property_flag})
            endforeach()
        endif()

        # Create all possible permutations of the properties
        foreach(name flag IN ZIP_LISTS PROPERTY_NAMES PROPERTY_FLAGS)
            set(permutations_true)
            set(permutations_true_flags)
            set(permutations_false ${PROPERTY_PERMUTATIONS_NAMES})
            set(permutations_false_flags ${PROPERTY_PERMUTATIONS_FLAGS})
            foreach(permutation permutation_flag IN ZIP_LISTS PROPERTY_PERMUTATIONS_NAMES PROPERTY_PERMUTATIONS_FLAGS)
                string(APPEND permutation ${name}/)
                list(APPEND permutations_true ${permutation})
                if(permutation_flag)
                    math(EXPR new_flag "${permutation_flag} | (1 << ${flag})" OUTPUT_FORMAT HEXADECIMAL)
                else()
                    math(EXPR new_flag "1 << ${flag}" OUTPUT_FORMAT HEXADECIMAL)
                endif()
                list(APPEND permutations_true_flags ${new_flag})
            endforeach()

            set(PROPERTY_PERMUTATIONS_NAMES ${permutations_true} ${permutations_false})
            set(PROPERTY_PERMUTATIONS_FLAGS ${permutations_true_flags} ${permutations_false_flags})
        endforeach()

        set(technique_target ${target}_${technique_name})
        foreach(permutation flag IN ZIP_LISTS PROPERTY_PERMUTATIONS_NAMES PROPERTY_PERMUTATIONS_FLAGS)
            string(REGEX MATCHALL "[A-Z\\_]+/" properties ${permutation})
            string(REPLACE "/" "" properties "${properties}")

            string(REPLACE "0x" "" flag ${flag})
            string(LENGTH ${flag} flag_length)
            math(EXPR zeros_to_add "8 - ${flag_length} - 1")
            if (NOT zeros_to_add LESS_EQUAL 0)
                foreach(i RANGE ${zeros_to_add})
                    set(flag 0${flag})
                endforeach()
            endif ()
            set(flag 0x${flag})

            set(glslc_properties)
            foreach(property IN ITEMS ${properties})
                list(APPEND glslc_properties -D${property})
            endforeach()

            foreach(stage_name IN ITEMS ${STAGE_NAMES})
                file(GLOB SHADER_FILES "shaders/${technique_name}*${stage_name}*.glsl")
                foreach(shader_source_path IN ITEMS ${SHADER_FILES})
                    get_filename_component(shader_target ${shader_source_path} NAME_WE)
                    get_filename_component(shader_type ${shader_source_path} NAME_WLE)
                    get_filename_component(shader_type ${shader_type} EXT)
                    get_filename_component(shader_source_file ${shader_source_path} NAME)
                    set(shader_glsl_file ${shader_target}-${flag}${shader_type}.glsl)
                    add_custom_command(
                            OUTPUT ${CMAKE_BINARY_DIR}/${shader_glsl_file}
                            COMMENT "Building glsl shader ${shader_source_file} for property flags ${flag}"
                            DEPENDS ${shader_source_path}
                            COMMAND
                            ${glslc_executable}
                            ${glslc_properties}
                            -E
                            -o ${CMAKE_BINARY_DIR}/${shader_glsl_file}
                            --target-env=opengl
                            ${shader_source_path}
                    )
                    list(APPEND generated_shaders ${CMAKE_BINARY_DIR}/${shader_glsl_file})
                endforeach()
            endforeach()
        endforeach()
    endforeach()

    add_custom_target(
            ${technique_target}
            ALL
            DEPENDS
            ${generated_shaders}
    )
    add_dependencies(${target} ${technique_target})
endmacro()
