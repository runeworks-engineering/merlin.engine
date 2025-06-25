// Generated with ImRAD 0.8
// visit https://github.com/tpecholt/imrad

#include "ui.h"

SlicerUI ui;

void SlicerUI::ResetLayout()
{
    // ImGui::GetCurrentWindow()->HiddenFramesCannotSkipItems = 2;
    vb4000001.Reset();
    hb4000001.Reset();
    hb400101011.Reset();
    vb43001.Reset();
    vb430001.Reset();
    hb430001.Reset();
    hb430000001.Reset();
    hb430000101.Reset();
    hb430001001.Reset();
    hb430001101.Reset();
    hb43002.Reset();
    vb430101.Reset();
    hb430101.Reset();
}

void SlicerUI::Draw()
{
    /// @style Light
    /// @unit px
    /// @begin TopWindow
    auto* ioUserData = (ImRad::IOUserData*)ImGui::GetIO().UserData;
    ImGui::PushFont(ImRad::GetFontByName(""));
    ImGui::PushStyleColor(ImGuiCol_MenuBarBg, 0xffc2c2c2);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 7);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    
    bool tmpOpen;
    if (ImGui::Begin("###SlicerUI", &tmpOpen, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar))
    {

        // TODO: Add Draw calls of dependent popup windows here

        /// @begin MenuBar
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_N, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_O, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_S, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Alt | ImGuiKey_S, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_I, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_G, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiMod_Shift | ImGuiKey_G, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_U, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Alt | ImGuiKey_F4, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_A, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_Escape, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_Delete, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Delete, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Z, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_Y, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_C, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiMod_Ctrl | ImGuiKey_V, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_0, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_1, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_2, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_3, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_4, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_5, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_6, ImGuiInputFlags_RouteGlobal))
                ;
            if (ImGui::Shortcut(ImGuiKey_F11, ImGuiInputFlags_RouteGlobal))
                ;
            /// @separator

            /// @begin MenuIt
            if (ImGui::BeginMenu("File"))
            {
                /// @separator

                /// @begin MenuIt
                ImGui::MenuItem("New Project", "Ctrl+N", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Open Project", "Ctrl+O", false);
                /// @end MenuIt

                /// @begin MenuIt
                if (ImGui::BeginMenu("Recent Project"))
                {
                    /// @separator

                    /// @begin MenuIt
                    ImGui::MenuItem("...", "", false);
                    /// @end MenuIt

                    /// @separator
                    ImGui::EndMenu();
                }
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Save Project", "Ctrl+S", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Save Project as", "Ctrl+Alt+S", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::Separator();
                if (ImGui::BeginMenu("Import"))
                {
                    /// @separator

                    /// @begin MenuIt
                    ImGui::MenuItem("Import Mesh", "Ctrl+I", false);
                    /// @end MenuIt

                    /// @begin MenuIt
                    ImGui::MenuItem("Import Configuration", "", false);
                    /// @end MenuIt

                    /// @separator
                    ImGui::EndMenu();
                }
                /// @end MenuIt

                /// @begin MenuIt
                if (ImGui::BeginMenu("Export"))
                {
                    /// @separator

                    /// @begin MenuIt
                    ImGui::MenuItem("Export G-Code..", "Ctrl+G", false);
                    /// @end MenuIt

                    /// @begin MenuIt
                    ImGui::MenuItem("Send G-Code..", "Ctrl+Shift+G", false);
                    /// @end MenuIt

                    /// @begin MenuIt
                    ImGui::MenuItem("Export G-code to SD-Card", "Ctrl+U", false);
                    /// @end MenuIt

                    /// @begin MenuIt
                    ImGui::Separator();
                    ImGui::MenuItem("Export project as STL", "", false);
                    /// @end MenuIt

                    /// @begin MenuIt
                    ImGui::Separator();
                    ImGui::MenuItem("Export Configuration", "", false);
                    /// @end MenuIt

                    /// @separator
                    ImGui::EndMenu();
                }
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::Separator();
                if (ImGui::BeginMenu("Convert"))
                {
                    /// @separator

                    /// @begin MenuIt
                    ImGui::MenuItem("Convert ASCII G-code to Binary", "", false);
                    /// @end MenuIt

                    /// @begin MenuIt
                    ImGui::MenuItem("Convert Binary G-code to ASCII", "", false);
                    /// @end MenuIt

                    /// @separator
                    ImGui::EndMenu();
                }
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::Separator();
                ImGui::MenuItem("Exit", "Alt+F4", false);
                /// @end MenuIt

                /// @separator
                ImGui::EndMenu();
            }
            /// @end MenuIt

            /// @begin MenuIt
            if (ImGui::BeginMenu("Edit"))
            {
                /// @separator

                /// @begin MenuIt
                ImGui::MenuItem("Select All", "Ctrl+A", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Deselect All", "Esc", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::Separator();
                ImGui::MenuItem("Delete Selected", "Del", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::Separator();
                ImGui::MenuItem("Delete All", "Ctrl+Del", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Undo", "Ctrl+Z", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Redo", "Ctrl+Y", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::Separator();
                ImGui::MenuItem("Copy", "Ctrl+C", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Paste", "Ctrl+V", false);
                /// @end MenuIt

                /// @separator
                ImGui::EndMenu();
            }
            /// @end MenuIt

            /// @begin MenuIt
            if (ImGui::BeginMenu("Tool"))
            {
                /// @separator

                /// @begin MenuIt
                ImGui::MenuItem("Nozzle ID tool", "", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("3D Object Gallery", "", false);
                /// @end MenuIt

                /// @separator
                ImGui::EndMenu();
            }
            /// @end MenuIt

            /// @begin MenuIt
            if (ImGui::BeginMenu("View"))
            {
                /// @separator

                /// @begin MenuIt
                ImGui::MenuItem("Iso", "0", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::Separator();
                ImGui::MenuItem("Top", "1", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Bottom", "2", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Front", "3", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Rear", "4", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Left", "5", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Right", "6", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::Separator();
                ImGui::MenuItem("FullScreen", "F11", false);
                /// @end MenuIt

                /// @separator
                ImGui::EndMenu();
            }
            /// @end MenuIt

            /// @begin MenuIt
            ImGui::MenuItem("Configuration", "", false);
            /// @end MenuIt

            /// @begin MenuIt
            if (ImGui::BeginMenu("Help"))
            {
                /// @separator

                /// @begin MenuIt
                ImGui::MenuItem("About", "", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Report an Issue", "", false);
                /// @end MenuIt

                /// @begin MenuIt
                ImGui::MenuItem("Documentation", "", false);
                /// @end MenuIt

                /// @separator
                ImGui::EndMenu();
            }
            /// @end MenuIt

            /// @separator
            ImGui::EndMenuBar();
        }
        /// @end MenuBar

        /// @begin MenuIt
        if (ImGui::BeginPopup("ContextMenu1", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings))
        {
            /// @separator

            /// @begin MenuIt
            ImGui::MenuItem("Item", "", false);
            /// @end MenuIt

            /// @separator
            ImGui::EndPopup();
        }
        /// @end MenuIt

        /// @begin MenuIt
        if (ImGui::BeginPopup("ContextMenu3", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings))
        {
            /// @separator

            /// @begin MenuIt
            ImGui::MenuItem("Item", "", false);
            /// @end MenuIt

            /// @separator
            ImGui::EndPopup();
        }
        /// @end MenuIt

        /// @begin MenuIt
        if (ImGui::BeginPopup("ContextMenu4", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoSavedSettings))
        {
            /// @separator

            /// @begin MenuIt
            ImGui::MenuItem("Item", "", false);
            /// @end MenuIt

            /// @separator
            ImGui::EndPopup();
        }
        /// @end MenuIt

        /// @begin TabBar
        ImGui::PushStyleColor(ImGuiCol_TabActive, 0xff0066ff);
        if (ImGui::BeginTabBar("tabBar1", ImGuiTabBarFlags_None))
        {
            /// @separator

            /// @begin TabItem
            if (ImGui::BeginTabItem("3D Workspace", nullptr, ImGuiTabItemFlags_None))
            {
                /// @separator

                /// @begin Splitter
                ImGui::BeginChild("splitter2", { -1, -1 });
                {
                    ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                    ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                    ImRad::Splitter(true, 8, &splitterH, 10, 10);
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                    /// @separator

                    /// @begin Child
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
                    ImGui::BeginChild("child3", { splitterH, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @begin Splitter
                        ImGui::BeginChild("splitter4", { -1, -1 });
                        {
                            ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                            ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                            ImRad::Splitter(false, 4, &splitterV, 10, 10);
                            ImGui::PopStyleColor();
                            ImGui::PopStyleColor();
                            /// @separator

                            /// @begin Child
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
                            ImGui::BeginChild("child5", { -1, splitterV }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
                            {
                                /// @separator

                                /// @begin CustomWidget
                                vb4000001.BeginLayout();
                                hb4000001.BeginLayout();
                                vb4000001.AddSize(0, ImRad::VBox::Stretch(1));
                                hb4000001.AddSize(0, ImRad::HBox::Stretch(1));
                                /// @end CustomWidget

                                /// @separator
                                ImGui::EndChild();
                            }
                            ImGui::PopStyleVar();
                            /// @end Child

                            /// @begin Child
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
                            ImGui::BeginChild("child6", { -1, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
                            {
                                /// @separator

                                /// @begin Spacer
                                ImRad::Dummy({ 0, 0 });
                                /// @end Spacer

                                /// @begin Button
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImGui::Button("Import", { 0, 0 });
                                /// @end Button

                                /// @begin Spacer
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImRad::Dummy({ 0, 0 });
                                /// @end Spacer

                                /// @begin Button
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImGui::Button("Copy", { 0, 0 });
                                /// @end Button

                                /// @begin Spacer
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImRad::Dummy({ 0, 0 });
                                /// @end Spacer

                                /// @begin Button
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImGui::Button("Move", { 0, 0 });
                                /// @end Button

                                /// @begin Button
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImGui::Button("Scale", { 0, 0 });
                                /// @end Button

                                /// @begin Spacer
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImRad::Dummy({ 0, 0 });
                                /// @end Spacer

                                /// @begin Button
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImGui::Button("Rotate", { 0, 0 });
                                /// @end Button

                                /// @begin Spacer
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImRad::Dummy({ 0, 0 });
                                /// @end Spacer

                                /// @begin Button
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImGui::Button("Center", { 0, 0 });
                                /// @end Button

                                /// @begin Spacer
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImRad::Dummy({ 0, 0 });
                                /// @end Spacer

                                /// @begin Button
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImGui::Button("Remove", { 0, 0 });
                                /// @end Button

                                /// @begin Spacer
                                ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                ImRad::Dummy({ -1, 0 });
                                /// @end Spacer

                                /// @separator
                                ImGui::EndChild();
                            }
                            ImGui::PopStyleVar();
                            /// @end Child

                            /// @separator
                            ImGui::EndChild();
                        }
                        /// @end Splitter

                        /// @separator
                        ImGui::EndChild();
                    }
                    ImGui::PopStyleVar();
                    /// @end Child

                    /// @begin Child
                    ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
                    ImGui::BeginChild("child7", { -1, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @begin Splitter
                        ImGui::BeginChild("splitter8", { -1, -1 });
                        {
                            ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                            ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                            ImRad::Splitter(false, 4, &splitterV2, 10, 10);
                            ImGui::PopStyleColor();
                            ImGui::PopStyleColor();
                            /// @separator

                            /// @begin Table
                            if (ImGui::BeginTable("table9", 1, ImGuiTableFlags_NoPadOuterX | ImGuiTableFlags_NoPadInnerX, { -1, 0 }))
                            {
                                ImGui::TableSetupColumn("item1", ImGuiTableColumnFlags_WidthStretch, 0);
                                ImGui::TableNextRow(0, 0);
                                ImGui::TableSetColumnIndex(0);
                                /// @separator

                                /// @begin Child
                                ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
                                ImGui::BeginChild("child9", { -1, splitterV2 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
                                {
                                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, { 0, 0 });
                                    /// @separator

                                    /// @begin TreeNode
                                    ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);
                                    if (ImGui::TreeNodeEx("Objects", ImGuiTreeNodeFlags_SpanAvailWidth))
                                    {
                                        /// @separator

                                        /// @separator
                                        ImGui::TreePop();
                                    }
                                    /// @end TreeNode

                                    /// @separator
                                    ImGui::PopStyleVar();
                                    ImGui::EndChild();
                                }
                                ImGui::PopStyleVar();
                                /// @end Child


                                /// @separator
                                ImGui::EndTable();
                            }
                            /// @end Table

                            /// @begin Child
                            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0, 0 });
                            ImGui::BeginChild("child11", { 328, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
                            {
                                /// @separator

                                /// @begin Splitter
                                ImGui::BeginChild("splitter12", { -1, -1 });
                                {
                                    ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                                    ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                                    ImRad::Splitter(false, 4, &value51, 10, 10);
                                    ImGui::PopStyleColor();
                                    ImGui::PopStyleColor();
                                    /// @separator

                                    /// @begin Child
                                    ImGui::BeginChild("child13", { -1, 90 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
                                    {
                                        /// @separator

                                        /// @separator
                                        ImGui::EndChild();
                                    }
                                    /// @end Child

                                    /// @begin Child
                                    ImGui::BeginChild("child14", { -1, 40 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
                                    {
                                        /// @separator

                                        /// @begin Spacer
                                        hb400101011.BeginLayout();
                                        ImRad::Dummy({ hb400101011.GetSize(), 0 });
                                        hb400101011.AddSize(0, ImRad::HBox::Stretch(1));
                                        /// @end Spacer

                                        /// @begin Button
                                        ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                        ImGui::Button("Slice", { 200, 0 });
                                        hb400101011.AddSize(1, 200);
                                        /// @end Button

                                        /// @begin Spacer
                                        ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                                        ImRad::Dummy({ -1, 0 });
                                        hb400101011.AddSize(1, -1);
                                        /// @end Spacer

                                        /// @separator
                                        ImGui::EndChild();
                                    }
                                    /// @end Child

                                    /// @separator
                                    ImGui::EndChild();
                                }
                                /// @end Splitter

                                /// @separator
                                ImGui::EndChild();
                            }
                            ImGui::PopStyleVar();
                            /// @end Child

                            /// @separator
                            ImGui::EndChild();
                        }
                        /// @end Splitter

                        /// @separator
                        ImGui::EndChild();
                    }
                    ImGui::PopStyleVar();
                    /// @end Child

                    /// @separator
                    ImGui::EndChild();
                }
                /// @end Splitter

                /// @separator
                ImGui::EndTabItem();
            }
            /// @end TabItem

            /// @begin TabItem
            if (ImGui::BeginTabItem("2D Slice", nullptr, ImGuiTabItemFlags_None))
            {
                /// @separator

                /// @begin Splitter
                ImGui::BeginChild("splitter15", { -1, -1 });
                {
                    ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                    ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                    ImRad::Splitter(true, 8, &slice_splitterH, 10, 10);
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                    /// @separator

                    /// @begin Child
                    ImGui::BeginChild("child16", { slice_splitterH, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding, ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @begin CustomWidget
                        /// @end CustomWidget

                        /// @separator
                        ImGui::EndChild();
                    }
                    /// @end Child

                    /// @begin Child
                    ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                    ImGui::BeginChild("child17", { -1, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding , ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @separator
                        ImGui::EndChild();
                    }
                    /// @end Child

                    /// @separator
                    ImGui::EndChild();
                }
                /// @end Splitter

                /// @separator
                ImGui::EndTabItem();
            }
            /// @end TabItem

            /// @begin TabItem
            if (ImGui::BeginTabItem("ToolPath", nullptr, ImGuiTabItemFlags_None))
            {
                /// @separator

                /// @begin Splitter
                ImGui::BeginChild("splitter18", { -1, -1 });
                {
                    ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                    ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                    ImRad::Splitter(true, 8, &slice_splitterH, 10, 10);
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                    /// @separator

                    /// @begin Child
                    ImGui::BeginChild("child19", { slice_splitterH, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding , ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @begin CustomWidget
                        /// @end CustomWidget

                        /// @separator
                        ImGui::EndChild();
                    }
                    /// @end Child

                    /// @begin Child
                    ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                    ImGui::BeginChild("child20", { -1, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding , ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @separator
                        ImGui::EndChild();
                    }
                    /// @end Child

                    /// @separator
                    ImGui::EndChild();
                }
                /// @end Splitter

                /// @separator
                ImGui::EndTabItem();
            }
            /// @end TabItem

            /// @begin TabItem
            if (ImGui::BeginTabItem("Gcode", nullptr, ImGuiTabItemFlags_None))
            {
                /// @separator

                /// @begin Splitter
                ImGui::BeginChild("splitter21", { -1, -1 });
                {
                    ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                    ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                    ImRad::Splitter(true, 8, &gcode_SplitterV, 10, 10);
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                    /// @separator

                    /// @begin Child
                    ImGui::BeginChild("child22", { gcode_SplitterV, -1 }, ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding | ImGuiChildFlags_FrameStyle , ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @begin Splitter
                        vb43001.BeginLayout();
                        ImGui::BeginChild("splitter23", { -1, -1 });
                        {
                            ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                            ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                            ImRad::Splitter(true, 8, &value68, 10, 10);
                            ImGui::PopStyleColor();
                            ImGui::PopStyleColor();
                            /// @separator

                            /// @begin Splitter
                            vb430001.BeginLayout();
                            hb430001.BeginLayout();
                            ImGui::BeginChild("splitter24", { hb430001.GetSize(), vb430001.GetSize() });
                            {
                                ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                                ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                                ImRad::Splitter(false, 0, &value69, 10, 10);
                                ImGui::PopStyleColor();
                                ImGui::PopStyleColor();
                                /// @separator

                                /// @begin TabBar
                                ImGui::PushStyleColor(ImGuiCol_TabActive, 0xffc2c2c2);
                                if (ImGui::BeginTabBar("tabBar25", ImGuiTabBarFlags_None))
                                {
                                    int _nTabs = std::max(1, (int)ImGui::GetCurrentTabBar()->TabsActiveCount);
                                    float _tabWidth = (ImGui::GetContentRegionAvail().x - (_nTabs - 1) * ImGui::GetStyle().ItemInnerSpacing.x) / _nTabs - 1;
                                    /// @separator

                                    /// @begin TabItem
                                    ImGui::SetNextItemWidth(_tabWidth);
                                    if (ImGui::BeginTabItem("Start G-code", nullptr, ImGuiTabItemFlags_None))
                                    {
                                        /// @separator

                                        /// @begin Input
                                        hb430000001.BeginLayout();
                                        ImGui::InputTextMultiline("##start_gcode_macro", &start_gcode_macro, { hb430000001.GetSize(), 280 }, ImGuiInputTextFlags_Multiline);
                                        if (ImGui::IsItemActive())
                                            ioUserData->imeType = ImRad::ImeText;
                                        hb430000001.AddSize(0, ImRad::HBox::Stretch(1));
                                        /// @end Input

                                        /// @separator
                                        ImGui::EndTabItem();
                                    }
                                    /// @end TabItem

                                    /// @separator
                                    ImGui::EndTabBar();
                                }
                                ImGui::PopStyleColor();
                                /// @end TabBar

                                /// @begin TabBar
                                ImGui::PushStyleColor(ImGuiCol_TabActive, 0xffc2c2c2);
                                if (ImGui::BeginTabBar("tabBar26", ImGuiTabBarFlags_None))
                                {
                                    int _nTabs = std::max(1, (int) ImGui::GetCurrentTabBar()->TabsActiveCount);
                                    float _tabWidth = (ImGui::GetContentRegionAvail().x - (_nTabs - 1) * ImGui::GetStyle().ItemInnerSpacing.x) / _nTabs - 1;
                                    /// @separator

                                    /// @begin TabItem
                                    ImGui::SetNextItemWidth(_tabWidth);
                                    if (ImGui::BeginTabItem("End G-code", nullptr, ImGuiTabItemFlags_None))
                                    {
                                        /// @separator

                                        /// @begin Input
                                        hb430000101.BeginLayout();
                                        ImGui::InputTextMultiline("##end_gcode_macro", &end_gcode_macro, { hb430000101.GetSize(), 280 }, ImGuiInputTextFlags_Multiline);
                                        if (ImGui::IsItemActive())
                                            ioUserData->imeType = ImRad::ImeText;
                                        hb430000101.AddSize(0, ImRad::HBox::Stretch(1));
                                        /// @end Input

                                        /// @separator
                                        ImGui::EndTabItem();
                                    }
                                    /// @end TabItem

                                    /// @separator
                                    ImGui::EndTabBar();
                                }
                                ImGui::PopStyleColor();
                                /// @end TabBar

                                /// @separator
                                ImGui::EndChild();
                            }
                            vb430001.AddSize(0, ImRad::VBox::Stretch(1));
                            hb430001.AddSize(0, ImRad::HBox::Stretch(1));
                            /// @end Splitter

                            /// @begin Splitter
                            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                            ImGui::BeginChild("splitter27", { -1, -1 });
                            {
                                ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                                ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                                ImRad::Splitter(false, 0, &value71, 10, 10);
                                ImGui::PopStyleColor();
                                ImGui::PopStyleColor();
                                /// @separator

                                /// @begin TabBar
                                ImGui::PushStyleColor(ImGuiCol_TabActive, 0xff97ccff);
                                if (ImGui::BeginTabBar("tabBar28", ImGuiTabBarFlags_None))
                                {
                                    int _nTabs = std::max(1, (int) ImGui::GetCurrentTabBar()->TabsActiveCount);
                                    float _tabWidth = (ImGui::GetContentRegionAvail().x - (_nTabs - 1) * ImGui::GetStyle().ItemInnerSpacing.x) / _nTabs - 1;
                                    /// @separator

                                    /// @begin TabItem
                                    ImGui::SetNextItemWidth(_tabWidth);
                                    if (ImGui::BeginTabItem("Before layer change G-code", nullptr, ImGuiTabItemFlags_None))
                                    {
                                        /// @separator

                                        /// @begin Input
                                        hb430001001.BeginLayout();
                                        ImGui::InputTextMultiline("##before_layer_gcode_macro", &before_layer_gcode_macro, { hb430001001.GetSize(), 280 }, ImGuiInputTextFlags_Multiline);
                                        if (ImGui::IsItemActive())
                                            ioUserData->imeType = ImRad::ImeText;
                                        hb430001001.AddSize(0, ImRad::HBox::Stretch(1));
                                        /// @end Input

                                        /// @separator
                                        ImGui::EndTabItem();
                                    }
                                    /// @end TabItem

                                    /// @separator
                                    ImGui::EndTabBar();
                                }
                                ImGui::PopStyleColor();
                                /// @end TabBar

                                /// @begin TabBar
                                ImGui::PushStyleColor(ImGuiCol_TabActive, 0xff97ccff);
                                if (ImGui::BeginTabBar("tabBar29", ImGuiTabBarFlags_None))
                                {
                                    int _nTabs = std::max(1, (int) ImGui::GetCurrentTabBar()->TabsActiveCount);
                                    float _tabWidth = (ImGui::GetContentRegionAvail().x - (_nTabs - 1) * ImGui::GetStyle().ItemInnerSpacing.x) / _nTabs - 1;
                                    /// @separator

                                    /// @begin TabItem
                                    ImGui::SetNextItemWidth(_tabWidth);
                                    if (ImGui::BeginTabItem("After layer change G-code", nullptr, ImGuiTabItemFlags_None))
                                    {
                                        /// @separator

                                        /// @begin Input
                                        hb430001101.BeginLayout();
                                        ImGui::InputTextMultiline("##after_layer_gcode_macro", &after_layer_gcode_macro, { hb430001101.GetSize(), 280 }, ImGuiInputTextFlags_Multiline);
                                        if (ImGui::IsItemActive())
                                            ioUserData->imeType = ImRad::ImeText;
                                        hb430001101.AddSize(0, ImRad::HBox::Stretch(1));
                                        /// @end Input

                                        /// @separator
                                        ImGui::EndTabItem();
                                    }
                                    /// @end TabItem

                                    /// @separator
                                    ImGui::EndTabBar();
                                }
                                ImGui::PopStyleColor();
                                /// @end TabBar

                                /// @separator
                                ImGui::EndChild();
                            }
                            vb430001.UpdateSize(0, -1);
                            hb430001.AddSize(1, -1);
                            /// @end Splitter

                            /// @separator
                            ImGui::EndChild();
                        }
                        vb43001.AddSize(0, -1);
                        /// @end Splitter

                        /// @begin Spacer
                        hb43002.BeginLayout();
                        ImRad::Dummy({ hb43002.GetSize(), vb43001.GetSize() });
                        vb43001.AddSize(1, ImRad::VBox::Stretch(1));
                        hb43002.AddSize(0, ImRad::HBox::Stretch(1));
                        /// @end Spacer

                        /// @separator
                        ImGui::EndChild();
                    }
                    /// @end Child

                    /// @begin Child
                    ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                    ImVec2 _sz30{ -1, -1 };
                    ImRad::IgnoreWindowPaddingData _data30;
                    ImRad::PushIgnoreWindowPadding(&_sz30, &_data30);
                    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 4, 1 });
                    ImGui::BeginChild("child30", _sz30, ImGuiChildFlags_Border | ImGuiChildFlags_AlwaysUseWindowPadding , ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @begin Splitter
                        ImGui::BeginChild("splitter31", { -1, -1 });
                        {
                            ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                            ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                            ImRad::Splitter(true, 8, &value81, 10, 10);
                            ImGui::PopStyleColor();
                            ImGui::PopStyleColor();
                            /// @separator

                            /// @begin Input
                            vb430101.BeginLayout();
                            hb430101.BeginLayout();
                            ImGui::PushStyleColor(ImGuiCol_FrameBg, ImGui::GetStyleColorVec4(ImGuiCol_MenuBarBg));
                            ImGui::InputTextMultiline("##gcode_lines", &gcode_lines, { 50, 600 }, ImGuiInputTextFlags_Multiline);
                            if (ImGui::IsItemActive())
                                ioUserData->imeType = ImRad::ImeText;
                            vb430101.AddSize(0, 600);
                            hb430101.AddSize(0, 50);
                            ImGui::PopStyleColor();
                            /// @end Input

                            /// @begin Input
                            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                            ImGui::InputTextMultiline("##gcode_content", &gcode_content, { hb430101.GetSize(), 600 }, ImGuiInputTextFlags_Multiline);
                            if (ImGui::IsItemActive())
                                ioUserData->imeType = ImRad::ImeText;
                            vb430101.UpdateSize(0, 600);
                            hb430101.AddSize(1, ImRad::HBox::Stretch(1));
                            /// @end Input

                            /// @begin Button
                            ImGui::PushStyleColor(ImGuiCol_Button, 0xffffffff);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xff989a98);
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0xffc2c2c2);
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
                            ImGui::Button("Generate G-code", { 182, vb430101.GetSize() });
                            vb430101.AddSize(1, ImRad::VBox::Stretch(1));
                            ImGui::PopStyleVar();
                            ImGui::PopStyleColor();
                            ImGui::PopStyleColor();
                            ImGui::PopStyleColor();
                            /// @end Button

                            /// @begin Button
                            ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                            ImGui::PushStyleColor(ImGuiCol_Button, 0xffffffff);
                            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, 0xff989a98);
                            ImGui::PushStyleColor(ImGuiCol_ButtonActive, 0xffc2c2c2);
                            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 0, 0 });
                            ImGui::Button("Export G-code", { 182, vb430101.GetSize() });
                            vb430101.UpdateSize(0, ImRad::VBox::Stretch(1));
                            ImGui::PopStyleVar();
                            ImGui::PopStyleColor();
                            ImGui::PopStyleColor();
                            ImGui::PopStyleColor();
                            /// @end Button

                            /// @separator
                            ImGui::EndChild();
                        }
                        /// @end Splitter

                        /// @separator
                        ImGui::EndChild();
                    }
                    ImGui::PopStyleVar();
                    ImRad::PopIgnoreWindowPadding(_data30);
                    /// @end Child

                    /// @separator
                    ImGui::EndChild();
                }
                /// @end Splitter

                /// @separator
                ImGui::EndTabItem();
            }
            /// @end TabItem

            /// @begin TabItem
            if (ImGui::BeginTabItem("Gcode Viewer", nullptr, ImGuiTabItemFlags_None))
            {
                /// @separator

                /// @begin Splitter
                ImGui::BeginChild("splitter32", { -1, -1 });
                {
                    ImGui::PushStyleColor(ImGuiCol_Separator, 0x00000000);
                    ImGui::PushStyleColor(ImGuiCol_SeparatorHovered, 0x00000000);
                    ImRad::Splitter(true, 8, &value57, 10, 10);
                    ImGui::PopStyleColor();
                    ImGui::PopStyleColor();
                    /// @separator

                    /// @begin Child
                    ImGui::BeginChild("child33", { slice_splitterH, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding , ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @begin CustomWidget
                        /// @end CustomWidget

                        /// @separator
                        ImGui::EndChild();
                    }
                    /// @end Child

                    /// @begin Child
                    ImGui::SameLine(0, 1 * ImGui::GetStyle().ItemSpacing.x);
                    ImGui::BeginChild("child34", { -1, -1 }, ImGuiChildFlags_AlwaysUseWindowPadding , ImGuiWindowFlags_NoSavedSettings);
                    {
                        /// @separator

                        /// @separator
                        ImGui::EndChild();
                    }
                    /// @end Child

                    /// @separator
                    ImGui::EndChild();
                }
                /// @end Splitter

                /// @separator
                ImGui::EndTabItem();
            }
            /// @end TabItem

            /// @separator
            ImGui::EndTabBar();
        }
        ImGui::PopStyleColor();
        /// @end TabBar

        /// @separator
        ImGui::End();
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
    ImGui::PopFont();
    /// @end TopWindow
}
