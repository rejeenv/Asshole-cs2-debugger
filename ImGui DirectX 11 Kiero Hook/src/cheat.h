#pragma once

#include <cstdint>
#include <atomic>
#include <Windows.h>

extern std::atomic<bool> g_running;

extern uintptr_t g_local_player_pawn_address;
extern uintptr_t g_entity_list_address;

namespace g {
    inline int  crosshair_id = -1;
    inline bool friendly_fire = false;
    inline bool triggerbot = true;
    inline bool esp = true;
}

// update offsets!!
namespace o {
    inline constexpr uintptr_t dwLocalPlayerPawn = 0x1834178;
    inline constexpr uintptr_t dwEntityList = 0x19BAA98;
    inline constexpr uintptr_t dwViewMatrix = 0x1A2FF60;

    inline constexpr uintptr_t m_iIDEntIndex = 0x153C;
    inline constexpr uintptr_t m_iTeamNum = 0x3BF;
    inline constexpr uintptr_t m_iHealth = 0x34C;
    inline constexpr uintptr_t m_lifeState = 0x338;
    inline constexpr uintptr_t m_vecOrigin = 0x1274;
    inline constexpr uintptr_t m_hPlayerPawn = 0x5F4;
}

namespace v {
    extern uintptr_t client;
    void mainVoider();
}
