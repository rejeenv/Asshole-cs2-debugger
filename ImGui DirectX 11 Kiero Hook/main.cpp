#include "includes.h"
#include "src/cheat.h"
#include "src/memory.h"
#include <sstream>
#include <iomanip>

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

extern uintptr_t g_local_player_pawn_address;
extern uintptr_t g_entity_list_address;
extern std::atomic<bool> g_running;
extern uintptr_t v::client;

Present oPresent = nullptr;
HWND window = nullptr;
WNDPROC oWndProc = nullptr;
ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pContext = nullptr;
ID3D11RenderTargetView* mainRenderTargetView = nullptr;
bool init = false;

void SetupImGuiStyle()
{
	ImGuiStyle& style = ImGui::GetStyle();

	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(10.0f, 10.0f);
	style.WindowRounding = 10.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(20.0f, 32.0f);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.WindowMenuButtonPosition = ImGuiDir_None;
	style.ChildRounding = 10.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 10.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(15.0f, 5.8f);
	style.FrameRounding = 6.0f;
	style.FrameBorderSize = 0.0f;
	style.ItemSpacing = ImVec2(4.0f, 6.0f);
	style.ItemInnerSpacing = ImVec2(10.0f, 4.0f);
	style.IndentSpacing = 20.0f;
	style.ColumnsMinSpacing = 4.0f;
	style.ScrollbarSize = 20.0f;
	style.ScrollbarRounding = 10.0f;
	style.GrabMinSize = 10.0f;
	style.GrabRounding = 10.0f;
	style.TabRounding = 6.0f;
	style.TabBorderSize = 0.0f;
	style.ColorButtonPosition = ImGuiDir_Right;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

	style.Colors[ImGuiCol_Text] = ImVec4(0.9490196f, 0.9490196f, 0.9490196f, 1.0f);
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.9490196f, 0.9490196f, 0.9490196f, 0.34509805f);
	style.Colors[ImGuiCol_WindowBg] = ImVec4(0.47843137f, 0.34901962f, 0.45882353f, 1.0f);
	style.Colors[ImGuiCol_ChildBg] = ImVec4(0.47843137f, 0.34901962f, 0.45882353f, 1.0f);
	style.Colors[ImGuiCol_PopupBg] = ImVec4(0.47843137f, 0.34901962f, 0.45882353f, 1.0f);
	style.Colors[ImGuiCol_Border] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	style.Colors[ImGuiCol_FrameBg] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.8197425f);
	style.Colors[ImGuiCol_TitleBg] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.6039216f, 0.41960785f, 0.5764706f, 0.3019608f);
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.6039216f, 0.41960785f, 0.5764706f, 0.4f);
	style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.6039216f, 0.41960785f, 0.5764706f, 0.6f);
	style.Colors[ImGuiCol_CheckMark] = ImVec4(0.6039216f, 0.41960785f, 0.5764706f, 1.0f);
	style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.6039216f, 0.41960785f, 0.5764706f, 0.3019608f);
	style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.6039216f, 0.41960785f, 0.5764706f, 0.6f);
	style.Colors[ImGuiCol_Button] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.4f);
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.6f);
	style.Colors[ImGuiCol_Header] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_Separator] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.4549356f);
	style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.7811159f);
	style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.9019608f);
	style.Colors[ImGuiCol_Tab] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 1.0f);
	style.Colors[ImGuiCol_TabHovered] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.8f);
	style.Colors[ImGuiCol_TabActive] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.84313726f);
	style.Colors[ImGuiCol_TabUnfocused] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.81960785f);
	style.Colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.8352941f);
	style.Colors[ImGuiCol_PlotLines] = ImVec4(0.9490196f, 0.9490196f, 0.9490196f, 1.0f);
	style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.47843137f, 0.34901962f, 0.45882353f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.60784316f, 0.42745098f, 0.5803922f, 1.0f);
	style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.60784316f, 0.42745098f, 0.5803922f, 0.639485f);
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.6039216f, 0.41960785f, 0.5764706f, 0.6f);
	style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.60784316f, 0.42745098f, 0.5803922f, 1.0f);
	style.Colors[ImGuiCol_NavHighlight] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.8f);
	style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.7019608f);
	style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.2f);
	style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.16078432f, 0.10980392f, 0.1764706f, 0.34901962f);
}


void InitImGui()
{
	if (ImGui::GetCurrentContext() == nullptr)
		ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;
	io.IniFilename = nullptr;

	SetupImGuiStyle();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(pDevice, pContext);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
        return true;

    return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}


HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    if (!init)
    {
        if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&pDevice)))
        {
            pDevice->GetImmediateContext(&pContext);

            DXGI_SWAP_CHAIN_DESC sd;
            pSwapChain->GetDesc(&sd);
            window = sd.OutputWindow;

            ID3D11Texture2D* pBackBuffer = nullptr;
            pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
            pDevice->CreateRenderTargetView(pBackBuffer, nullptr, &mainRenderTargetView);
            pBackBuffer->Release();

            oWndProc = (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
            InitImGui();
            init = true;
        }

        return oPresent(pSwapChain, SyncInterval, Flags);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Asshole Debugger", nullptr, ImGuiWindowFlags_AlwaysAutoResize);

    ImGui::Text("Client.dll: 0x%llX", v::client);
    ImGui::Separator();
    ImGui::Text("LocalPlayer : 0x%llX", g_local_player_pawn_address);
    ImGui::Text("EntityList  : 0x%llX", g_entity_list_address);
    ImGui::Separator();

    ImGui::End();

    ImGui::Render();

    pContext->OMSetRenderTargets(1, &mainRenderTargetView, nullptr);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return oPresent(pSwapChain, SyncInterval, Flags);
}

DWORD WINAPI MainThread(LPVOID lpReserved)
{

    CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)v::mainVoider, nullptr, 0, nullptr);

    // Hook Present
    while (kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success)
        Sleep(100);

    kiero::bind(8, (void**)&oPresent, hkPresent);

    return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved)
{
    switch (dwReason)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hMod);
        CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
        break;

    case DLL_PROCESS_DETACH:
        g_running.store(false);
        kiero::shutdown();
        break;
    }
    return TRUE;
}