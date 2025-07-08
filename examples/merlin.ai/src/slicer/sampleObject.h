#pragma once
#include "merlin.h"
#include <string>
#include <tinyxml2/tinyxml2.h>

using namespace Merlin;

struct SampleProperty {
    std::string name;
    std::string comment;
    float x_position = 0;
    float y_position = 0;
    float rotation_z = 0;

    float width = 10;
    float length = 20;
    float height = 0.15;

    float layer_height = 0.15;
    float line_width = 0.72;
    float flow = 1.0;
    float retract = 1.0;

    float feedrate = 600;

    int perimeter = 0;
};

class SampleObject {
public:
	SampleObject();
	SampleObject(const SampleProperty& props);

	void renderMenu();
	const SampleProperty& getProperties() const { return props; }
	const Mesh_Ptr& getMesh() const { return mesh; }

    static SampleProperty fromXML(const tinyxml2::XMLElement* elem);
    std::string toXML() const;
	bool enabled = true;
private:
    void generateMesh();

    SampleProperty props;
	Mesh_Ptr mesh;

	bool show_toolpath = false;
	bool show_mesh = true;
	bool show_tool = true;
	bool show_purge_tower = false;
};
