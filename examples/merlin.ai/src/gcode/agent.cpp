#include "agent.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include "merlin/core/console.h"

using namespace Merlin;

Agent::Agent() : m_current_position(0.0f), m_current_target(0.0f) {
    m_current_position = glm::vec4(150, 100, 0, 0);
}

void Agent::reset() {
    m_current_position = glm::vec4(-m_origin_offset, 0);
    m_current_position = glm::vec4(0);
}
 
void Agent::update(float dt) {
    m_current_position += m_current_velocity * dt;
    m_current_position.w = 1.0;

    if (m_current_position.x > 80) m_current_position.x = 80;
    if (m_current_position.x < -80) m_current_position.x = -80;
    if (m_current_position.y > 80) m_current_position.y = 80;
    if (m_current_position.y < -80) m_current_position.y = -80;
}

void Agent::control(float vx, float vy, float vz, float ve){
    m_current_velocity = glm::vec4(vx, vy, vz, ve);
}

glm::vec3 Agent::getNozzlePosition() {
    return glm::vec3(m_current_position) + m_origin_offset;
}

float Agent::getExtruderDistance() {
    return m_current_velocity.w * flow_override;
}
