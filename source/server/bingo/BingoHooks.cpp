#include "heap/seadHeap.h"
#include "heap/seadHeapMgr.h"
#include "server/Client.hpp"
#include "server/bingo/BingoMode.hpp"
#include "server/gamemode/GameModeManager.hpp"

static bool isBingoActive() {
    return GameModeManager::instance()->isMode(GameMode::BINGO);
}

