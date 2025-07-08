#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct Command {
    glm::vec4 position;
    float speed;
};

class GcodeSimulator{
public:
    GcodeSimulator();
    void readFile(const std::string& filepath);
    void readGCode(const std::string& gcode);
    void partGCodeLine(const std::string& line);

    void reset();
    void update(float dt);

    glm::vec3 getNozzlePosition();
    float getExtruderDistance();
	bool lastCommandReached();

    float flow_override = 1.0;

private:
    std::vector<Command> m_commands;
    size_t currentIndex = 0;
    float m_current_speed = 0;
    bool m_extrusion_absolute = false;
    float m_current_extrusion = 0;
	
    glm::vec4 m_current_position;
    glm::vec4 m_current_velocity = glm::vec4(0);
    glm::vec4 m_current_target;
    glm::vec3 m_origin_offset = glm::vec3(0,0,5);
};

