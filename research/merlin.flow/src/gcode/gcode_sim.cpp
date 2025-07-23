#include "gcode_sim.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "merlin/core/console.h"
#include "../settings.h"

using namespace Merlin;

GCodeSimulator::GCodeSimulator() : m_current_position(0.0f), m_current_target(0.0f) {
    m_current_position = glm::vec3(0);
    m_current_velocity = glm::vec3(0);
    m_current_flowrate = 0.0f;
    m_current_speed = 0.0f;
}

void GCodeSimulator::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return;
    }

    reset();
    m_commands.clear();
    m_commands_str.clear();

    Command command = { glm::vec3(0), 0 };
    while (getline(file, line)) {
        partGCodeLine(line, command);
    }

    file.close();
    reset();
}

void GCodeSimulator::readGCode(const std::string& gcode) {
    std::stringstream ss(gcode);
    std::string line;
    if (gcode.empty()) return;
    reset();
    m_commands.clear();
    m_commands_str.clear();

    Command command = { glm::vec3(0), 0 };
    while (getline(ss, line)) {
        partGCodeLine(line, command);
    }

    reset();
}

void GCodeSimulator::partGCodeLine(const std::string& line, Command& command) {
    if (line[0] == 'G') {
        std::istringstream iss(line.substr(1));
        int commandNum;
        iss >> commandNum;

        if (commandNum == 1 || commandNum == 0) {
            char coord;
            float value;
            while (iss >> coord >> value) {
                switch (coord) {
                case 'X': command.position.x = value; break;
                case 'Y': command.position.y = value; break;
                case 'Z': command.position.z = value; break;
                case 'E': command.flow = value; break;
                case 'F': command.speed = value; break;
                }
            }
            m_commands.push_back(command);
            m_commands_str.push_back(line);
        }
        // Ignore G92 E resets now
    }
    // Ignore M82/M83 (absolute/relative E)
}

void GCodeSimulator::reset() {
    m_current_position = glm::vec3(0);
    m_current_target = glm::vec3(0);
    m_current_velocity = glm::vec3(0);
	m_current_flowrate = 0.0f;
    currentIndex = 0;

    if (!m_commands.empty()) {
        m_current_target = m_commands[0].position;
        m_current_speed = m_commands[0].speed;
        m_current_flowrate = m_commands[0].flow;
    }
}

void GCodeSimulator::update(float dt) {
    if (m_commands.empty()) return;
    if (currentIndex >= m_commands.size()) return;
    // Compute the 2D delta to the current target
    glm::vec3 delta = m_current_target - m_current_position;
    float dist = glm::length(delta);
    bool xyz_done = dist < 0.01f;  // within 0.01 mm tolerance

    /*
    Console::info() << "Current Position: " 
                    << m_current_position.x << ", " 
                    << m_current_position.y << ", " 
                    << m_current_position.z << Console::endl;

    Console::info() << "Current Target: "
                    << m_current_target.x << ", "
                    << m_current_target.y << ", "
                    << m_current_target.z << Console::endl;
    */

    if (xyz_done) {
        // Snap exactly to target
        m_current_position = m_current_target;
        // Zero out velocity when stopped
        m_current_velocity = glm::vec3(0.0f);

        // Advance to next command if available
        if (currentIndex < m_commands.size()) {
            currentIndex++;
            if (currentIndex >= m_commands.size()) return;
            // Load new target, speed, and flowrate
            m_current_target = m_commands[currentIndex].position;   // vec3
            m_current_speed = m_commands[currentIndex].speed;      // mm/s
            m_current_flowrate = m_commands[currentIndex].flow;   // unit/s
        }
    }
    else {
        // Compute proportional step along XY
        float move_time = dist / std::max(m_current_speed, 1e-6f);
        float alpha = std::min(dt / move_time, 1.0f);
        glm::vec3 movement = delta * alpha;

        // Update position & velocity
        m_current_position += movement;
        m_current_velocity = movement / dt;
    }

    // Always update flowrate to the current command’s value
    m_current_flowrate = m_commands[currentIndex].flow;
}

void GCodeSimulator::clear() {
    m_commands.clear();
    m_commands_str.clear();
    reset();
}

bool GCodeSimulator::lastCommandReached() {
    return currentIndex >= m_commands.size();
}

glm::vec3 GCodeSimulator::getNozzleVelocity() {
    return glm::vec3(m_current_velocity);
}

float GCodeSimulator::getFlowrate(){
    return m_current_flowrate;
}

glm::vec3 GCodeSimulator::getNozzlePosition() {
    return glm::vec3(m_current_position);
}

void GCodeSimulator::setNozzlePosition(const glm::vec3& pos) {
    m_current_position = pos;
}
