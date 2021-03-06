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

#ifndef OPENAWE_EVENTMAN_H
#define OPENAWE_EVENTMAN_H

#include <map>
#include <functional>

#include "src/common/singleton.h"

#include "src/events/key.h"
#include "src/events/event.h"

namespace Events {

typedef std::function<void(const Event &event)> EventCallback;

/*!
 * \brief Class for exchanging input
 *
 * This class is a singleton for exchanging inputs to callbacks. A real input is for example a key on the keyboard or a
 * certain axis for example the mouse position, the scroll wheel or the game controller thumb stick. These raw input
 * events are mapped to one or more abstract actions. A callback is associated with multiple actions. If an input is
 * injected using the inject* methods, the assodciated actions are retrieved and the event is passed to the callbacks
 * associated actions.
 *
 * This design was chosen to decouple raw input events from the actions and allow to change the keybindings to be
 * changed at runtime.
 */
class EventManager : public Common::Singleton<EventManager> {
public:
	/*!
	 * Inject a keyboard event to the event system
	 * \param key The key for the event
	 * \param state If the key is pressed or released
	 */
	void injectKeyboardInput(Key key, KeyState state);

	/*!
	 * Set a callback for a set of actions
	 * \param actions The actions, the callback is associated with
	 * \param callback The callback, to be associated with actions
	 */
	void setActionCallback(std::initializer_list<uint32_t> actions, EventCallback callback);

	/*!
	 * Associate an with a specific keyboard key
	 * \param action The action for association with the key
	 * \param key The key for association with the action
	 */
	void addBinding(uint32_t action, Key key);

private:
	std::map<uint32_t, EventCallback> _actionCallbacks;

	std::multimap<Key, uint32_t> _keyBindings;
};

} // Enf of namespace Events

#define EventMan Events::EventManager::instance()

#endif //OPENAWE_EVENTMAN_H
