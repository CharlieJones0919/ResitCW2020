#pragma once
#include <glm/glm.hpp>

enum class MeshType {Cuboid = 0, Sphere = 1, Capsule = 2}; 

class RenderComponent
{
public:
	int meshNum = 0; //!< Which number mesh type from the enum list the mesh should be rendered as.
	glm::vec3 rgb = { 1.f, 1.f, 1.f }; //!< RGB colour values of the rendered mesh.

	//! Default constructor which sets the render component to the default mesh type and colour.
	RenderComponent() : m_meshType(MeshType(meshNum)), m_colour(rgb) {  }; 	

	//! Construtor which takes a mesh type and colour
	RenderComponent(MeshType type, glm::vec3 colour) : m_meshType(type), m_colour(colour) 
	{
		// Set the meshNum and rgb values to the according newly set colour and meshType so when the entities' render GUI elements are first selected their colours aren't set to white by default.
		meshNum = static_cast<int>(m_meshType);
		rgb = m_colour;
	}; 
	
	MeshType& getMeshType() { return m_meshType; }
	glm::vec3& getColour() { return m_colour; }

	void updateRender()
	{ 
		m_meshType = MeshType(meshNum);
		m_colour = rgb;
	}

private:
	MeshType m_meshType; //!< Internal storage for mesh type
	glm::vec3 m_colour; //!< Tint to be applied to the mesh
};