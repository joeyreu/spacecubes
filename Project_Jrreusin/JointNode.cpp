#include "JointNode.hpp"
#include<iostream>

//---------------------------------------------------------------------------------------
JointNode::JointNode(const std::string& name)
	: SceneNode(name)
{
	m_nodeType = NodeType::JointNode;
}

//---------------------------------------------------------------------------------------
JointNode::~JointNode() {

}
 //---------------------------------------------------------------------------------------
void JointNode::set_joint_x(double min, double init, double max) {
	m_joint_x.min = min;
	m_joint_x.init = init;
	m_joint_x.max = max;
}

//---------------------------------------------------------------------------------------
void JointNode::set_joint_y(double min, double init, double max) {
	m_joint_y.min = min;
	m_joint_y.init = init;
	m_joint_y.max = max;
}

void JointNode::rotate(char axis, float angle) {
    glm::vec3 rot_axis;
    glm::mat4 rot_matrix;
//    std::cout << "  min: " << m_joint_x.min << "max: " << m_joint_x.max  << std::endl;
//    std::cout << " current: " << cur_x << " angle: " << angle  << std::endl;
    switch (axis) {
        case 'x':
            rot_axis = glm::vec3(1,0,0);
            if (cur_x + angle >= m_joint_x.max)
                cur_x = m_joint_x.max;
            else if (cur_x + angle <= m_joint_x.min)
                cur_x = m_joint_x.min;
            else
                cur_x += angle;
            rot_matrix = glm::rotate(degreesToRadians(cur_x), rot_axis);
            break;
        case 'y':
            rot_axis = glm::vec3(0,1,0);
            if (cur_y + angle >= m_joint_y.max)
                cur_y = m_joint_y.max;
            else if (cur_y + angle <= m_joint_y.min)
                cur_y = m_joint_y.min;
            else
                cur_y += angle;
            rot_matrix = glm::rotate(degreesToRadians(cur_y), rot_axis);
            break;
        case 'z':
            rot_axis = glm::vec3(0,0,1);
            break;
        default:
            break;
    }
    
    trans = rot_matrix;
}

void JointNode::rotatePicker(float angle) {
    if (m_joint_x.min != m_joint_x.max) {
        rotate('x', angle);
    }else {
        rotate('y', angle);
    }
}
