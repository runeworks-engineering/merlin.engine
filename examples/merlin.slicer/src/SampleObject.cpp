#include "SampleObject.h"
#include <iomanip> // for std::setprecision
#include <sstream>


SampleObject::SampleObject(const SampleProperty& props) {

	this->props = props;

	static int i = 0;
	mesh_A = Primitives::createBox(props.width, props.thickness * 0.5, props.height);
	mesh_A->rename("s_" + std::to_string(i) + "PLA");
	mesh_A->translate(glm::vec3(0, props.thickness * 0.25, props.height * 0.5) + glm::vec3(props.x_offset, props.y_offset, 0));
	mesh_A->setMaterial("black plastic");  //PLA


	mesh_B = Primitives::createBox(props.width, props.thickness * 0.5, props.height);
	mesh_B->rename("s_" + std::to_string(i) + "TPU");
	mesh_B->translate(glm::vec3(0, -props.thickness * 0.25, props.height * 0.5) + glm::vec3(props.x_offset, props.y_offset, 0));
	mesh_B->setMaterial("cyan plastic");  //TPU

	i++;

}

void SampleObject::renderMenu() {
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;

	ImGui::Text((std::string("name : ") + props.name).c_str());

	bool changed = false;

	ImGui::Checkbox("Enabled", &enabled);

	changed |= ImGui::DragFloat("X Offset", &props.x_offset);
	changed |= ImGui::DragFloat("Y Offset", &props.y_offset);
	changed |= ImGui::DragFloat("Width", &props.width);
	changed |= ImGui::DragFloat("Height", &props.height);
	changed |= ImGui::DragFloat("Thickness", &props.thickness);

	ImGui::InputInt("Tool A", &props.tool_a);
	ImGui::InputInt("Tool B", &props.tool_b);

	ImGui::InputFloat("Flow A", &props.flow_a);
	ImGui::InputFloat("Flow B", &props.flow_b);

	ImGui::InputFloat("Retract A", &props.retract_a);
	ImGui::InputFloat("Retract B", &props.retract_b);

	ImGui::InputFloat("Feedrate A", &props.feedrate_a);
	ImGui::InputFloat("Feedrate B", &props.feedrate_b);

	ImGui::InputFloat("Line Width", &props.line_width);

	ImGui::InputFloat("Temperature A", &props.temperature_a);
	ImGui::InputFloat("Temperature B", &props.temperature_b);

	ImGui::InputInt("Overlap lines", &props.overlap);
	ImGui::InputFloat("Overlap Flow Modifier", &props.overlap_flow_modifier);

	ImGui::Checkbox("Use Purge Tower", &props.use_purge_tower);
	ImGui::Checkbox("Use Alternate Sweep", &props.use_alternate_sweep);
	ImGui::Checkbox("Use In to Out", &props.use_in_to_out);

	char buffer[256];
	strncpy(buffer, props.comment.c_str(), sizeof(buffer));
	buffer[sizeof(buffer) - 1] = '\0';
	if (ImGui::InputText("Comment", buffer, sizeof(buffer))) {
		props.comment = buffer;
	}

	if (ImGui::Button("Apply changes") || changed) {
		std::string name_A = mesh_A->name();
		mesh_A = Primitives::createBox(props.width, props.thickness * 0.5, props.height);
		mesh_A->rename(name_A);
		mesh_A->translate(glm::vec3(0, props.thickness * 0.25, props.height * 0.5) + glm::vec3(props.x_offset, props.y_offset, 0));
		mesh_A->setMaterial("black plastic");  //PLA

		std::string name_B = mesh_B->name();
		mesh_B = Primitives::createBox(props.width, props.thickness * 0.5, props.height);
		mesh_B->rename(name_B);
		mesh_B->translate(glm::vec3(0, -props.thickness * 0.25, props.height * 0.5) + glm::vec3(props.x_offset, props.y_offset, 0));
		mesh_B->setMaterial("cyan plastic");  //TPU

	}
}

std::string SampleObject::toXML() const {
	std::ostringstream ss;

	ss << "<Sample name=\"" << props.name << "\">\n";
	ss << "  <Comment>" << props.comment << "</Comment>\n";
	ss << "  <Position x=\"" << props.x_offset << "\" y=\"" << props.y_offset << "\" />\n";
	ss << "  <Geometry width=\"" << props.width << "\" height=\"" << props.height
		<< "\" thickness=\"" << props.thickness << "\" />\n";

	ss << "  <Extrusion toolA=\"" << props.tool_a << "\" toolB=\"" << props.tool_b
		<< "\" flowA=\"" << props.flow_a << "\" flowB=\"" << props.flow_b
		<< "\" retractA=\"" << props.retract_a << "\" retractB=\"" << props.retract_b << "\" />\n";

	ss << "  <Print feedrateA=\"" << props.feedrate_a << "\" feedrateB=\"" << props.feedrate_b
		<< "\" temperatureA=\"" << props.temperature_a << "\" temperatureB=\"" << props.temperature_b
		<< "\" lineWidth=\"" << props.line_width << "\" />\n";

	ss << "  <Overlap count=\"" << props.overlap << "\" flowModifier=\"" << props.overlap_flow_modifier << "\" />\n";

	ss << "  <Flags purgeTower=\"" << (props.use_purge_tower ? "true" : "false")
		<< "\" alternateSweep=\"" << (props.use_alternate_sweep ? "true" : "false")
		<< "\" inToOut=\"" << (props.use_in_to_out ? "true" : "false") << "\" />\n";

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
		geo->QueryFloatAttribute("width", &props.width);
		geo->QueryFloatAttribute("height", &props.height);
		geo->QueryFloatAttribute("thickness", &props.thickness);
	}

	if (auto extrude = elem->FirstChildElement("Extrusion")) {
		extrude->QueryIntAttribute("toolA", &props.tool_a);
		extrude->QueryIntAttribute("toolB", &props.tool_b);
		extrude->QueryFloatAttribute("flowA", &props.flow_a);
		extrude->QueryFloatAttribute("flowB", &props.flow_b);
		extrude->QueryFloatAttribute("retractA", &props.retract_a);
		extrude->QueryFloatAttribute("retractB", &props.retract_b);
	}

	if (auto print = elem->FirstChildElement("Print")) {
		print->QueryFloatAttribute("feedrateA", &props.feedrate_a);
		print->QueryFloatAttribute("feedrateB", &props.feedrate_b);
		print->QueryFloatAttribute("temperatureA", &props.temperature_a);
		print->QueryFloatAttribute("temperatureB", &props.temperature_b);
		print->QueryFloatAttribute("lineWidth", &props.line_width);
	}

	if (auto overlap = elem->FirstChildElement("Overlap")) {
		overlap->QueryIntAttribute("count", &props.overlap);
		overlap->QueryFloatAttribute("flowModifier", &props.overlap_flow_modifier);
	}

	if (auto flags = elem->FirstChildElement("Flags")) {
		flags->QueryBoolAttribute("purgeTower", &props.use_purge_tower);
		flags->QueryBoolAttribute("alternateSweep", &props.use_alternate_sweep);
		flags->QueryBoolAttribute("inToOut", &props.use_in_to_out);
	}

	return props;
}