#pragma once

#include <vector>
#include "Node.h"
#include "imgui_internal.h"
#include "imgui.h"


namespace NodeRender
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

    struct _CanvasStateImpl;

	struct IMGUI_API CanvasState
	{
		// Current scroll offset of canvas.
        ImVec2 Offset;
        // Colors used to style elements of this canvas.
        ImColor Colors[StyleColor::ColMax];

        // Implementation detail.
        _CanvasStateImpl* _Impl = nullptr;

        CanvasState() noexcept;
        ~CanvasState();
	};


    // Create a node graph canvas in current window.
    IMGUI_API void BeginCanvas(CanvasState* canvas);
    // Terminate a node graph canvas that was created by calling BeginCanvas().
    IMGUI_API void EndCanvas();
    IMGUI_API void CenterCanvas();
    /// Begin rendering of node in a graph. Render node content when returns `true`.
    float createNodeGraph(Node* node, ImVec2 curPos = ImVec2{ 0, 0 }, bool isRoot = true);

    Node* setSelected(Node* node);
    Node* getSelected();
}
