#pragma once
#include "IMAT3905.h"
#include <entt/entt.hpp>

#include "labelComponent.h"
#include "transformComponent.h"
#include "renderComponent.h"
#include "keyboardComponent.h"
#include "AIControllerComponent.h"

class Editor
{
public:
	void init(); //!< Initialise the editorm application
	void run();  //!< Run until closed
	void shutdown(); //!< Shutdown, free all resources
	void onEvent(SC::Event& e); //!< Event callback function
	bool onClose(SC::WindowCloseEvent& e) { SC::Application::getInstance()->shutdown(); return true; }; //!< Close the application
	bool onKeyPress(SC::KeyPressedEvent& e); //!< On a key pressed event
private:
	std::shared_ptr<SC::Application> m_application; //!< Application
	entt::registry m_registry; //!< ECS registry
	std::vector<entt::entity> m_entities; //!< Game objects

	//! A struct to store the element's of a keyboard mapping - including the key number, a text description of the action it's for, and a pointer to the function the key should execute.
	struct KeyBinding
	{
		//! Constructor to create a key binding.
		KeyBinding(int key, char* desc, void(*func)()) : keyNum(key), keyDesc(desc), boundFunc(func) {};
		//KeyBinding(int key, char* desc, void* func) : keyNum(key), keyDesc(desc), boundFunc(func) {};

		int keyNum;					//!< The SC int this key binding is for.
		char* keyDesc;				//!< A text description of the action this key is for.
		void(*boundFunc);			//!< Pointer to the function this key should trigger.
	};
	std::vector<KeyBinding> m_keyBindings; //!< List of defined key mappings.
	int m_numKeyBindings = 0;		   //!< How many keys mappings are defined.
};
