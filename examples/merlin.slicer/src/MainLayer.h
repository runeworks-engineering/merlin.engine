#pragma once

#include <Merlin.h>
#include "Slicer.h"

using namespace Merlin;

namespace Settings {
	constexpr float nozzle_diameter = 0.4f;
	constexpr float layer_height = 0.2f;
	constexpr float interface_overlap = 1.0f;
	constexpr float filament_diameter = 1.75f;
	constexpr float full_flowrate = 1.0f;
	constexpr float interface_flowrate = 0.5f;
	constexpr int tool_A{ 0 }, tool_B{ 1 };
	constexpr float feedrate(1800.0);

	float computeExtrusion(float length, float lineWidth, float height, float flowMultiplier) {
		float area = lineWidth * height;
		float volume = area * length * flowMultiplier;
		float filArea = static_cast<float>(glm::pi<float>()) * std::pow(filament_diameter * 0.5f, 2);
		return volume / filArea;
	}
}


class MainLayer : public Layer3D {
public:
	MainLayer();
	virtual ~MainLayer();

	void createScene();
	void slice();


	virtual void onAttach() override;
	virtual void onUpdate(Timestep ts) override;
	virtual void onImGuiRender() override;
private:

	Slicer slicer;

	Mesh_Ptr sample_A;
	Mesh_Ptr sample_B;
	Mesh_Ptr interface;


	Scene_Ptr scene;
	Renderer renderer;
};