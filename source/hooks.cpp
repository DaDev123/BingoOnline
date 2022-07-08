#include <sys/types.h>
#include "server/Client.hpp"
#include "al/LiveActor/LiveActor.h"
#include "al/actor/ActorInitInfo.h"
#include "al/actor/Placement.h"
#include "al/byaml/ByamlIter.h"
#include "al/nerve/Nerve.h"
#include "al/nerve/NerveExecutor.h"
#include "al/nerve/NerveKeeper.h"
#include "al/util.hpp"
#include "al/util/ControllerUtil.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/NerveUtil.h"
#include "game/Actors/WorldEndBorderKeeper.h"
#include "game/Layouts/CoinCounter.h"
#include "game/Player/Actions/PlayerActionGroundMoveControl.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerConst.h"
#include "game/Player/States/PlayerStateRunHakoniwa.h"
#include "game/StageScene/StageSceneStateOption.h"
#include "game/StageScene/StageSceneStatePauseMenu.h"
#include "server/StageSceneStateServerConfig.hpp"
#include "server/StageSceneStateDebugMenu.hpp"
#include "logger.hpp"
#include "main.hpp"
#include "al/byaml/writer/ByamlWriter.h"
#include "math/seadVector.h"
#include "rs/util/InputUtil.h"
#include "sead/prim/seadSafeString.h"
#include "server/HideAndSeekMode.hpp"
#include "server/gamemode/GameModeManager.hpp"

bool comboBtnHook(int port) {
    if (GameModeManager::instance()->isActive()) { // only switch to combo if any gamemode is active
        return !al::isPadHoldL(port) && al::isPadTriggerDown(port);
    } else {
        return al::isPadTriggerDown(port);
    }
}

void saveWriteHook(al::ByamlWriter* saveByml) {

    const char *serverIP = Client::getCurrentIP();
    const int serverPort = Client::getCurrentPort();

    if (serverIP) {
        saveByml->addString("ServerIP", serverIP);
    } else {
        saveByml->addString("ServerIP", "0.0.0.0");
    }

    if (serverPort) {
        saveByml->addInt("ServerPort", serverPort);
    } else {
        saveByml->addInt("ServerPort", 0);
    }

    saveByml->pop();
}

bool saveReadHook(int* padRumbleInt, al::ByamlIter const& saveByml, char const* padRumbleKey) {

    const char *serverIP = "";
    int serverPort = 0;

    if (al::tryGetByamlString(&serverIP, saveByml, "ServerIP")) {
        Client::setLastUsedIP(serverIP);
    }

    if (al::tryGetByamlS32(&serverPort, saveByml, "ServerPort")) {
        Client::setLastUsedPort(serverPort);
    }
    
    return al::tryGetByamlS32(padRumbleInt, saveByml, padRumbleKey);
}

bool registerShineToList(Shine* shineActor) {

    if (shineActor->shineId >= 0) {
        Client::tryRegisterShine(shineActor);
    } 

    return al::isAlive(shineActor);
}

void overrideNerveHook(StageSceneStatePauseMenu* thisPtr, al::Nerve* nrvSet) {
    if (al::isPadHoldL(-1) && !al::isPadHoldZL(-1) && !al::isPadHoldR(-1) && al::isPadHoldZR(-1)) { // L + R + ZR (- ZL to avoid discovery)
        al::setNerve(thisPtr, &nrvStageSceneStatePauseMenuDebugMenu);
    } else if (al::isPadHoldZL(-1)) {
        al::setNerve(thisPtr, &nrvStageSceneStatePauseMenuServerConfig);
    } else {
        al::setNerve(thisPtr, nrvSet);
    }
}

StageSceneStateServerConfig *sceneStateServerConfig = nullptr;
StageSceneStateDebugMenu *sceneStateDebugMenu = nullptr;

void initStateHook(StageSceneStatePauseMenu *thisPtr, char const *stateName, al::Scene *host, al::LayoutInitInfo const &initInfo, FooterParts *footer,
                   GameDataHolder *data, bool unkBool) {
    thisPtr->mStateOption =
        new StageSceneStateOption(stateName, host, initInfo, footer, data, unkBool);

    sceneStateServerConfig = new StageSceneStateServerConfig("ServerConfig", host, initInfo, footer, data, unkBool);
    sceneStateDebugMenu = new StageSceneStateDebugMenu("DebugMenu", host, initInfo, footer, data, unkBool);
}

void initNerveStateHook(StageSceneStatePauseMenu* stateParent, StageSceneStateOption* stateOption,
                        al::Nerve const* executingNerve, char const* stateName) {

    al::initNerveState(stateParent, stateOption, executingNerve, stateName);

    al::initNerveState(stateParent, sceneStateServerConfig, &nrvStageSceneStatePauseMenuServerConfig, "CustomNerveOverride");
    al::initNerveState(stateParent, sceneStateDebugMenu, &nrvStageSceneStatePauseMenuDebugMenu, "DebugMenuNerve");
}

// skips starting both coin counters
void startCounterHook(CoinCounter* thisPtr) {
    if (!GameModeManager::instance()->isActive()) {
        thisPtr->tryStart();
    }
}

// Simple hook that can be used to override isModeE3 checks to enable/disable certain behaviors
bool modeE3Hook() {
    return GameModeManager::instance()->isActive();
}

// Skips ending the play guide layout if a mode is active, since the mode would have already ended it
void playGuideEndHook(al::SimpleLayoutAppearWaitEnd* thisPtr) {
    if (!GameModeManager::instance()->isActive()) {
        thisPtr->end();
    }
}

// Gravity Hooks

void initHackCapHook(al::LiveActor *cappy) {
    al::initActorPoseTQGSV(cappy);
}

al::PlayerHolder* createTicketHook(StageScene* curScene) {
    // only creates custom gravity camera ticket if hide and seek mode is active
    if (GameModeManager::instance()->isMode(GameMode::HIDEANDSEEK)) {
        al::CameraDirector* director = curScene->getCameraDirector();
        if (director) {
            if (director->mFactory) {
                al::CameraTicket* gravityCamera = director->createCameraFromFactory(
                    "CameraPoserCustom", nullptr, 0, 5, sead::Matrix34f::ident);

                HideAndSeekMode* mode = GameModeManager::instance()->getMode<HideAndSeekMode>();

                mode->setCameraTicket(gravityCamera);
            }
        }
    }

    return al::getScenePlayerHolder(curScene);
}

bool borderPullBackHook(WorldEndBorderKeeper* thisPtr) {

    bool isFirstStep = al::isFirstStep(thisPtr);

    if (isFirstStep) {
        if (GameModeManager::instance()->isModeAndActive(GameMode::HIDEANDSEEK)) {

            HideAndSeekMode* mode = GameModeManager::instance()->getMode<HideAndSeekMode>();

            if (mode->isUseGravity()) {
                killMainPlayer(thisPtr->mActor);
            }
        }
    }
    
    return isFirstStep;
}