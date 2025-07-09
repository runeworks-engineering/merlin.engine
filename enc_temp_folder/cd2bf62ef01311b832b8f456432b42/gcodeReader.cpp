#include "GcodeReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "merlin/core/console.h"
#include "../settings.h"

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
    reset();
    m_commands.clear();
    m_commands_str.clear();

    Command command = { glm::vec4(0),0 };
    while (getline(file, line)) {
        partGCodeLine(line, command);
    }

    file.close();
    reset();
}

void GcodeSimulator::readGCode(const std::string& gcode) {
    std::stringstream ss(gcode);
    std::string line;
    if (gcode.empty()) return;
    reset();
    m_commands.clear();
    m_commands_str.clear();

    Command command = { glm::vec4(0),0 };
    while (getline(ss, line)) {
        partGCodeLine(line, command);
    }

    reset();
}

void GcodeSimulator::partGCodeLine(const std::string& line, Command& command){
    if (line[0] == 'G') {
        std::istringstream iss(line.substr(1));
        int commandNum;
        iss >> commandNum;
        
        if (commandNum == 1 || commandNum == 0) {  // We handle G1 commands

            char coord;
            float value;
            while (iss >> coord >> value) {
                switch (coord) {
<<<<<<< HEAD
                case 'X': command.position.x = value; break;
                case 'Y': command.position.y = value; break;
                case 'Z': command.position.z = value; break;
=======
                case 'X': command.position.x = value ; break;
                case 'Y': command.position.y = value ; break;
                case 'Z': command.position.z = value ; break;
>>>>>>> c545464c7d3d1a390f122e966bd1fb943399c50a
                case 'E':
                    if (m_extrusion_absolute) cumulative_extrusion = value;
                    else cumulative_extrusion += value;
                    command.position.w = cumulative_extrusion;
                    break;
                case 'F': command.speed = value; break;
                    break;
                }
            }
            m_commands.push_back(command);
            m_commands_str.push_back(line);
        }
        else if (commandNum == 92) {  // We handle G1 commands
            char coord;
            float value;
            while (iss >> coord >> value) {
                switch (coord) {
                case 'E':
                    cumulative_extrusion = 0; 
                    command.position.w = 0;
                    break;
                }
            }
            m_commands.push_back(command);
            m_commands_str.push_back(line);
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
    m_current_position = glm::vec4(0);
    m_current_target = glm::vec4(0);
    m_current_velocity = glm::vec4(0);
    distanceToExtrude = 0;
    currentIndex = 0;
    cumulative_extrusion = 0;
    if (!m_commands.empty()) {
        m_current_target = m_commands[0].position; // Set the first target
        m_current_speed = m_commands[0].speed; // Set the first target
        currentIndex = 0;
        Console::info("GcodeSimulator") << "Executing command :" << m_commands_str[0] << Console::endl;
    }
}

void GcodeSimulator::resetVolume(){
    distanceToExtrude = 0;
}

void GcodeSimulator::update(float dt) {
    float flow_override = settings.flow_override;
    if (glm::isnan(flow_override)) flow_override = 0;

    glm::vec4 delta = m_current_target - m_current_position;
    glm::vec3 xyz_delta = glm::vec3(delta);
    float e_delta = delta.w * flow_override;
    
    bool xyz_done = glm::length(xyz_delta) < 0.01f;
    bool e_done = fabs(e_delta) < 0.001f;

    float move_distance = glm::length(xyz_delta);
    float extrude_distance = fabs(e_delta);

    // If both position and extrusion are done, advance
    if (xyz_done && e_done) {
        m_current_position = m_current_target;

        if (currentIndex < m_commands.size() - 1) {
            currentIndex++;
            Console::info("GcodeSimulator") << "Current Position :" << m_current_position << Console::endl;
            Console::info("GcodeSimulator") << "Executing command :" << m_commands_str[currentIndex] << Console::endl;
            m_current_target = m_commands[currentIndex].position;
            m_current_speed = m_commands[currentIndex].speed;
            Console::info("GcodeSimulator") << "Current Target :" << m_current_target << Console::endl;
        }
    }
    else {
        // Coordinated move: both XYZ and E finish at the same time
        float total_distance = move_distance;
        if (total_distance < 1e-6f) total_distance = extrude_distance;
        if (total_distance < 1e-6f) total_distance = 1e-6f; // avoid div by zero
        float move_time = total_distance / m_current_speed;
        float step = dt / move_time;
        if (step > 1.0f) step = 1.0f;

        glm::vec4 movement(0.0f);
        if (!xyz_done && move_distance > 0) {
            movement += glm::vec4(xyz_delta * step, 0.0f);
        }
        if (!e_done) {
            movement.w += e_delta * step;
        }
        m_current_position += movement;
        distanceToExtrude += 50 * dt * flow_override;
    }
}

glm::vec3 GcodeSimulator::getNozzlePosition() {
    return glm::vec3(m_current_position) + m_origin_offset;
}

float GcodeSimulator::getExtruderDistance() {
    return distanceToExtrude;
}

float GcodeSimulator::getVolumeToExtrude() { //0.5 penalty that AI is fighting
    return distanceToExtrude * 3.1415926*(17.5 * 17.5)/4.0;
}

bool GcodeSimulator::lastCommandReached(){
	if (currentIndex >= m_commands.size() - 1)
		return true;
    else
		return false;
}
