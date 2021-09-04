#pragma once

#include <vector>
#include <cstdlib>
#include "Node.h"
#include "Manager.h"
#include "imgui_internal.h"
#include "imgui.h"


namespace NodeGUI
{
    enum StyleColor
    {
        ColCanvasLines,
        ColNodeBg,
        ColNodeActiveBg,
        ColNodeBorder,
        ColConnection,
        ColConnectionActive,
        ColSelectBg,
        ColSelectBorder,
        ColMax
    };

	struct IMGUI_API Canvas
	{
        ImGuiStyle& imgui_style;
		// Current scroll offset of canvas.
        ImVec2 Offset;
        // Colors used to style elements of this canvas.
        ImColor Colors[StyleColor::ColMax];

        ImVec2 relStartPos;
        ImVec2 gridSpacing;
        ImVec2 boxSize;

        ImVec2 bounds;

        int numOfNodes;

        Canvas(ImVec2 relStartPos, ImVec2 gridSpacing, ImVec2 boxSize);
        ~Canvas();
	};


    // Singleton class that handles the GUI Client
    class Client
    {
    public:
        // Returns the static instance
        static Client& getInstance();

        ~Client();
        
        // Creates the three windows
        bool createWindows();

        // Disable the copy constructor and assignment operator.
        Client(Client const&)         = delete;
        void operator=(Client const&) = delete;

    private:
        // Private constructor so that you can't instantiate the class without using getInstance()
        Client();

        // The three windows
        void masterListWindow();
        void graphWindow();
        void dpenEditorWindow();

        // Create a node graph canvas in current window.
        void BeginCanvas();

        // Terminate a node graph canvas that was created by calling BeginCanvas().
        void EndCanvas();

        // 
        float createNodeGraph(Node* node, ImVec2 curPos = ImVec2{ 0, 0 }, Node* parent = nullptr, bool isRoot = true);

        // Draw the lines between each Node Box
        void renderConnection(const ImVec2& parentPos, const ImVec2& childPos);

        // Draw a Node Box
        bool nodeBox(std::string label, const ImVec2 pos, Node* node, Node* parent, bool isRoot);

        // The list of dpens that are available to add to the currently selected Node
        std::vector<Node*> addDpenList(Node* selectedNode, std::vector<Node*> nodeVec);

        // Returns a filtered Node list
        std::vector<Node*> getSearchList(std::vector<Node*> nodes, char search[]);

        void createNode(char name[]);

    private:
       
        ImGuiViewport* viewport;
        Manager& manager;
        Canvas canvas;

        const float leftMenuWidth;
        const float dpenMenuHeight;

        std::vector<Node*> nodeList;
        Node* selectedNode;
        Node* selectedDpen;
        Node* selectedDeleted;
    };
}
