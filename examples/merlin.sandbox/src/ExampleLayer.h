#pragma once

#include <Merlin.h>
using namespace Merlin;

class ExampleLayer : public Layer3D
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	void createScene();
	void createPhysics();

	virtual void onAttach() override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImGuiRender() override;
private:

	Scene_Ptr scene;
	PhysicsSolver3D solver;

	Mesh_Ptr fluid; //vizualization
	Mesh_Ptr fluid_boundingbox;
	Mesh_Ptr domain;

	Renderer renderer;
};