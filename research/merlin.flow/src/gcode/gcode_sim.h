#pragma once
#include <vector>
#include <string>
#include <glm/glm.hpp>

struct Command {
    glm::vec3 position;
    float speed;
    float flow;
};

class GCodeSimulator{
public:
    GCodeSimulator();
    void readFile(const std::string& filepath);
    void readGCode(const std::string& gcode);
    void partGCodeLine(const std::string& line, Command& command);

    inline void move(const glm::vec3& position, float flow, float speed = 20) {
        m_commands.push_back({position, speed, flow});
        m_commands_str.push_back("CUSTOM_COMMAND"); // Placeholder for string representation
    }

    void reset();
    void update(float dt);

    void setNozzlePosition(const glm::vec3& pos);
    glm::vec3 getNozzlePosition();
    glm::vec3 getNozzleVelocity();
    float getFlowrate();
	bool lastCommandReached();
    void clear();
private:
    std::vector<Command> m_commands;
    std::vector<std::string> m_commands_str;
    size_t currentIndex = 0;

    float m_current_speed = 0;
    float m_current_flowrate = 0;
	
    glm::vec3 m_current_position = glm::vec3(0);
    glm::vec3 m_current_velocity = glm::vec3(0);
    glm::vec3 m_current_target = glm::vec3(0);
    glm::vec3 m_origin_offset = glm::vec3(0);
};

