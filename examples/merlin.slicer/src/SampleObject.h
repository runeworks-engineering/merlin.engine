#pragma once
#include "merlin.h"
#include <string>
#include <tinyxml2/tinyxml2.h>

using namespace Merlin;

struct SampleProperty {
    std::string name;
    std::string comment;
    float x_offset = 0;
    float y_offset = 0;
    float width = 30;
    float height = 10;
    float thickness = 4;
    float layer_height = 0.2;
    float line_width = 0.42;
    int tool_a = 0;
    int tool_b = 1;
    float flow_a = 1.0;
    float flow_b = 1.0;
    float retract_a = 0.8;
    float retract_b = 0.8;
    float feedrate_a = 600;
    float feedrate_b = 600;
    float temperature_a = 220;
    float temperature_b = 230;

    int overlap = 1;
    float overlap_flow_modifier = 1.0;


    bool use_purge_tower = false;
    bool use_alternate_sweep = true;
    bool use_in_to_out = true;
};

class SampleObject {
public:
	SampleObject(const SampleProperty& props);

	void renderMenu();
	const SampleProperty& getProperties() const { return props; }
	const Mesh_Ptr& getMeshA() const { return mesh_A; }
	const Mesh_Ptr& getMeshB() const { return mesh_B; }

    static SampleProperty fromXML(const tinyxml2::XMLElement* elem);
    std::string toXML() const;
	bool enabled = true;
private:
    SampleProperty props;
	Mesh_Ptr mesh_A;
	Mesh_Ptr mesh_B;

	bool show_toolpath = false;
	bool show_mesh = true;
	bool show_tool = true;
	bool show_purge_tower = false;
};
