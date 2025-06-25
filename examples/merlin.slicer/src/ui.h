// Generated with ImRAD 0.8
// visit https://github.com/tpecholt/imrad

#pragma once

class SlicerUI
{
public:
    /// @begin interface
    void Draw();

    float splitterH = 750;
    float splitterV = 600;
    float splitterV2 = 500;
    float value51 = 100;
    float slice_splitterH = 980;
    float value57 = 100;
    float gcode_SplitterV = 700;
    float value68 = 100;
    float value69 = 100;
    float value71 = 100;
    std::string start_gcode_macro = ";Gcode generated with Powd3r Slicer;";
    std::string before_layer_gcode_macro = ";Layer change;";
    std::string after_layer_gcode_macro = ";Layer changed;";
    std::string end_gcode_macro = ";End of file;";
    std::string gcode_content;
    std::string gcode_lines = 0;
    float value81 = 100;
    /// @end interface

private:
    /// @begin impl
    void ResetLayout();

    ImRad::VBox vb4000001;
    ImRad::HBox hb4000001;
    ImRad::HBox hb400101011;
    ImRad::VBox vb43001;
    ImRad::VBox vb430001;
    ImRad::HBox hb430001;
    ImRad::HBox hb430000001;
    ImRad::HBox hb430000101;
    ImRad::HBox hb430001001;
    ImRad::HBox hb430001101;
    ImRad::HBox hb43002;
    ImRad::VBox vb430101;
    ImRad::HBox hb430101;
    /// @end impl
};

extern SlicerUI ui;
