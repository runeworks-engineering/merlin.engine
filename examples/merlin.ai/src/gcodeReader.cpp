#include "GcodeReader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "merlin/core/console.h"

using namespace Merlin;

GcodeSimulator::GcodeSimulator() : m_current_position(0.0f), m_current_target(0.0f) {
    m_current_position = glm::vec4(150, 100, 0, 0);
    //m_commands.push_back({ glm::vec4(0, 0, 10, 0) - glm::vec4(m_origin_offset,0), 10 });
    /*
    m_commands.push_back({ glm::vec4(-50, 0, 4, 200) - glm::vec4(m_origin_offset,0), 300 });
    m_commands.push_back({ glm::vec4(-40, 0, 4, 200) - glm::vec4(m_origin_offset,0), 100 });
    m_commands.push_back({ glm::vec4(40, 0, 4, 200) - glm::vec4(m_origin_offset,0), 300 });
    m_commands.push_back({ glm::vec4(40, 0, 15, 0) - glm::vec4(m_origin_offset,0), 100 });
    /**/


    /*
    m_commands.push_back({ glm::vec4(-50, 0, 4, 200) - glm::vec4(m_origin_offset,0), 300 });
    m_commands.push_back({ glm::vec4(-40, 0, 4, 200) - glm::vec4(m_origin_offset,0), 100 });
    m_commands.push_back({ glm::vec4(40, 0, 4, 200) - glm::vec4(m_origin_offset,0), 300 });
    m_commands.push_back({ glm::vec4(40, 8, 4, 200) - glm::vec4(m_origin_offset,0), 300 });
    m_commands.push_back({ glm::vec4(-40, 8, 4, 200) - glm::vec4(m_origin_offset,0), 300 });
    m_commands.push_back({ glm::vec4(-40, 16, 4, 200) - glm::vec4(m_origin_offset,0), 300 });
    m_commands.push_back({ glm::vec4(40, 16, 4, 200) - glm::vec4(m_origin_offset,0), 300 });
    m_commands.push_back({ glm::vec4(40, 16, 20, 0) - glm::vec4(m_origin_offset,0), 100 });
    /**/



    /*
    m_commands.push_back({ glm::vec4(0, 0, 20, 0) - glm::vec4(m_origin_offset,0), 1000 });
    m_commands.push_back({ glm::vec4(0, 0, 19, 500) - glm::vec4(m_origin_offset,0), 10 });
    /**/



    /**/
    float radius = 20.0f;
    float height = 20.0f;
    int num_turns = 5;
    int segments_per_turn = 100;
    m_commands.push_back({ glm::vec4(radius, 0, 2, 0) - glm::vec4(m_origin_offset, 0) + glm::vec4(150,100,0,0), 200});
    for (int i = 0; i <= num_turns * segments_per_turn; ++i) {
        float t = static_cast<float>(i) / segments_per_turn;
        float angle = t * 2.0f * glm::pi<float>();
        float x = radius * cos(angle);
        float y = radius * sin(angle);
        float z = 2+ (height * t) / num_turns;

        float extrusion = 100; // constant extrusion per segment (could be dynamic)
        float speed = 200.0f;

        glm::vec4 position = glm::vec4(x, y, z, extrusion);
        m_commands.push_back({ position - glm::vec4(m_origin_offset, 0) + glm::vec4(150,100,0,0), speed });
    }
    m_commands.push_back({ glm::vec4(radius, 0, 30, 500) - glm::vec4(m_origin_offset, 0) + glm::vec4(150,100,0,0), 200 });
    /**/




    /*
	float speed = 500.0f;
    float layer = 2;
    float Xline = 20;
    float Xwidth = 10;

    m_commands.push_back({ glm::vec4(-Xline - Xwidth * 0.5, -20, 4, 2000) - glm::vec4(m_origin_offset,0), speed * 2 });

    for (int i = 0; i < 5; i++) {
        m_commands.push_back({ glm::vec4(-Xline - Xwidth* 0.5, -20, 4 + layer * i, 120) - glm::vec4(m_origin_offset,0), speed });
        m_commands.push_back({ glm::vec4(-Xline - Xwidth* 0.5,  20, 4 + layer * i, 100) - glm::vec4(m_origin_offset,0), speed });
        m_commands.push_back({ glm::vec4(-Xline + Xwidth* 0.5,  20, 4 + layer * i, 100) - glm::vec4(m_origin_offset,0), speed });
        m_commands.push_back({ glm::vec4(-Xline + Xwidth* 0.5, -20, 4 + layer * i, 50) - glm::vec4(m_origin_offset,0), speed });
        m_commands.push_back({ glm::vec4(-Xline + Xwidth* 0.5, -20, 4 + layer * (i+1), 0) - glm::vec4(m_origin_offset,0), speed });
    }

    m_commands.push_back({ glm::vec4(Xline - Xwidth * 0.5, -20, 4, 0) - glm::vec4(m_origin_offset,0), speed*2 });

    for (int i = 0; i < 5; i++) {
        m_commands.push_back({ glm::vec4(Xline - Xwidth * 0.5, -20, 4 + layer * i, 100) - glm::vec4(m_origin_offset,0), speed });
        m_commands.push_back({ glm::vec4(Xline - Xwidth * 0.5, 20, 4 + layer * i, 100) - glm::vec4(m_origin_offset,0), speed });
        m_commands.push_back({ glm::vec4(Xline + Xwidth * 0.5, 20, 4 + layer * i, 100) - glm::vec4(m_origin_offset,0), speed });
        m_commands.push_back({ glm::vec4(Xline + Xwidth * 0.5, -20, 4 + layer * i, 50) - glm::vec4(m_origin_offset,0), speed });
        m_commands.push_back({ glm::vec4(Xline + Xwidth * 0.5, -20, 4 + layer * (i + 1), 0) - glm::vec4(m_origin_offset,0), speed });
    }
    speed = 600.0f;
    for (int i = 0; i < 6; i++) {
        m_commands.push_back({ glm::vec4(Xline, -20 + layer *i, 20, 60) - glm::vec4(m_origin_offset,0), speed });
        m_commands.push_back({ glm::vec4(-Xline, -20 + layer * i, 20, 60) - glm::vec4(m_origin_offset,0), speed });
    }

    m_commands.push_back({ glm::vec4(40, 0, 4, 0) - glm::vec4(m_origin_offset,0), 500 });
    */
}

void GcodeSimulator::readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    std::string line;

    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filepath << std::endl;
        return;
    }
    float prevE = 0;
    while (getline(file, line)) {
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
                    case 'X': command.position.x = value*10; break;
                    case 'Y': command.position.y = value*10; break;
                    case 'Z': command.position.z = value*10; break;
                    case 'E': 
                        command.position.w = 100; 
                        //command.position.w = value*2000 - prevE; 
						prevE = value * 2000;
                        break;
                    case 'F': command.speed = 300; break;
                    }
                }
                m_commands.push_back(command);
            } else if (commandNum == 92) {  // We handle G1 commands
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
    }

    file.close();
    if (!m_commands.empty()) {
        m_current_target = m_commands[0].position; // Set the first target
        m_current_speed = m_commands[0].speed; // Set the first target
    }
}

void GcodeSimulator::reset() {
    m_current_position = glm::vec4(-m_origin_offset, 0);
    m_current_position = glm::vec4(0);
    if (!m_commands.empty()) {
        m_current_target = m_commands[0].position; // Set the first target
        m_current_speed = m_commands[0].speed; // Set the first target
        currentIndex = 0;
    }
}

//void GcodeSimulator::update(float dt) {
//    glm::vec3 delta = glm::vec3(m_current_target) - glm::vec3(m_current_position);
//    glm::vec3 direction = glm::normalize(delta);
//    glm::vec3 movement = direction * m_current_speed * dt;
//
//    if (glm::length(glm::vec3(delta)) < 0.1) {
//        m_current_position = m_current_target;
//        // Move to the next target if available
//        
//        if (currentIndex < m_commands.size() - 1) {
//            currentIndex++;
//            m_current_target = m_commands[currentIndex].position;
//            m_current_speed = m_commands[currentIndex].speed; // Set the first target
//            Console::info() << "Current target : " << m_current_target << Console::endl;
//            Console::info() << "Current speed  : " << m_current_speed << Console::endl;
//        }
//    }
//    else {
//        m_current_position += glm::vec4(movement,1.0);
//    }
//}
 
void GcodeSimulator::update(float dt) {
    m_current_position += m_current_velocity * dt;
    m_current_position.w = 1.0;
    //Console::info() << "dt" << dt << "xyz(" << glm::vec3(m_current_position) << ", " << m_current_velocity.w << ")" << Console::endl;
}

void GcodeSimulator::control(float vx, float vy, float vz, float ve){
    //m_current_position = glm::vec4(vx, vy, vz, ve);
    m_current_velocity = glm::vec4(vx, vy, vz, ve);
}

glm::vec3 GcodeSimulator::getNozzlePosition() {
    return glm::vec3(m_current_position) + m_origin_offset;
}

float GcodeSimulator::getExtruderDistance() {
    //if (m_commands[currentIndex].position.w != 0) return 500 * 2.0 * flow_override;//return m_commands[currentIndex].position.w * 2.0 * flow_override;
    return m_current_velocity.w * flow_override;

    //if (m_commands[currentIndex].position.w != 0) return m_commands[currentIndex].position.w * flow_override;
    //   return 0.0;
}

bool GcodeSimulator::lastCommandReached(){
	if (currentIndex >= m_commands.size() - 1)
		return true;
    else
		return false;
}
