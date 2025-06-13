#pragma once


class Slicer {
public:
    Slicer();

    /// Generate slice data for one layer of objA (tool A) and objB (tool B).
    /// Populates internal path lines.
    void slice(const Mesh& objA, const Mesh& objB);

    /// Convert the sliced path data into raw G-code commands.
    void process();

    /// Wrap the generated commands with headers/footers and return final G-code.
    std::string postprocess() const;

private:
    struct PathLine {
        int tool;
        glm::vec3 start;
        glm::vec3 end;
        float lineWidth;
        float flowMultiplier;
        float feedrate;
    };

    std::vector<PathLine> lines_;
    std::vector<std::string> gcodeLines_;

    int m_currentLayer;
    float m_cumulativeE;
    int m_lastTool;
};