#include <cmath>
#include "server/bingo/BingoConfigMenu.hpp"
#include "server/bingo/BingoMode.hpp"
#include "server/gamemode/GameModeManager.hpp"
#include "logger.hpp"
#include "server/Client.hpp"

BingoConfigMenu::BingoConfigMenu() : GameModeConfigMenu() {}

void BingoConfigMenu::initMenu(const al::LayoutInitInfo &initInfo) {
     
}

const sead::WFixedSafeString<0x200> *BingoConfigMenu::getStringData() {
    sead::SafeArray<sead::WFixedSafeString<0x200>, mItemCount>* gamemodeConfigOptions =
        new sead::SafeArray<sead::WFixedSafeString<0x200>, mItemCount>();

    gamemodeConfigOptions->mBuffer[0].copy(u"Among Us");
    gamemodeConfigOptions->mBuffer[1].copy(u"Among Us in the year 2012");

    return gamemodeConfigOptions->mBuffer;
}

bool BingoConfigMenu::updateMenu(int selectIndex) {

    BingoInfo* curMode = GameModeManager::instance()->getInfo<BingoInfo>();

    if (!curMode) {
        Logger::log("Unable to Load Mode info!\n");
        return true;   
    }
    
    switch (selectIndex) {
        case 0: {
            if (GameModeManager::instance()->isMode(GameMode::BINGO)) {
                // curMode->mIsUseGravity = true;
            }
            return true;
        }
        case 1: {
            if (GameModeManager::instance()->isMode(GameMode::BINGO)) {
                // curMode-> = false;
            }
            return true;
        }
        default:
            Logger::log("Failed to interpret Index!\n");
            return false;
    }
    
}