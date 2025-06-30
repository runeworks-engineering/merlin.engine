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

    float radius = 6;
    float length = 20;
    float height = 0.15;
    float resolution = 0.01;

    float layer_height = 0.15;
    float line_width = 0.72;
    int tool = 15;
    float flow = 1.0;

    float retract = 0.8;
    float feedrate = 600;

    int sample_type = 0;
    bool use_outline = false;
};

class SampleObject {
public:
	SampleObject(const SampleProperty& props);

	void renderMenu();
	const SampleProperty& getProperties() const { return props; }
	const Model_Ptr& getModel() const { return model; }

    static SampleProperty fromXML(const tinyxml2::XMLElement* elem);
    std::string toXML() const;
	bool enabled = true;
private:
    SampleProperty props;
	Model_Ptr model;

	bool show_toolpath = false;
	bool show_mesh = true;
	bool show_tool = true;
	bool show_purge_tower = false;
};
