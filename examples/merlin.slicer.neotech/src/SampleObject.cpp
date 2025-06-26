#include "SampleObject.h"
#include <iomanip> // for std::setprecision
#include <sstream>


SampleObject::SampleObject(const SampleProperty& props) {

	this->props = props;

	static int i = 0;
	mesh = Primitives::createCylinder(props.radius, props.height, 30);
	mesh->rename(props.name);
	mesh->translate(glm::vec3(props.x_offset, props.y_offset, props.height * 1.0));
	mesh->rotate(glm::vec3(0, 90 * DEG_TO_RAD, 0));
	mesh->setMaterial("black plastic");  //PLA
	i++;

}

void SampleObject::renderMenu() {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	ImGui::Text((std::string("name : ") + props.name).c_str());

	bool changed = false;

	ImGui::Checkbox("Enabled", &enabled);

	changed |= ImGui::DragFloat("X Offset", &props.x_offset);
	changed |= ImGui::DragFloat("Y Offset", &props.y_offset);
	changed |= ImGui::DragFloat("Radius", &props.radius);
	changed |= ImGui::DragFloat("Height", &props.height);

	ImGui::InputInt("Tool", &props.tool);

	ImGui::InputFloat("Flow", &props.flow);

	ImGui::InputFloat("Retract", &props.retract);

	ImGui::InputFloat("Feedrate", &props.feedrate);

	ImGui::InputFloat("Line Width", &props.line_width);

	if (!ImGui::Checkbox("Use concentric infill", &props.use_concentric))
		ImGui::Checkbox("Use outline", &props.use_outline);

	char buffer[256];
	strncpy(buffer, props.comment.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	if (ImGui::InputText("Comment", buffer, sizeof(buffer))) {
		props.comment = buffer;
	}

	if (ImGui::Button("Apply changes") || changed) {
		std::string name = mesh->name();
		mesh = Primitives::createCylinder(props.radius, props.height,30);
		mesh->rename(name);
		mesh->translate(glm::vec3(props.x_offset, props.y_offset, props.height * 1.0));
		mesh->rotate(glm::vec3(0, 90 * DEG_TO_RAD, 0));
		mesh->setMaterial("black plastic");  //PLA

	}
}

std::string SampleObject::toXML() const {
	std::ostringstream ss;

	ss << "<Sample name=\"" << props.name << "\">\n";
	ss << "  <Comment>" << props.comment << "</Comment>\n";
	ss << "  <Position x=\"" << props.x_offset << "\" y=\"" << props.y_offset << "\" />\n";

	ss << "  <Geometry radius=\"" << props.radius << "\" height=\"" << props.height << "\" />\n";

	ss << "  <Print layerHeight=\"" << props.layer_height
		<< "\" lineWidth=\"" << props.line_width
		<< "\" tool=\"" << props.tool
		<< "\" flow=\"" << props.flow
		<< "\" retract=\"" << props.retract
		<< "\" feedrate=\"" << props.feedrate << "\" />\n";

	ss << "</Sample>\n";
	return ss.str();
}



SampleProperty SampleObject::fromXML(const tinyxml2::XMLElement* elem) {
	SampleProperty props;

	if (!elem) return props;

	const char* name = elem->Attribute("name");
	if (name) props.name = name;

	if (auto comment = elem->FirstChildElement("Comment"))
		props.comment = comment->GetText() ? comment->GetText() : "";

	if (auto pos = elem->FirstChildElement("Position")) {
		pos->QueryFloatAttribute("x", &props.x_offset);
		pos->QueryFloatAttribute("y", &props.y_offset);
	}

	if (auto geo = elem->FirstChildElement("Geometry")) {
		geo->QueryFloatAttribute("radius", &props.radius);
		geo->QueryFloatAttribute("height", &props.height);
	}

	if (auto print = elem->FirstChildElement("Print")) {
		print->QueryFloatAttribute("layerHeight", &props.layer_height);
		print->QueryFloatAttribute("lineWidth", &props.line_width);
		print->QueryIntAttribute("tool", &props.tool);
		print->QueryFloatAttribute("flow", &props.flow);
		print->QueryFloatAttribute("retract", &props.retract);
		print->QueryFloatAttribute("feedrate", &props.feedrate);
	}

	return props;
}
