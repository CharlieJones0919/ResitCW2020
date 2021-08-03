#include "editor.h"
#include "ImGuiHelper.h"
#include <map>

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
	m_registry.emplace<TransformComponent>(m_entities.back(), glm::vec3(0.f, 0.5f, 0.f), glm::vec3(0.f), glm::vec3(1.f));
	m_registry.emplace<RenderComponent>(m_entities.back(), MeshType::Cuboid, glm::vec3(0.f, 0.f, 1.f));
	m_registry.emplace<KeyboardComponent>(m_entities.back());
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
			if (m_registry.any_of<TransformComponent>(selectedEntity)) {componentLabels.push_back("Transform"); componentTypes.push_back('T');}
			if (m_registry.any_of<RenderComponent>(selectedEntity)) {componentLabels.push_back("Render"); componentTypes.push_back('R');}
			if (m_registry.any_of<KeyboardComponent>(selectedEntity)) {componentLabels.push_back("Key Controller"); componentTypes.push_back('K');}
			if (m_registry.any_of<AIControllerComponent>(selectedEntity)) {componentLabels.push_back("AI Controller"); componentTypes.push_back('A');}

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
				//ImGui::TextWrapped("ADD STUFF HERE");
				/*  TODO:
					Based on the code above you should implement ImGui controls to be able to change the render component mesh type and colour.
					Mesh type should be selected using an ImGui::RadioButton
					Colour should be changed using an ImGui::ColorEdit3
					Look in the ImGui::ShowDemoWindow() code for help on how to use these controls.
				*/

				auto& renderComp = m_registry.get<RenderComponent>(selectedEntity);

				//////////////////// Colour ////////////////////
				static float rgb[3];
				// Set rgb from component
				rgb[0] = renderComp.rgb.x;
				rgb[1] = renderComp.rgb.y;
				rgb[2] = renderComp.rgb.z;
				ImGui::TextWrapped("Colour");
				ImGui::Text("R           G           B");
				ImGui::ColorEdit3("Colour", rgb, 2);
				// Set colour render component from rgb
				renderComp.rgb = { rgb[0], rgb[1], rgb[2] };

				//////////////////// Mesh Type ////////////////////
				// Set mesh type number from component
				int meshEnum = renderComp.meshNum;
				// The mesh types as strings for UI text.
				const char* meshTypeNames[] { "Cuboid", "Sphere", "Capsule" };
				ImGui::TextWrapped("Mesh Type");
				// Counter for which mesh type in the following for loop to add the option for as a radio button.
				int typeCount = 0; 
				// A for loop to add a radio button with a corresponding label for each of the specified mesh types in meshTypeNames.
				for (auto type : meshTypeNames)
				{
					ImGui::RadioButton(meshTypeNames[typeCount], &meshEnum, typeCount);
					typeCount++;
				}
				// Set mesh render component from meshEnum
				renderComp.meshNum = meshEnum;

				// Update mesh render
				renderComp.updateRender();
			}
				break;
			case 'K':
				ImGui::TextWrapped("No properties.");
				// For higher marks perform key mapping here.
				break;
			case 'A':
				ImGui::TextWrapped("No properties.");
				// For higher marks allow way points to be edited here.
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

bool Editor::onKeyPress(SC::KeyPressedEvent & e)
{
	switch (e.GetKeyCode())
	{
	case SC_KEY_W:
		SC::Renderer::cameraForward(); return true;
		break;
	case SC_KEY_S:
		SC::Renderer::cameraBack(); return true;
		break;
	case SC_KEY_A:
		SC::Renderer::cameraLeft(); return true;
	break;
	case SC_KEY_D:
		SC::Renderer::cameraRight(); return true;
		break;
	case SC_KEY_Q:
		SC::Renderer::cameraUp(); return true;
		break;
	case SC_KEY_E:
		SC::Renderer::cameraDown(); return true;
		break;
	}
	return false;
}
