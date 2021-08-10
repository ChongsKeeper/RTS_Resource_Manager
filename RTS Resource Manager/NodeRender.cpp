#ifndef IMGUI_DEFINE_MATH_OPERATORS
#   define IMGUI_DEFINE_MATH_OPERATORS
#endif


#include "NodeRender.h"

namespace NodeRender
{
    CanvasState* gCanvas = nullptr;
    const ImVec2 relStartPos{ 50, 50 }; // starting position for the root node of the tree
    const ImVec2 gridSpacing{ 200, 50 }; // grid offset for node spacing
    const ImVec2 boxSize{ 100, 30 };
    int numOfNodes = 0;
    Node* selectedNode = nullptr;

    ImVec2 canvasBounds;

    struct _CanvasStateImpl
    {
        // Previous canvas pointer. Used to restore proper gCanvas value when nesting canvases.
        CanvasState* PrevCanvas = nullptr;
    };


    CanvasState::CanvasState() noexcept
    {
        _Impl = new _CanvasStateImpl();

        auto& imgui_style = ImGui::GetStyle();
        Colors[ColCanvasLines] = imgui_style.Colors[ImGuiCol_Separator];
        Colors[ColNodeBg] = imgui_style.Colors[ImGuiCol_WindowBg];
        Colors[ColNodeActiveBg] = imgui_style.Colors[ImGuiCol_FrameBgActive];
        Colors[ColNodeBorder] = imgui_style.Colors[ImGuiCol_Border];
        Colors[ColConnection] = imgui_style.Colors[ImGuiCol_PlotLines];
        Colors[ColConnectionActive] = imgui_style.Colors[ImGuiCol_PlotLinesHovered];
        Colors[ColSelectBg] = imgui_style.Colors[ImGuiCol_FrameBgActive];
        Colors[ColSelectBg].Value.w = 0.25f;
        Colors[ColSelectBorder] = imgui_style.Colors[ImGuiCol_Border];
    }


    CanvasState::~CanvasState()
    {
        delete _Impl;
    }

    void BeginCanvas(CanvasState* canvas)
    {
        numOfNodes = 0;
        canvas->_Impl->PrevCanvas = gCanvas;
        gCanvas = canvas;
        const ImGuiWindow* w = ImGui::GetCurrentWindow();
        ImGui::PushID(canvas);

        ImGui::ItemAdd(w->ContentRegionRect, ImGui::GetID("canvas"));

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImGuiIO& io = ImGui::GetIO();

        ImVec2 winSize = ImGui::GetWindowSize();

        ImVec2 bounds = ImVec2(-1, -1) * (relStartPos * ImVec2(2, 2) + boxSize + canvasBounds * gridSpacing - winSize);

        if (!ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
        {
            if (ImGui::IsMouseDragging(2))
                canvas->Offset += io.MouseDelta;

            if (io.KeyShift && !io.KeyCtrl)
                canvas->Offset.x += io.MouseWheel * 24.0f;

            if (!io.KeyShift && !io.KeyCtrl)
            {
                canvas->Offset.y += io.MouseWheel * 24.0f;
                canvas->Offset.x += io.MouseWheelH * 24.0f;
            }
        }

        

        if (canvas->Offset.x < bounds.x) canvas->Offset.x = bounds.x;
        if (canvas->Offset.y < bounds.y) canvas->Offset.y = bounds.y;

        if (canvas->Offset.x > 0) canvas->Offset.x = 0;
        if (canvas->Offset.y > 0) canvas->Offset.y = 0;
        
        canvasBounds = ImVec2(0, 0);
    }

    void EndCanvas()
    {
        assert(gCanvas != nullptr);

        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        auto* canvas = gCanvas;
        auto* impl = canvas->_Impl;

        ImGui::SetWindowFontScale(1.f);
        ImGui::PopID();     // canvas
        gCanvas = impl->PrevCanvas;
    }

    void CenterCanvas()
    {
        assert(gCanvas != nullptr);
        gCanvas->Offset = ImVec2{ 0.0f, 0.0f };
    }

    bool nodeBox(const char* label, const ImVec2 pos, Node* node, bool isRoot)
    {
        assert(gCanvas != nullptr);

        const ImGuiStyle& style = ImGui::GetStyle();
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        auto* canvas = gCanvas;
        ImGuiWindow* window = ImGui::GetCurrentWindow();

        auto bgCol = isRoot ? canvas->Colors[ColSelectBg] : canvas->Colors[ColNodeBg];
        auto borderCol = isRoot ? canvas->Colors[ColCanvasLines] : canvas->Colors[ColNodeBorder];

        if (node->isDeleted())
        {
            borderCol = canvas->Colors[ColConnectionActive];
        }

        // 0 - node rect, curves
        // 1 - node content
        draw_list->ChannelsSplit(2);

        const ImVec2 winPos = ImGui::GetWindowPos();

        // Top-let corner of the node
        ImGui::SetCursorScreenPos(winPos + pos + canvas->Offset);
        ImGui::PushID(window->GetID(label));

        draw_list->ChannelsSetCurrent(1);

        ImGui::Text(label);

        draw_list->ChannelsSetCurrent(0);

        ImRect node_rect{
            winPos - style.ItemInnerSpacing + pos + canvas->Offset,
            winPos + style.ItemInnerSpacing + boxSize + pos + canvas->Offset
        };

        draw_list->AddRectFilled(node_rect.Min, node_rect.Max, bgCol, style.FrameRounding);
        draw_list->AddRect(node_rect.Min, node_rect.Max, borderCol, style.FrameRounding);

        ImGui::ItemAdd(node_rect, window->GetID(numOfNodes));
        
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::MenuItem("Focus"))
            {
                selectedNode = node;
            }
            ImGui::EndPopup();
        }

        draw_list->ChannelsMerge();

        ImGui::PopID();

        return true;
    }


    void renderConnection(const ImVec2& parentPos, const ImVec2& childPos)
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        CanvasState* canvas = gCanvas;
        const ImGuiStyle& style = ImGui::GetStyle();
        const ImVec2 winPos = ImGui::GetWindowPos();
        const auto col = canvas->Colors[ColConnection];
        float thickness = 2.0f;

        ImVec2 parent = winPos + ImVec2{ style.ItemInnerSpacing.x, 0 } + (parentPos + ImVec2{ boxSize.x, boxSize.y / 2 }) + canvas->Offset;
        ImVec2 child = winPos + (childPos + ImVec2{ 0, boxSize.y / 2 }) + canvas->Offset;

        ImVec2 p2 = parent - ImVec2{ (parent.x - child.x) / 2, 0 };
        ImVec2 p3 = child + ImVec2{ (parent.x - child.x) / 2, 0 };

        //draw_list->AddBezierCubic(parent, p2, p3, child, canvas->Colors[ColConnection], thickness, 0);
        /*draw_list->AddLine(parent, p2, col, thickness);
        draw_list->AddLine(p2, p3, col, thickness);
        draw_list->AddLine(p3, child, col, thickness);*/

        draw_list->PathLineTo(parent);
        if (p2.y != p3.y)
        {
            draw_list->PathLineTo(p2);
            draw_list->PathLineTo(p3);
        }
        draw_list->PathLineTo(child);
        draw_list->PathStroke(col, 0, thickness);
    }


    float createNodeGraph(Node *node, ImVec2 curPos, bool isRoot)
    {
        ImVec2 parentPos = curPos * gridSpacing + relStartPos;
        if (nodeBox(node->getName().c_str(), parentPos, node, isRoot))
            numOfNodes++; // int used to generate a unique ID for every node in the graph. Otherwise multiple instances of the same node would cause bugs due to shared IDs

        if (curPos.x > canvasBounds.x) canvasBounds.x = curPos.x;
        if (curPos.y > canvasBounds.y) canvasBounds.y = curPos.y;

        auto dpenVec = node->getDpens();

        if (dpenVec.empty() == false && node->isDeleted() == false)
        {
            curPos.y -= 1;
            curPos.x += 1;
            for (auto& dpen : dpenVec)
            {
                curPos.y += 1;

                ImVec2 childPos = curPos * gridSpacing + relStartPos;

                renderConnection(parentPos, childPos);

                curPos.y = createNodeGraph(dpen, curPos, false);
            }
        }
        return curPos.y;
    }

    Node* setSelected(Node* node)
    {
        selectedNode = node;
        return selectedNode;
    }

    Node* getSelected()
    {
        return selectedNode;
    }
}