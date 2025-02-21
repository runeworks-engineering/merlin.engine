#pragma once

#include <Merlin.h>
using namespace Merlin;

class ExampleLayer : public Layer3D
{
public:
	ExampleLayer();
	virtual ~ExampleLayer();

	virtual void onAttach() override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImGuiRender() override;
private:

	Scene scene;
	//Renderer renderer;
	Renderer renderer;
	PhysicsSolver solver;

	shared<PointLight>  light;
};