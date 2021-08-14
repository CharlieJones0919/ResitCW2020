#include "editor.h"
#include "ImGuiHelper.h"
#include <map>

TransformComponent* KeyboardComponent::m_keyTransComp = nullptr;
glm::vec3 KeyboardComponent::wFRONT = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 KeyboardComponent::wUP = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 KeyboardComponent::wSIDE = glm::vec3(1.0f, 0.0f, 0.0f);
float KeyboardComponent::m_speed = 0.0f;

void Editor::init()
{
	// Start the singleton application
	m_application.reset(SC::Application::getInstance());

	SC::WindowProperties props;
	props.m_width = 1200;
	props.m_height = 800;
	props.m_title = "IMAT3905 Resit";
	m_application->init(props);

	// Set the event callback to this class
	auto window = SC::Application::getInstance()->getWindow();
	window->setEventCallback(std::bind(&Editor::onEvent, this, std::placeholders::_1));

	// Start the renderer
	SC::Renderer::init();

	// Setup ImGui
	ImGuiHelper::init();

	// Set up Game Objects
	m_entities.reserve(100);

	// Floor
	m_entities.push_back(m_registry.create());
	m_registry.emplace<LabelComponent>(m_entities.back(), "Floor");
	m_registry.emplace<TransformComponent>(m_entities.back(), glm::vec3(0.f, -0.05f, 0.f), glm::vec3(0.f), glm::vec3(10.f,0.1f,10.f));
	m_registry.emplace<RenderComponent>(m_entities.back(), MeshType::Cuboid, glm::vec3(0.f, 1.f, 0.f));

	// Sphere
	m_entities.push_back(m_registry.create());
	m_registry.emplace<LabelComponent>(m_entities.back(), "Sphere");
	m_registry.emplace<TransformComponent>(m_entities.back(), glm::vec3(-4.5f, 0.5f, -4.5f), glm::vec3(0.f), glm::vec3(1.f));
	m_registry.emplace<RenderComponent>(m_entities.back(), MeshType::Sphere, glm::vec3(1.f, 0.f, 0.f));
	m_registry.emplace<AIControllerComponent>(m_entities.back());

	// Cube
	m_entities.push_back(m_registry.create());
	m_registry.emplace<LabelComponent>(m_entities.back(), "Cube");
	auto& transformCompB = m_registry.emplace<TransformComponent>(m_entities.back(), glm::vec3(0.f, 0.5f, 0.f), glm::vec3(0.f), glm::vec3(1.f));
	m_registry.emplace<RenderComponent>(m_entities.back(), MeshType::Cuboid, glm::vec3(0.f, 0.f, 1.f));
	m_registry.emplace<KeyboardComponent>(m_entities.back(), &transformCompB, 0.05f);

	// Setup Keyboard Bindings
	m_keyBindings.reserve(m_numKeyBindings);
	m_keyBindings.push_back(KeyBinding(SC_KEY_W, "	   Move Camera Forward", &(SC::Renderer::cameraForward))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_A, "	   Move Camera Left", &(SC::Renderer::cameraLeft))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_S, "	   Move Camera Back", &(SC::Renderer::cameraBack))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_D, "	   Move Camera Right", &(SC::Renderer::cameraRight))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_Q, "	   Rotate Camera Up", &(SC::Renderer::cameraUp))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_E, "	   Rotate Camera Down", &(SC::Renderer::cameraDown))); m_numKeyBindings++;

	m_keyBindings.push_back(KeyBinding(SC_KEY_I, "	   Move Playable Object Forward", &(KeyboardComponent::moveForward))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_J, "	   Move Playable Object Left", &(KeyboardComponent::moveLeft))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_K, "	   Move Playable Object Back", &(KeyboardComponent::moveBack))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_L, "	   Move Playable Object Right", &(KeyboardComponent::moveRight))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_U, "	   Rotate Playable Object Left", &(KeyboardComponent::rotateLeft))); m_numKeyBindings++;
	m_keyBindings.push_back(KeyBinding(SC_KEY_O, "	   Rotate Playable Object Right", &(KeyboardComponent::rotateRight))); m_numKeyBindings++;
}

void Editor::run()
{
	float elapsedTime = 0.f;

	while (m_application->isRunning())
	{
		elapsedTime += m_application->resetTimer();
		if (elapsedTime > 1.0 / 60.f)
		{
			ImGuiHelper::writeToConsole("Frame time (ms): " + std::to_string(elapsedTime));

			// Draw all game objects

			SC::Renderer::beginScene();
			
			auto renderView = m_registry.view<RenderComponent, TransformComponent>();
			for (auto entity : renderView)
			{
				auto renderComp = m_registry.get<RenderComponent>(entity);
				auto transformComp = m_registry.get<TransformComponent>(entity);

				auto meshType = renderComp.getMeshType();
				auto colour = renderComp.getColour();
				auto model = transformComp.getTransform();

				switch (meshType)
				{
				case MeshType::Capsule:
					SC::Renderer::drawCapsule(model, 0.5f, 0.5f, colour);
					break;
				case MeshType::Cuboid:
					SC::Renderer::drawCube(model, { 1.f,1.f,1.f }, colour);
					break;
				case MeshType::Sphere:
					SC::Renderer::drawSphere(model, 1.0f, colour);
				}
			}
			SC::Renderer::endScene();

			// ImGui stuff
			ImGuiHelper::begin();

			// Renderer output window
			ImGui::Begin("Renderer output");
			uint32_t textureID = SC::Renderer::getFramebufferTextureID();
			ImGui::Image((void*)textureID, ImVec2{ 800, 600 }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
			ImGui::End();

			// Game object settings window

			ImGui::Begin("Properties");
			// Game object list box
			// Get char * for entity labels
			auto labelView = m_registry.view<LabelComponent>();
			std::vector<char *> labels(labelView.size());
			int i = 0;
			for (auto entity : labelView)
			{
				auto labelComp = m_registry.get<LabelComponent>(entity);
				const std::string& name = labelComp.label;
				int len = strlen(name.c_str()) + 1;
				labels[i] = (char*)malloc(len);
				strcpy_s(labels[i], len, name.c_str());
				i++;
			}

			ImGui::TextWrapped("Game Objects:");
			static int labelIndex = 0;
			ImGui::SetNextItemWidth(-1);
			ImGui::ListBox("##GameObjects", &labelIndex, labels.data(), labels.size(), labels.size());
			// Clean up char * for list box
			for (auto it = labels.begin(); it != labels.end(); ++it) free(*it);
			
			// Component list box
			// Get char * for component types/names
			auto selectedEntity = labelView[labelIndex];
			std::vector<char *> componentLabels;
			std::vector<char> componentTypes;
			if (m_registry.any_of<LabelComponent>(selectedEntity)) { componentLabels.push_back("Label"); componentTypes.push_back('L'); }
			if (m_registry.any_of<TransformComponent>(selectedEntity)) {componentLabels.push_back("Transform"); componentTypes.push_back('T'); }
			if (m_registry.any_of<RenderComponent>(selectedEntity)) {componentLabels.push_back("Render"); componentTypes.push_back('R'); }
			if (m_registry.any_of<KeyboardComponent>(selectedEntity)) { componentLabels.push_back("Key Controller"); componentTypes.push_back('K'); }
			if (m_registry.any_of<AIControllerComponent>(selectedEntity)) {componentLabels.push_back("AI Controller"); componentTypes.push_back('A'); }

			ImGui::TextWrapped("Components:");
			static int componentIndex = 0;
			ImGui::SetNextItemWidth(-1);
			ImGui::ListBox("##Components", &componentIndex, componentLabels.data(), componentLabels.size(), componentLabels.size());

			// Component properties
			ImGui::TextWrapped("Properties:");
			ImGui::NewLine();
			
			char selectedComponentType = componentTypes[componentIndex];
			switch (selectedComponentType)
			{
			case 'L':
				ImGui::TextWrapped("No properties.");
				break;
			case 'T':
			{
				auto& transformComp = m_registry.get<TransformComponent>(selectedEntity);

				// Position
				static float pos[3];
				// Set pos from component
				pos[0] = transformComp.translation.x;
				pos[1] = transformComp.translation.y;
				pos[2] = transformComp.translation.z;

				ImGui::TextWrapped("Transform");
				ImGui::Text("X           Y           Z");
				ImGui::InputFloat3("Translation", pos, 2);
				// Set component from pos
				transformComp.translation = { pos[0],pos[1],pos[2] };


				// Rotation
				glm::quat rotation = transformComp.rotation;
				glm::vec3 eulerAngles = glm::eulerAngles(rotation);
				static float rot[3];
				// Set pos from component
				rot[0] = glm::degrees(eulerAngles.x);
				rot[1] = glm::degrees(eulerAngles.y);
				rot[2] = glm::degrees(eulerAngles.z);

				ImGui::InputFloat3("Rotation", rot, 2);
				// Set component from rot
				transformComp.rotation = glm::quat(glm::vec3(glm::radians(rot[0]), glm::radians(rot[1]), glm::radians(rot[2])));

				// Scale
				static float scl[3];
				// Set pos from component
				scl[0] = transformComp.scale.x;
				scl[1] = transformComp.scale.y;
				scl[2] = transformComp.scale.z;

				ImGui::InputFloat3("Scale", scl, 2);
				// Set component from scl
				transformComp.scale = { scl[0],scl[1],scl[2] };

				// Recalc model matrix
				transformComp.updateTransform();
			}
				break;
			case 'R':
			{
				// Retrieve the render component from the selectedEnitity in the registry.
				auto& renderComp = m_registry.get<RenderComponent>(selectedEntity);

				//////////////////// Colour ////////////////////
				static float rgb[3];
				// Set rgb from component
				rgb[0] = renderComp.rgb.x;
				rgb[1] = renderComp.rgb.y;
				rgb[2] = renderComp.rgb.z;
				// UI labels for the colour elements.
				ImGui::TextWrapped("Colour");
				ImGui::Text("R           G           B");
				// Colour edit UI element the value of which is set to rgb.
				ImGui::ColorEdit3("Colour", rgb, 2);
				// Set colour render component from rgb
				renderComp.rgb = { rgb[0], rgb[1], rgb[2] };

				ImGui::NewLine();

				//////////////////// Mesh Type ////////////////////
				// Set mesh type number from component
				int meshEnum = renderComp.meshNum;
				// The mesh types as strings for UI text.
				const char* meshTypeNames[] { "Cuboid", "Sphere", "Capsule" };
				// UI label for the mesh type radio button selection elements.
				ImGui::TextWrapped("Mesh Type");
				// Counter for which mesh type in the following for loop to add the option for as a radio button.
				int typeCount = 0; 
				// A for loop to add a radio button with a corresponding label for each of the specified mesh types in meshTypeNames.
				for (auto type : meshTypeNames)
				{
					ImGui::RadioButton(meshTypeNames[typeCount], &meshEnum, typeCount);
					ImGui::SameLine();
					typeCount++;
				}
				// Set mesh render component from meshEnum
				renderComp.meshNum = meshEnum;

				// Update mesh render
				renderComp.updateRender();
			}
				break;
			case 'K':	
			{
				ImGui::TextWrapped("Keyboard Bindings");
				ImGui::Text("Bound Key      Action");

				// Stores all bound keys' labels (char*) and the key they're bound to as a character
				std::vector<std::pair<char*, char>> boundKeys;
				boundKeys.reserve(m_numKeyBindings);
				// Sets all the key bindings from m_keyBindings
				for (int kCount = 0; kCount < m_numKeyBindings; kCount++)
				{
					if (boundKeys.size() < m_numKeyBindings)
					{
						boundKeys.push_back(std::pair<char*, char>());
						boundKeys[kCount] = std::pair<char*, char>(m_keyBindings[kCount].keyDesc, m_keyBindings[kCount].keyNum);
					}
				}

				// Text input boxes which allow for the input of 1 character (letter or number) which the described function/action is rebound to.
				for (int kCount = 0; kCount < m_numKeyBindings; kCount++)
				{
					ImGui::PushItemWidth(50);
					ImGui::InputText(boundKeys[kCount].first, &boundKeys[kCount].second, sizeof(char) * 2);

					// Only rebind the key if the input was a number or letter.
					if (isalpha(boundKeys[kCount].second) || isdigit(boundKeys[kCount].second))
					{
						// Rebind the key binding's number (key) to the input character as an integer in uppercase.
						m_keyBindings[kCount].keyNum = (int)toupper(boundKeys[kCount].second);
					}
				}
			}
				break;
			case 'A': // For higher marks allow way points to be edited here.
				ImGui::TextWrapped("No properties.");
				
				break;
			}
			
			ImGui::End();

			// Console output window -- use ImGuiHelper::writeToConsole to write to this console
			ImGui::Begin("Console");
			auto consoleText = ImGuiHelper::getConsoleString();
			ImGui::Text(consoleText);
			free(consoleText);
			ImGui::SetScrollHere(1.0f);
			ImGui::End();
			
			ImGuiHelper::end();

			m_application->updateWindow(elapsedTime);
			elapsedTime = 0.f;
		}
	}
}

void Editor::shutdown()
{
	// Stop IMGui
	ImGuiHelper::shutdown();

	// Stop the renderer
	SC::Renderer::shutdown();
}

void Editor::onEvent(SC::Event & e)
{
	SC::EventDispatcher dispatcher(e);
	dispatcher.dispatch<SC::WindowCloseEvent>(std::bind(&Editor::onClose, this, std::placeholders::_1));
	dispatcher.dispatch<SC::KeyPressedEvent>(std::bind(&Editor::onKeyPress, this, std::placeholders::_1));
}

bool Editor::onKeyPress(SC::KeyPressedEvent& e)
{
	int pressedKey = e.GetKeyCode();

	// Checks if the key pressed was any of the keys in the m_keyBindings list of keys to functions - then execute's the bound key's function if it is.
	for (auto key : m_keyBindings)
	{
		if (key.keyNum == pressedKey)
		{
			((void(*)())key.boundFunc)();
			return true;
		}
	}
}