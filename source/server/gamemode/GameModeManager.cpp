#include "server/gamemode/GameModeManager.hpp"
#include <cstring>
#include <heap/seadFrameHeap.h>
#include "al/util.hpp"
#include "basis/seadNew.h"
#include "heap/seadHeapMgr.h"
#include "server/gamemode/GameModeBase.hpp"
#include "server/gamemode/GameModeFactory.hpp"

SEAD_SINGLETON_DISPOSER_IMPL(GameModeManager)

GameModeManager::GameModeManager() {
    mHeap = sead::FrameHeap::create(0x100000, "GameModeHeap", al::getSequenceHeap(), 8,
                                    sead::Heap::HeapDirection::cHeapDirection_Reverse, false);
    setMode(GameMode::HIDEANDSEEK);
}

void GameModeManager::begin() {
    if (mCurModeBase) {
    sead::ScopedCurrentHeapSetter heapSetter(mHeap);
        mCurModeBase->begin();
    }
}

void GameModeManager::end() {
    if (mCurModeBase) {
        sead::ScopedCurrentHeapSetter heapSetter(mHeap);
        mCurModeBase->end();
    }
}

void GameModeManager::toggleActive() {
    mActive = !mActive;
    if (mActive) begin();
    else end();
}

void GameModeManager::setPaused(bool paused) {
    mPaused = paused;
    if (!mActive) return;
    if (mPaused) end();
    else begin();
}

void GameModeManager::setMode(GameMode mode) {
    if (mCurMode == mode)
        return;

    sead::ScopedCurrentHeapSetter heapSetter(mHeap);

    if (mCurModeBase) {
        if (mLastInitInfo)
            mCurModeBase->end();
        delete mCurModeBase;
    }

    mCurMode = mode;

    if (mode == GameMode::NONE) {
        mCurModeBase = nullptr;
        mModeInfo = nullptr;
        return;
    }

    Logger::log("Setting mode to %d\n", mode);
    GameModeFactory factory("GameModeFactory");
    const char* name = factory.getModeString(mCurMode);
    mCurModeBase = factory.getCreator(name)(name);
    if (mLastInitInfo &&
        al::getSceneHeap() != nullptr) {  // check if there's a previously used init info AND there
                                          // is an active scene (scene heap will be null otherwise)
        mCurModeBase->init(*mLastInitInfo);
        // mCurModeBase->begin();
    }
}

void GameModeManager::update() {
    if (!mCurModeBase) return;
    bool inScene = al::getSceneHeap() != nullptr;
    if (mActive && inScene && !mPaused && !mCurModeBase->isModeActive()) begin();
    if ((!mActive || mPaused || !inScene) && mCurModeBase->isModeActive()) end();
    if (mCurModeBase && mCurModeBase->isModeActive()) {
        sead::ScopedCurrentHeapSetter heapSetter(mHeap);
        mCurModeBase->update();
    }
}

void GameModeManager::initScene(const GameModeInitInfo& info) {
    sead::ScopedCurrentHeapSetter heapSetter(mHeap);

    if (mLastInitInfo != nullptr) {
        delete mLastInitInfo;
    }

    mLastInitInfo = new GameModeInitInfo(info);

    if (mCurModeBase) {
        mCurModeBase->init(*mLastInitInfo);
        mCurModeBase->begin();
    }
}