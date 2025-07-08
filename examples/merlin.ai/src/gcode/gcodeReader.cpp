#include "GcodeReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "merlin/core/console.h"

using namespace Merlin;

GcodeSimulator::GcodeSimulator() : m_current_position(0.0f), m_current_target(0.0f) {
    m_current_position = glm::vec4(150, 100, 0, 0);
}

void GcodeSimulator::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    
    std::string line;
   
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return;
    }

    while (getline(file, line)) {
        partGCodeLine(line);
    }

    file.close();
    if (!m_commands.empty()) {
        m_current_target = m_commands[0].position; // Set the first target
        m_current_speed = m_commands[0].speed; // Set the first target
    }
}

void GcodeSimulator::readGCode(const std::string& gcode) {
    std::stringstream ss(gcode);
    std::string line;
    if (gcode.empty()) return;

    while (getline(ss, line)) {
        partGCodeLine(line);
    }

    if (!m_commands.empty()) {
        m_current_target = m_commands[0].position; // Set the first target
        m_current_speed = m_commands[0].speed; // Set the first target
    }
}

void GcodeSimulator::partGCodeLine(const std::string& line){
    if (line[0] == 'G') {
        std::istringstream iss(line.substr(1));
        int commandNum;
        iss >> commandNum;
        static Command command = { glm::vec4(0), 0 };

        if (commandNum == 1) {  // We handle G1 commands

            char coord;
            float value;
            while (iss >> coord >> value) {
                switch (coord) {
                case 'X': command.position.x = value * 10; break;
                case 'Y': command.position.y = value * 10; break;
                case 'Z': command.position.z = value * 10; break;
                case 'E':
                    if (m_extrusion_absolute) command.position.w = value;
                    else command.position.w += value;
                    break;
                case 'F': command.speed = value; break;
                    break;
                }
            }
            m_commands.push_back(command);
        }
        else if (commandNum == 92) {  // We handle G1 commands
            char coord;
            float value;
            while (iss >> coord >> value) {
                switch (coord) {
                case 'E': command.position.w = 5 /*value * 10.0*/; break;
                }
            }
            m_commands.push_back(command);
        }
    }
    else if (line[0] == 'M') {
        std::istringstream iss(line.substr(1));
        int commandNum;
        iss >> commandNum;
        static Command command = { glm::vec4(0), 0 };
        if (commandNum == 82) {  // Absolute E
            m_extrusion_absolute = true;
        }
        else if (commandNum == 83) {  // Relative E
            m_extrusion_absolute = false;
        }
    }
}

void GcodeSimulator::reset() {
    m_extrusion_absolute = false;
    m_current_extrusion = 0;
    m_current_position = glm::vec4(-m_origin_offset, 0);
    m_current_position = glm::vec4(0);
    if (!m_commands.empty()) {
        m_current_target = m_commands[0].position; // Set the first target
        m_current_speed = m_commands[0].speed; // Set the first target
        currentIndex = 0;
    }
}

void GcodeSimulator::update(float dt) {
    glm::vec4 delta = m_current_target - m_current_position;
    glm::vec3 xyz_delta = glm::vec3(delta);
    float e_delta = delta.w;

    bool xyz_done = glm::length(xyz_delta) < 0.01f;
    bool e_done = fabs(e_delta) < 0.0001f;

    float move_distance = glm::length(xyz_delta);
    float extrude_distance = fabs(e_delta);

    // If both position and extrusion are done, advance
    if (xyz_done && e_done) {
        m_current_position = m_current_target;
        if (currentIndex < m_commands.size() - 1) {
            currentIndex++;
            m_current_target = m_commands[currentIndex].position;
            m_current_speed = m_commands[currentIndex].speed;
        }
    }
    else {
        // Move and extrude proportionally
        glm::vec4 movement(0.0f);
        if (!xyz_done && move_distance > 0) {
            glm::vec3 dir = glm::normalize(xyz_delta);
            glm::vec3 move = dir * m_current_speed * dt;
            if (glm::length(move) > move_distance) move = xyz_delta;
            movement = glm::vec4(move, 0.0f);
        }
        if (!e_done && extrude_distance > 0) {
            // Use same speed or define an extrusion speed?
            float extrude_amount = m_current_speed * dt;
            if (fabs(extrude_amount) > extrude_distance) extrude_amount = e_delta;
            movement.w = (e_delta > 0 ? 1 : -1) * fabs(extrude_amount);
        }
        m_current_position += movement;
    }
}

glm::vec3 GcodeSimulator::getNozzlePosition() {
    return glm::vec3(m_current_position) + m_origin_offset;
}

float GcodeSimulator::getExtruderDistance() {
    if (m_commands[currentIndex].position.w != 0) return m_commands[currentIndex].position.w * flow_override;
    return 0.0;
}

bool GcodeSimulator::lastCommandReached(){
	if (currentIndex >= m_commands.size() - 1)
		return true;
    else
		return false;
}
