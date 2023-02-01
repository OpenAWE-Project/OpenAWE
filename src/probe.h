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

#ifndef AWE_PROBE_H
#define AWE_PROBE_H

#include "src/awe/resman.h"

enum ProbeResult {
	kResultUnknown 				= 0,
	kResultAlanWake 			= 1,
	kResultNightmare 			= 2,
	kResultQuantumBreak 		= 4,
	kResultAlanWakeRemastered 	= 8,
};

class Probe {
public:
	Probe();

	/*!
	 * Checks game archives to find out what game data have been
	 * supplied to the engine.
	 * 
	 * \return One of the ProbeResult values
	 */
	unsigned char performProbe();

private:
	/*!
	 * Checks whether supplied data archives are of
	 * original Alan Wake
	 */
	ProbeResult checkAlanWake();
	
	/*!
	 * Checks whether supplied data archives are of
	 * Alan Wake's American Nightmare
	 */
	ProbeResult checkNightmare();

	/*!
	 * Checks whether supplied data archives are of
	 * Quantum Break
	 */
	ProbeResult checkQuantumBreak();

	/*!
	 * Checks whether supplied data archives are of
	 * Alan Wake Remastered
	 */
	ProbeResult checkAlanWakeRemastered();
	
	AWE::RessourceManager* _resources;

};

#endif //AWE_PROBE_H