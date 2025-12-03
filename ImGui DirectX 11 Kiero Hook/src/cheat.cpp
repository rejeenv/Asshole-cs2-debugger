#include "cheat.h"
#include "memory.h"
#include <thread>
#include <chrono>

std::atomic<bool> g_running{ true };

uintptr_t g_local_player_pawn_address = 0;
uintptr_t g_entity_list_address = 0;

namespace v {
    uintptr_t client = 0; 
}

void v::mainVoider() {
    while (g_running.load()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(4));

        v::client = (uintptr_t)GetModuleHandleA("client.dll");
        if (!v::client) continue;

        uintptr_t localPawn = Memory::Read<uintptr_t>(v::client + o::dwLocalPlayerPawn);
        uintptr_t entityList = Memory::Read<uintptr_t>(v::client + o::dwEntityList);

        g_local_player_pawn_address = localPawn;
        g_entity_list_address = entityList;

        if (!localPawn || !entityList) continue;

    }
}
