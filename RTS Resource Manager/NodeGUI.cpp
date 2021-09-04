#ifndef IMGUI_DEFINE_MATH_OPERATORS
#   define IMGUI_DEFINE_MATH_OPERATORS
#endif


#include "NodeGUI.h"

namespace NodeGUI
{
    Canvas::Canvas(ImVec2 relStartPos, ImVec2 gridSpacing, ImVec2 boxSize)
        : imgui_style(ImGui::GetStyle())
        , relStartPos(relStartPos)
        , gridSpacing(gridSpacing)
        , boxSize(boxSize)
        , numOfNodes(0)
    {
        Colors[ColCanvasLines]      = imgui_style.Colors[ImGuiCol_Separator];
        Colors[ColNodeBg]           = imgui_style.Colors[ImGuiCol_WindowBg];
        Colors[ColNodeActiveBg]     = imgui_style.Colors[ImGuiCol_FrameBgActive];
        Colors[ColNodeBorder]       = imgui_style.Colors[ImGuiCol_Border];
        Colors[ColConnection]       = imgui_style.Colors[ImGuiCol_PlotLines];
        Colors[ColConnectionActive] = imgui_style.Colors[ImGuiCol_PlotLinesHovered];
        Colors[ColSelectBg]         = imgui_style.Colors[ImGuiCol_FrameBgActive];
        Colors[ColSelectBg].Value.w = 0.25f;
        Colors[ColSelectBorder]     = imgui_style.Colors[ImGuiCol_Border];
    }

    Canvas::~Canvas()
    {
    }

    Client& Client::getInstance()
    {
        static Client instance;
        return instance;
    }

    Client::Client()
        : viewport(ImGui::GetMainViewport())
        , manager(Manager::getInstance())
        , canvas(ImVec2{  50, 50 } // Relative Start Position
               , ImVec2{ 200, 50 } // Grid Spacing
               , ImVec2{ 100, 30 })// Box Size
        , leftMenuWidth(250)
        , dpenMenuHeight(250)
        , nodeList{}
        , selectedNode(nullptr)
        , selectedDpen(nullptr)
        , selectedDeleted(nullptr)
    {}

    Client::~Client()
    {
    }


    bool Client::createWindows()
    {
        // Populate the nodeList on every call.
        nodeList = manager.getNodes();

        masterListWindow();
        graphWindow();
        dpenEditorWindow();

        return true;
    }

    ////////////////////////////////////////////////////////////////////////////
    //                                                                        //
    // ----------------------- // Window functions // ----------------------- //
    //                                                                        //
    ////////////////////////////////////////////////////////////////////////////

    void Client::masterListWindow()
    {
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(ImVec2(leftMenuWidth, viewport->WorkSize.y));

        ImGui::Begin("Master List", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        ImGui::PushItemWidth(leftMenuWidth - 50);

        static char name[32] = "";
        if (ImGui::InputTextWithHint("##newNode", "New Node...", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue) && name[0] != '\0')
        {
            createNode(name);
        }

        ImGui::SameLine(leftMenuWidth - 38);

        if (ImGui::Button("Add##create") && name[0] != '\0')
        {
            createNode(name);
        }


        ImGui::PushItemWidth(-1);

        // Filter. Applies to both the valid and deleted nodes tabs. 
        static char masterListFilter[32] = "";

        ImGui::InputTextWithHint("##masterListFilter", "Filter...", masterListFilter, IM_ARRAYSIZE(masterListFilter));

        if (ImGui::BeginTabBar("##TabBar"))
        {
            if (ImGui::BeginTabItem("Valid Nodes"))
            {
                if (ImGui::BeginListBox("##Nodes", ImVec2(0, viewport->WorkSize.y - 127)))
                {
                    for (auto node : getSearchList(nodeList, masterListFilter))
                    {
                        if (!node->isDeleted())
                        {
                            std::string label = node->isUsable() ? node->getName() : node->getName() + " (unusable)";
                            if (ImGui::Selectable(label.c_str(), selectedNode == node))
                            {
                                selectedNode = node;
                            }
                            if (ImGui::BeginPopupContextItem())
                            {
                                std::string popupLabel = "Delete " + node->getName();
                                if (ImGui::MenuItem(popupLabel.c_str()))
                                {
                                    node->setDeleted(true);
                                    if (node == selectedNode)
                                    {
                                        selectedNode = nullptr;
                                    }
                                }
                                ImGui::EndPopup();
                            }
                        }
                    }
                    ImGui::EndListBox();
                }

                if (ImGui::Button("Delete Selected"))
                    if (selectedNode)
                    {
                        selectedNode->setDeleted(true);
                        selectedNode = nullptr;
                    }

                ImGui::EndTabItem();
            }

            if (ImGui::BeginTabItem("Deleted Nodes##tab2"))
            {
                if (ImGui::BeginListBox("##DeletedNodes", ImVec2(0, viewport->WorkSize.y - 127)))
                {
                    for (auto node : getSearchList(nodeList, masterListFilter))
                    {
                        if (node->isDeleted())
                        {
                            std::string nodeName = node->getName();
                            if (ImGui::Selectable(nodeName.c_str(), selectedDeleted == node))
                            {
                                selectedDeleted = node;
                            }
                            if (ImGui::BeginPopupContextItem())
                            {
                                std::string popupLabel = "Restore " + node->getName();
                                if (ImGui::MenuItem(popupLabel.c_str()))
                                {
                                    node->setDeleted(false);
                                    if (node == selectedDeleted)
                                    {
                                        selectedDeleted = nullptr;
                                    }
                                }
                                ImGui::EndPopup();
                            }
                        }
                    }
                    ImGui::EndListBox();
                }

                if (ImGui::Button("Restore Selected"))
                    if (selectedDeleted != nullptr)
                    {
                        selectedDeleted->setDeleted(false);
                        selectedDeleted = nullptr;
                    }

                ImGui::EndTabItem();
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }

    void Client::graphWindow()
    {
        ImGui::SetNextWindowPos(ImVec2(leftMenuWidth, viewport->WorkPos.y + dpenMenuHeight));
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x - leftMenuWidth, viewport->WorkSize.y - dpenMenuHeight));

        ImGui::Begin("Graph View", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        BeginCanvas();

        if (selectedNode)
            createNodeGraph(selectedNode);

        EndCanvas();
        ImGui::End();
    }

    void Client::dpenEditorWindow()
    {
        // Set the windows position
        ImGui::SetNextWindowPos(ImVec2(leftMenuWidth, 0));
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x - leftMenuWidth, dpenMenuHeight));

        // Vector of dependencies that can be added
        static std::vector<Node*> addDpenVec;
        // Empty if no selected node
        addDpenVec = selectedNode ? addDpenList(selectedNode, nodeList) : std::vector<Node*>();

        ImGui::Begin("Dependency Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

        // Filters. ImGui works with char arrays.
        static char validNodeFilter[32] = "";
        static char dpenNodeFilter[32] = "";

        // Dependencies of the selected node.
        std::vector<Node*> selNodeDpens = {};
        if (selectedNode) selNodeDpens = selectedNode->getDpens();

        const ImGuiStyle& style = ImGui::GetStyle();

        // The individual elements of the editor had to be manually positioned.

        /*
        
        Window layout

        Current Node:
        Valid Nodes:           Current Dependencies
         _________________      _________________
        |filter...        |    |filter...        |
         _________________      _________________
        |Nodes...         | -> |Dpens...         |
        |                 | <- |                 |
        |                 | << |                 |
        |                 |    |                 |
        |                 |    |                 |
        |                 |    |                 |
        |                 |    |                 |
        |                 |    |                 |
        |                 |    |                 |
        |_________________|    |_________________|

        */


        
        // Start position
        ImVec2 curPos(10, 25);

        // listWidth controls the size of the two lists
        const float listWidth = 200.0f;
        ImGui::PushItemWidth(listWidth);

        ImGui::SetCursorPos(curPos);
        // Check if a node is currently selected. Set name to "None" if not
        std::string selectedNodeName = selectedNode ? selectedNode->getName() : "None";
        ImGui::Text("Current Node: %s", selectedNodeName.c_str());

        curPos.y += 15.0f;
        ImGui::SetCursorPos(curPos);
        ImGui::Text("Valid Nodes:");

        curPos.y += 15.0f;
        ImGui::SetCursorPos(curPos);
        ImGui::InputTextWithHint("##validmanagerFilter", "Filter...", validNodeFilter, IM_ARRAYSIZE(validNodeFilter));

        curPos.y += 23.0f;
        ImGui::SetCursorPos(curPos);

        if (ImGui::BeginListBox("##dpens", ImVec2(listWidth, dpenMenuHeight - 87)))
        {
            for (auto& dpen : getSearchList(addDpenVec, validNodeFilter))
            {
                if (ImGui::Selectable(dpen->getName().c_str(), selectedDpen == dpen))
                {
                    selectedDpen = dpen;
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    selectedNode->addDpen(dpen);
                    selectedNode->sortDpens();
                }
            }
            ImGui::EndListBox();
        }

        // Place the buttons between the two lists.
        curPos.x += listWidth + 5.0f;
        ImGui::SetCursorPos(curPos);

        // Add a dpen
        if (ImGui::Button("->##addDpenButton"))
        {
            if (selectedDpen != nullptr)
            {
                selectedNode->addDpen(selectedDpen);
                selectedNode->sortDpens();
            }
        }

        const float btnHgt = 23.0f;

        curPos.y += btnHgt;
        ImGui::SetCursorPos(curPos);

        // Remove an individual dpen
        if (ImGui::Button("<-##removeDpenButton"))
        {
            if (selectedDpen != nullptr)
            {
                selectedNode->removeDpen(selectedDpen);
                selectedNode->sortDpens();
            }
        }

        curPos.y += btnHgt;
        ImGui::SetCursorPos(curPos);

        // This button removes every dpen currently filtered for from the selected node
        if (ImGui::Button("<<##removeAllDpenButton"))
        {
            for (auto& node : getSearchList(selNodeDpens, dpenNodeFilter))
            {
                selectedNode->removeDpen(node);
            }
        }

        // Move back to one row below the starting row to create the dependencies list
        curPos.y = 40.0f;
        curPos.x += 27.0f;
        ImGui::SetCursorPos(curPos);

        ImGui::Text("Current Dependencies:");

        curPos.y += 15.0f;
        ImGui::SetCursorPos(curPos);

        ImGui::InputTextWithHint("##dpenmanagerFilter", "Filter...", dpenNodeFilter, IM_ARRAYSIZE(dpenNodeFilter));

        curPos.y += 23.0f;
        ImGui::SetCursorPos(curPos);

        if (ImGui::BeginListBox("##Current Dependencies", ImVec2(listWidth, dpenMenuHeight - 87)))
        {
            for (auto& node : getSearchList(selNodeDpens, dpenNodeFilter))
            {
                if (ImGui::Selectable(node->getName().c_str(), selectedDpen == node))
                {
                    selectedDpen = node;
                }
                if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
                {
                    selectedNode->removeDpen(node);
                    selectedNode->sortDpens();
                }
            }
            ImGui::EndListBox();
        }
        ImGui::End();
    }

    ////////////////////////////////////////////////////////////////////////////////
    //                                                                            //
    // ----------------------- // Graph View functions // ----------------------- //
    //                                                                            //
    ////////////////////////////////////////////////////////////////////////////////

    void Client::BeginCanvas()
    {
        canvas.numOfNodes = 0;

        const ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImGui::PushID(&canvas);

        ImGui::ItemAdd(w->ContentRegionRect, ImGui::GetID("canvas"));

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 wndSize = ImGui::GetWindowSize();

        ImVec2 bounds = ImVec2(-1, -1) * (canvas.relStartPos * ImVec2(2, 2) + canvas.boxSize + canvas.bounds * canvas.gridSpacing - wndSize);

        if (!ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            if (ImGui::IsMouseDragging(2))
                canvas.Offset += io.MouseDelta;

            if (io.KeyShift && !io.KeyCtrl)
                canvas.Offset.x += io.MouseWheel * 24.0f;

            if (!io.KeyShift && !io.KeyCtrl)
            {
                canvas.Offset.y += io.MouseWheel * 24.0f;
                canvas.Offset.x += io.MouseWheelH * 24.0f;
            }
        }

        if (canvas.Offset.x < bounds.x) canvas.Offset.x = bounds.x;
        if (canvas.Offset.y < bounds.y) canvas.Offset.y = bounds.y;

        if (canvas.Offset.x > 0) canvas.Offset.x = 0;
        if (canvas.Offset.y > 0) canvas.Offset.y = 0;
        
        canvas.bounds = ImVec2(0, 0);
    }

    void Client::EndCanvas()
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();

        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopID();     // canvas
    }

    float Client::createNodeGraph(Node* node, ImVec2 curPos, Node* parent, bool isRoot)
    {
        ImVec2 parentPos = curPos * canvas.gridSpacing + canvas.relStartPos;

        if (nodeBox(node->getName(), parentPos, node, parent, isRoot))
        {
            // int used to generate a unique ID for every node in the graph. Otherwise multiple instances of the same node would cause bugs due to shared IDs
            canvas.numOfNodes++;
        }

        // Prevent the node graph from leaving the viewable area
        if (curPos.x > canvas.bounds.x) canvas.bounds.x = curPos.x;
        if (curPos.y > canvas.bounds.y) canvas.bounds.y = curPos.y;

        // Iterate through each dpen of the current node and display them
        auto dpenVec = node->getDpens();

        if (!dpenVec.empty() && !node->isDeleted())
        {
            curPos.y--;
            curPos.x++;
            for (auto& dpen : dpenVec)
            {
                curPos.y++;

                ImVec2 childPos = curPos * canvas.gridSpacing + canvas.relStartPos;

                renderConnection(parentPos, childPos);

                curPos.y = createNodeGraph(dpen, curPos, node, false);
            }
        }
        return curPos.y;
    }

    // Draw a line from the parent node to the child node
    void Client::renderConnection(const ImVec2& parentPos, const ImVec2& childPos)
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const ImGuiStyle& style = ImGui::GetStyle();
        const ImVec2 winPos = ImGui::GetWindowPos();
        const auto col = canvas.Colors[ColConnection];
        float thickness = 2.0f;

        // Create screenspace coordinates for child and parent
        ImVec2 parent = winPos + ImVec2{ style.ItemInnerSpacing.x, 0 } + (parentPos + ImVec2{ canvas.boxSize.x, canvas.boxSize.y / 2 }) + canvas.Offset;
        ImVec2 child = winPos + (childPos + ImVec2{ 0, canvas.boxSize.y / 2 }) + canvas.Offset;

        // Create intermediate coordinates.
        ImVec2 p2 = parent - ImVec2{ ((parent.x - child.x) / 2), 0 };
        ImVec2 p3 = child + ImVec2{ ((parent.x - child.x) / 2), 0 };

        // Draw the line.
        draw_list->PathLineTo(parent);
        // The line is drawn wavy if the middle two points are the same.
        if (p2.y != p3.y)
        {
            draw_list->PathLineTo(p2);
            draw_list->PathLineTo(p3);
        }
        draw_list->PathLineTo(child);
        draw_list->PathStroke(col, 0, thickness);
    }

    
    bool Client::nodeBox(std::string label, const ImVec2 pos, Node* node, Node* parent, bool isRoot)
    {
        const ImGuiStyle& style = ImGui::GetStyle();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        ImColor bgCol = isRoot ? canvas.Colors[ColSelectBg] : canvas.Colors[ColNodeBg];
        ImColor borderCol;
        if (!node->isUsable() || node->isDeleted())
        {
            borderCol = canvas.Colors[ColConnectionActive];
        }
        else if (isRoot)
        {
            borderCol = canvas.Colors[ColCanvasLines];
        }
        else
        {
            borderCol = canvas.Colors[ColNodeBorder];
        }

        // 0 - node rect, curves
        // 1 - node content
        draw_list->ChannelsSplit(2);

        const ImVec2 winPos = ImGui::GetWindowPos();

        // Top-let corner of the node
        ImGui::SetCursorScreenPos(winPos + pos + canvas.Offset);
        ImGui::PushID(window->GetID(label.c_str()));

        draw_list->ChannelsSetCurrent(1);

        // If the label is too long, replace everything that doesn't fit with "...";
        std::string boxLabel;
        if (label.size() > 14)
        {
            boxLabel = label.substr(0, 11) + "...";
        }
        else
        {
            boxLabel = label;
        }
        ImGui::Text(boxLabel.c_str());

        draw_list->ChannelsSetCurrent(0);

        ImRect node_rect{
            winPos - style.ItemInnerSpacing + pos + canvas.Offset,
            winPos + style.ItemInnerSpacing + canvas.boxSize + pos + canvas.Offset
        };

        draw_list->AddRectFilled(node_rect.Min, node_rect.Max, bgCol, style.FrameRounding);
        draw_list->AddRect(node_rect.Min, node_rect.Max, borderCol, style.FrameRounding);

        ImGui::ItemAdd(node_rect, window->GetID(canvas.numOfNodes));

        // Select a non deleted node
        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0) && !node->isDeleted())
        {
            selectedNode = node;
        }

        if (ImGui::BeginPopupContextItem())
        {
            // Full name of node. Useful when a node's name is too long.
            ImGui::TextColored(ImVec4(0.5, 0.9, 1, 1), label.c_str());
            if (!isRoot)
            {
                // Remove a dpen from its parent
                std::string removeLabel = "Break Connection: " + parent->getName();
                if (ImGui::MenuItem(removeLabel.c_str()))
                {
                    parent->removeDpen(node);
                }
            }
            // Delete a node
            if (!node->isDeleted())
            {
                if(ImGui::MenuItem("Delete"))
                {
                    node->setDeleted(true);
                    if (isRoot) selectedNode = nullptr;
                }
            }
            // Restore a deleted node
            else
            {
                if (ImGui::MenuItem("Restore"))
                {
                    node->setDeleted(false);
                }
            }
            ImGui::EndPopup();
        }

        draw_list->ChannelsMerge();

        ImGui::PopID();

        return true;
    }

    //////////////////////////////////////////////////////////////////////////
    //                                                                      //
    // ---------------------- // Helper functions // ---------------------- //
    //                                                                      //
    //////////////////////////////////////////////////////////////////////////

    std::vector<Node*> Client::addDpenList(Node* selectedNode, std::vector<Node*> nodeVec)
    {
        std::vector<Node*> dpenList;
        auto dpenVec = selectedNode->getDpens();
        bool valid = true;
        for (auto& node : nodeVec)
        {
            if (node != selectedNode && !node->isDeleted())
            {
                valid = true;
                for (auto& dpen : dpenVec)
                {
                    if (dpen == node) valid = false;
                }
                if (valid) dpenList.push_back(node);
            }
        }
        return dpenList;
    }


    std::vector<Node*> Client::getSearchList(std::vector<Node*> nodes, char search[])
    {
        std::string filter;
        for (unsigned int i = 0; i < strlen(search); i++)
        {
            filter += tolower(search[i]);
        }

        std::vector<Node*> filteredList;
        for (auto& node : nodes)
        {
            if (strstr(node->getSortName().c_str(), filter.c_str()))
            {
                filteredList.push_back(node);
            }
        }
        return filteredList;
    }

    void Client::createNode(char name[])
    {
        for (int i = 0; i < strlen(name); i++)
        {
            if (name[i] == ' ')
            {
                name[i] = '_';
            }
        }
        manager.addNode(name, false);
        manager.sortNodes();
        name[0] = 0;
    }
}



