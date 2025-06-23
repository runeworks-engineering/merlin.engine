#pragma once
#include <string>
#include <vector>
#include "merlin.h"

struct Tool {
    int id;
    float flowrate;
    float retract_length;
    float feedrate;
    float temperature;
};

struct ToolPath {
    glm::vec4 start = glm::vec4(0); // x, y, z, e;
    glm::vec4 end = glm::vec4(0);   // x, y, z, e;
    glm::vec4 meta = glm::vec4(0);  // feed, T°, flow, tool
    glm::vec4 meta_bis = glm::vec4(0);  // lauer, null, null, null
};

struct Sample {
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

class Slicer {
public:
    Slicer();
    void clear();
    void slice();
    void load_macro(const std::string& start_path, const std::string& end_path);
    void export_gcode(const std::string& filename);
    void process();
    void postprocess();

    int getLayer() const;

    void generateSample(Sample props);
    inline std::vector<ToolPath>& getToolPath() { return toolpath; };
private:
    void comment(const std::string& comment);

    float compute_e(float length, float flow, float line_width, float layer_height) const;
    std::string time() const;

    void retract(float length, float feedrate = -1);
    void extrude(float length, float feedrate = -1);

    void move(glm::vec4 destination, int mode = 1, float feedrate = -1);
    void move_Z(float destination, int mode = 1, float feedrate = -1);
    void moveXY(glm::vec2 destination, int mode = 1, float feedrate = -1);
    void moveXYE(glm::vec2 destination, float e, int mode = 1, float feedrate = -1);

    void tool_change(Tool tool);
    void new_layer(float z);
    void brush();

    ToolPath gen_toolpath(const glm::vec4& start, const glm::vec4& end, const Tool& tool, float feedrate = -1);

    void add_gcode(ToolPath tp, const std::string& command = "G1");
    void add_gcode(const std::string& cmd);


private:
    int numLayers;
    int m_current_layer = 0;
    float filament_diameter;
    float actual_max_z = 0;
    float m_cumulative_E;
    float m_current_feedrate;
    const Tool noTool;
    Tool m_last_tool = noTool;
    Tool m_active_tool = noTool;

    bool use_relative_extrusion = true;

    std::vector<std::string> start_gcode;
    std::vector<std::string> end_gcode;
    std::vector<std::string> gcode;
    std::vector<std::string> post_processed_gcode;

    std::vector<ToolPath> toolpath;
    glm::vec4 m_current_position; // x, y, z, e
};

