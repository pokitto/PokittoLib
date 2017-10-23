#include <stdio.h>
#include "Pokitto_joystick.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>


JoypadController m_controllers[MAX_CONTROLLERS];

JoypadController::JoypadController() {
  m_is_connected=false;
  m_gamepad=0;
  m_instance_id=-1;
  m_haptic=0;
}

// Opens the joystick controller
void JoypadController::Open(int device)
{
	m_gamepad = SDL_GameControllerOpen(device);
	SDL_Joystick *j = SDL_GameControllerGetJoystick(m_gamepad);
	m_instance_id = SDL_JoystickInstanceID(j);
	m_is_connected = true;
	if (SDL_JoystickIsHaptic(j)) {
		m_haptic = SDL_HapticOpenFromJoystick(j);
		printf("Haptic Effects: %d\n", SDL_HapticNumEffects(m_haptic));
		printf("Haptic Query: %x\n", SDL_HapticQuery(m_haptic));
		if (SDL_HapticRumbleSupported(m_haptic)) {
			if (SDL_HapticRumbleInit(m_haptic) != 0) {
				printf("Haptic Rumble Init: %s\n", SDL_GetError());
				SDL_HapticClose(m_haptic);
				m_haptic = 0;
			}
		} else {
			SDL_HapticClose(m_haptic);
			m_haptic = 0;
		}
	}
}

void JoypadController::Close()
{
	if (m_is_connected) {
		m_is_connected = false;
		if (m_haptic) {
			SDL_HapticClose(m_haptic);
			m_haptic = 0;
		}
		SDL_GameControllerClose(m_gamepad);
		m_gamepad = 0;
	}
}


int JoypadController::GetControllerIndex(SDL_JoystickID instance)
{
	for (int i = 0; i < MAX_CONTROLLERS; ++i)
	{
		if (m_controllers[i].m_is_connected &&  m_controllers[i].m_instance_id == instance) {
			return i;
		}
	}
	return -1;
}

int JoypadController::processEvent(const SDL_Event& event)
{
	switch (event.type) {
		case SDL_CONTROLLERAXISMOTION: {
			// handle axis motion
			break;
		}
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERBUTTONUP: {
			// handle button up/down
			break;
		}
		case SDL_CONTROLLERDEVICEADDED: {
			if (event.cdevice.which < MAX_CONTROLLERS ) {
				JoypadController& jc = m_controllers[event.cdevice.which];
				jc.Open(event.cdevice.which);
			}
			break;
		}
		case SDL_CONTROLLERDEVICEREMOVED: {
			int cIndex = GetControllerIndex(event.cdevice.which);
			if (cIndex < 0) return 0; // unknown controller?
			JoypadController& jc = m_controllers[cIndex];
			jc.Close();
			break;
		}
	}
	return 0;
}
