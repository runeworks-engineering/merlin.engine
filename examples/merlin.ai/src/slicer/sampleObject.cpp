#include "SampleObject.h"
#include <iomanip> // for std::setprecision
#include <sstream>
#include "merlin/utils/ressourceManager.h"

SampleObject::SampleObject(){
	this->props = SampleProperty();
	generateMesh();
}

SampleObject::SampleObject(const SampleProperty& props) {
	this->props = props;
	generateMesh();
}

void SampleObject::renderMenu() {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	ImGui::Text((std::string("name : ") + props.name).c_str());

	bool changed = false;

	ImGui::Checkbox("Enabled", &enabled);

	changed |= ImGui::DragFloat("X Position", &props.x_position);
	changed |= ImGui::DragFloat("Y Position", &props.y_position);
	changed |= ImGui::DragFloat("Rotation (Z)", &props.rotation_z, 0.1f, -3.14f, 3.14f);

	changed |= ImGui::DragFloat("Length", &props.length);
	changed |= ImGui::DragFloat("Width", &props.width);
	changed |= ImGui::DragFloat("Height", &props.height);

	ImGui::InputFloat("Flow", &props.flow);

	ImGui::InputFloat("Layer height", &props.feedrate);
	ImGui::InputFloat("Line width", &props.line_width);
	ImGui::InputFloat("Feedrate", &props.feedrate);


	char buffer[256];
	strncpy(buffer, props.comment.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	if (ImGui::InputText("Comment", buffer, sizeof(buffer))) {
		props.comment = buffer;
	}

	if (ImGui::Button("Apply changes") || changed) {
		generateMesh();
	}
}

std::string SampleObject::toXML() const {
	std::ostringstream ss;

	ss << "<Sample name=\"" << props.name << "\">\n";
	ss << "  <Comment>" << props.comment << "</Comment>\n";
	ss << "  <Position x=\"" << props.x_position << "\" y=\"" << props.y_position << "\" />\n";

	ss << "  <Geometry lenght=\"" << props.length << "\" width=\"" << props.width << "\" height=\"" << props.height << "\" />\n";

	ss << "  <Print layerHeight=\"" << props.layer_height
		<< "\" lineWidth=\"" << props.line_width
		<< "\" flow=\"" << props.flow
		<< "\" feedrate=\"" << props.feedrate << "\" />\n";

	ss << "</Sample>\n";
	return ss.str();
}

void SampleObject::generateMesh(){
	mesh = Primitives::createBox(props.length, props.width, props.height);
	mesh->rename(props.name);
	mesh->translate(glm::vec3(props.x_position, props.y_position, props.height * 0.5));
	mesh->rotate(glm::vec3(0, 0 , props.rotation_z * DEG_TO_RAD));
	//mesh->setMaterial(Merlin::MaterialLibrary::instance().getColorCycle());  //PLA
	mesh->setMaterial("white rubber");  //PLA
}



SampleProperty SampleObject::fromXML(const tinyxml2::XMLElement* elem) {
	SampleProperty props;

	if (!elem) return props;

	const char* name = elem->Attribute("name");
	if (name) props.name = name;

	if (auto comment = elem->FirstChildElement("Comment"))
		props.comment = comment->GetText() ? comment->GetText() : "";

	if (auto pos = elem->FirstChildElement("Position")) {
		pos->QueryFloatAttribute("x", &props.x_position);
		pos->QueryFloatAttribute("y", &props.y_position);
	}

	if (auto geo = elem->FirstChildElement("Geometry")) {
		geo->QueryFloatAttribute("length", &props.length);
		geo->QueryFloatAttribute("width", &props.width);
		geo->QueryFloatAttribute("height", &props.height);
	}

	if (auto print = elem->FirstChildElement("Print")) {
		print->QueryFloatAttribute("layerHeight", &props.layer_height);
		print->QueryFloatAttribute("lineWidth", &props.line_width);
		print->QueryFloatAttribute("flow", &props.flow);
		print->QueryFloatAttribute("feedrate", &props.feedrate);
	}

	return props;
}
