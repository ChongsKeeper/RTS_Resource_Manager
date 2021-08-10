#include "main.h"

// Data
static ID3D11Device* g_pd3dDevice = NULL;
static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;


// Main code
int main(int, char**)
{

    Manager nodeList;

    nodeList.importFile("resource_list.txt");

    nodeList.printNodes();

    nodeList.exportFile("test.txt");

    // Create application window
    //ImGui_ImplWin32_EnableDpiAwareness();
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = ::CreateWindow(wc.lpszClassName, _T("Allan Nilsson Portfolio Submission - RTS Resource Editor"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(hwnd))
    {
        CleanupDeviceD3D();
        ::UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ::ShowWindow(hwnd, SW_SHOWDEFAULT);
    ::UpdateWindow(hwnd);

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);


    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop
    bool done = false;
    while (!done)
    {
        
        // Poll and handle messages (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        MSG msg;
        while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }
        if (done)
            break;

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        const ImGuiViewport* viewport = ImGui::GetMainViewport();

        const float leftMenuWidth = 250;
        const float dpenMenuHeight = 250;
        
        static Node* selectedDpen;
        auto nodeVec = nodeList.getNodes();
        static std::vector<Node*> dpenVec;
        static bool showGraphView = false;
        static Node* selectedNode = nodeVec[0];
        bool add = false;
        static bool showAddDpen = false;
        static std::vector<Node*> masterList = nodeVec;
        {

            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2(leftMenuWidth, viewport->WorkSize.y));

            ImGui::Begin("Master List", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);

            static char name[32] = "";

            ImGui::PushItemWidth(leftMenuWidth - 50);

            if (ImGui::InputTextWithHint("##newNode", "New Node...", name, IM_ARRAYSIZE(name), ImGuiInputTextFlags_EnterReturnsTrue) && name[0] != '\0')
            {
                nodeList.addNode(name, false);
                nodeList.sortNodes();
                name[0] = 0;
            }

            ImGui::SameLine(leftMenuWidth - 38);

            if (ImGui::Button("Add##create") && name[0] != '\0')
            {
                nodeList.addNode(name, false);
                nodeList.sortNodes();
                name[0] = 0;
            }

            ImGui::PushItemWidth(-1);

            static char masterListFilter[32] = "";
            ImGui::InputTextWithHint("##masterListFilter", "Filter...", masterListFilter, IM_ARRAYSIZE(masterListFilter));
            
            if (ImGui::BeginTabBar("##TabBar"))
            {
                if (ImGui::BeginTabItem("Valid Nodes"))
                {
                    if (ImGui::BeginListBox("##Nodes", ImVec2(0, viewport->WorkSize.y - 127)))
                    {
                        for (auto node : getSearchList(nodeVec, masterListFilter))
                        {
                            if (!node->isDeleted())
                            {
                                std::string nodeName = node->getName();
                                if (ImGui::Selectable(nodeName.c_str(), selectedNode == node))
                                {
                                    selectedNode = NodeRender::setSelected(node);
                                    dpenVec = selectedNode->getDpens();
                                }
                            }
                        }
                        ImGui::EndListBox();
                    }

                    if (ImGui::Button("Delete Selected")) selectedNode->setDeleted(true);

                    ImGui::EndTabItem();
                }

                if (ImGui::BeginTabItem("Deleted Nodes##tab2"))
                {
                    if (ImGui::BeginListBox("##DeletedNodes", ImVec2(0, viewport->WorkSize.y - 127)))
                    {
                        for (auto node : getSearchList(nodeVec, masterListFilter))
                        {
                            if (node->isDeleted())
                            {
                                std::string nodeName = node->getName();
                                if (ImGui::Selectable(nodeName.c_str(), selectedNode == node))
                                {
                                    selectedNode = NodeRender::setSelected(node);
                                    dpenVec = selectedNode->getDpens();
                                }
                            }
                        }
                        ImGui::EndListBox();
                    }

                    if (ImGui::Button("Restore Selected")) selectedNode->setDeleted(false);

                    ImGui::EndTabItem();
                }

                ImGui::EndTabBar();
            }

            ImGui::End();

            static NodeRender::CanvasState canvas;
              
            ImGui::SetNextWindowPos(ImVec2(leftMenuWidth, viewport->WorkPos.y + dpenMenuHeight));
            ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x - leftMenuWidth, viewport->WorkSize.y - dpenMenuHeight));

            if (!ImGui::Begin("Graph View", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
            {
                ImGui::End();
            }
            else
            {
                NodeRender::BeginCanvas(&canvas);

                if (selectedNode != nullptr)
                    NodeRender::createNodeGraph(selectedNode);

                NodeRender::EndCanvas();

                ImGui::End();
            }           
            
            
            static std::vector<Node*> addDpenVec;

            ImGui::SetNextWindowPos(ImVec2(leftMenuWidth, 0));
            ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x - leftMenuWidth, dpenMenuHeight));

            addDpenVec = addDpenList(selectedNode, nodeVec);

            if (!ImGui::Begin("Dependency Editor", nullptr, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse))
            {
                ImGui::End();
            }
            else
            {
                static char validNodeListFilter[32] = "";
                static char dpenNodeListFilter[32] = "";

                auto selNodeDpens = selectedNode->getDpens();

                const ImGuiStyle& style = ImGui::GetStyle();

                ImVec2 curPos(10, 25);

                const float listWidth = 200.0f;
                ImGui::PushItemWidth(listWidth);

                ImGui::SetCursorPos(curPos);
                ImGui::Text("Current Node: %s", selectedNode->getName().c_str());

                curPos.y += 15;
                ImGui::SetCursorPos(curPos);
                ImGui::Text("Valid Nodes:");
                
                curPos.y += 15;
                ImGui::SetCursorPos(curPos);
                ImGui::InputTextWithHint("##validNodeListFilter", "Filter...", validNodeListFilter, IM_ARRAYSIZE(validNodeListFilter));

                curPos.y += 23;
                ImGui::SetCursorPos(curPos);

                if (ImGui::BeginListBox("##dpens", ImVec2(listWidth, dpenMenuHeight - 87)))
                {
                    for (auto& dpen : getSearchList(addDpenVec, validNodeListFilter))
                    {
                        if (ImGui::Selectable(dpen->getName().c_str(), selectedDpen == dpen))
                        {
                            selectedDpen = dpen;
                        }
                    }
                    ImGui::EndListBox();
                }

                curPos.x += listWidth + 5;
                ImGui::SetCursorPos(curPos);

                if (ImGui::Button("->##addDpenButton"))
                {
                    if (selectedDpen != nullptr)
                    {
                        selectedNode->addDpen(selectedDpen);
                        selectedNode->sortDpens();
                    }
                }

                const float btnHgt = 23;

                curPos.y += btnHgt;
                ImGui::SetCursorPos(curPos);

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

                if (ImGui::Button("<<##removeAllDpenButton"))
                {
                    for (auto& node : getSearchList(selNodeDpens, dpenNodeListFilter))
                    {
                        selectedNode->removeDpen(node);
                    }
                }

                curPos.y = 40;
                curPos.x += 27;
                ImGui::SetCursorPos(curPos);

                ImGui::Text("Current Dependencies:");

                curPos.y += 15;
                ImGui::SetCursorPos(curPos);

                ImGui::InputTextWithHint("##dpenNodeListFilter", "Filter...", dpenNodeListFilter, IM_ARRAYSIZE(dpenNodeListFilter));

                curPos.y += 23;
                ImGui::SetCursorPos(curPos);

                if (ImGui::BeginListBox("##Current Dependencies", ImVec2(listWidth, dpenMenuHeight - 87)))
                {
                    for (auto& node : getSearchList(selNodeDpens, dpenNodeListFilter))
                    {
                        if (ImGui::Selectable(node->getName().c_str(), selectedDpen == node))
                        {
                            selectedDpen = node;
                        }
                    }

                    ImGui::EndListBox();

                }

                ImGui::End();
            }
        }

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
    }

    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

// Helper functions

bool CreateDeviceD3D(HWND hWnd)
{
    // Setup swap chain
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory(&sd, sizeof(sd));
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;
    //createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
    if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
        return false;

    CreateRenderTarget();
    return true;
}

void CleanupDeviceD3D()
{
    CleanupRenderTarget();
    if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
    if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
    if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
}

void CreateRenderTarget()
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
    g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
    pBackBuffer->Release();
}

void CleanupRenderTarget()
{
    if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Win32 message handler
LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
            CreateRenderTarget();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
            return 0;
        break;
    case WM_DESTROY:
        ::PostQuitMessage(0);
        return 0;
    }
    return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

std::vector<Node*> addDpenList(Node* selectedNode, std::vector<Node*> nodeVec)
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


std::vector<Node*> getSearchList(std::vector<Node*> nodes, char search[])
{

    for (unsigned int i = 0; i < strlen(search); i++)
    {
        search[i] = tolower(search[i]);
    }

    std::vector<Node*> filteredList;
    for (auto& node : nodes)
    {
        if (strstr(node->getSortName().c_str(), search))
        {
            filteredList.push_back(node);
        }
    }
    return filteredList;
}

