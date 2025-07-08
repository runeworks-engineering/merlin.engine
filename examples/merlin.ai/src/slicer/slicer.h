#pragma once
#include <string>
#include <vector>
#include "SampleObject.h"
#include "merlin.h"

struct Tool {
    int id;
    float flowrate;
    float retract_length;
    float feedrate;
};

struct ToolPath {
    alignas(16) glm::vec4 start = glm::vec4(0); // x, y, z, e;
    alignas(16) glm::vec4 end = glm::vec4(0);   // x, y, z, e;
    alignas(16) glm::vec4 meta = glm::vec4(0);  // feed, T°, flow, tool
    alignas(16) glm::vec4 meta_bis = glm::vec4(0);  // layer, type, null, null
};

class Slicer {
public:
    Slicer();
    void clear();
    void slice();
    void load_macro(const std::string& start_path, const std::string& end_path);
    void export_gcode(const std::string& filename);
    const std::vector<std::string>& get_gcode();
    void process();
    void postprocess();

    int getLayer() const;
    int getLayerCount() const;

    void generateSample(SampleProperty props);


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

    ToolPath gen_toolpath(const glm::vec4& start, const glm::vec4& end, const Tool& tool, float feedrate = -1, int mode = 1);

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

