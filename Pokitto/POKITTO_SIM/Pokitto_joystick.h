#ifndef POKITTO_JOYSTICK_H
#define POKITTO_JOYSTICK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>

#define MAX_CONTROLLERS 4



class JoypadController {
public:
	JoypadController(); //: m_is_connected(false), m_gamepad(0), m_instance_id(-1), m_haptic(0) {}

	processEvent(const SDL_Event& event);
//private:
	SDL_GameController *m_gamepad;
	SDL_Haptic *m_haptic;
	SDL_JoystickID m_instance_id;
	bool m_is_connected;
int GetControllerIndex(SDL_JoystickID instance);
	void Open(int device);
	void Close();

};


#endif // POKITTO_JOYSTICK_H
