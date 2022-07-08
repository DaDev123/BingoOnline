#include "server/StageSceneStateDebugMenu.hpp"
#include <heap/seadHeapMgr.h>
#include <stream/seadBufferStream.h>
#include "AmongWriteStream.hpp"

const char16_t* StageSceneStateDebugMenu::mainMenuItems[] = {
    u"Reconnect logger",
    u"Log Heap Tree",
};

enum Option {
    RECONNECT_LOGGER,
    LOG_HEAPS,
};

char amonger[0xC000] = {};

StageSceneStateDebugMenu::StageSceneStateDebugMenu(const char* name, al::Scene* scene,
                                                   const al::LayoutInitInfo& initInfo,
                                                   FooterParts* footerParts,
                                                   GameDataHolder* dataHolder, bool)
    : al::HostStateBase<al::Scene>(name, scene) {
    constexpr const int mainMenuItemCount =
        sizeof(StageSceneStateDebugMenu::mainMenuItems) / sizeof(char16_t*);

    mFooterParts = footerParts;
    mGameDataHolder = dataHolder;

    mMsgSystem = initInfo.getMessageSystem();

    mInput = new InputSeparator(mHost, true);

    // page 0 menu
    mMainOptions = new SimpleLayoutMenu("DebugMenu", "OptionSelect", initInfo, 0, false);
    mMainOptionsList = new CommonVerticalList(mMainOptions, initInfo, true);

    al::setPaneString(mMainOptions, "TxtOption", u"Debug Menu", 0);

    mMainOptionsList->initDataNoResetSelected(mainMenuItemCount);

    sead::SafeArray<sead::WFixedSafeString<0x200>, mainMenuItemCount>* mainMenuOptions =
        new sead::SafeArray<sead::WFixedSafeString<0x200>, mainMenuItemCount>();

    int i = 0;
    for (const char16_t* item : mainMenuItems) {
        mainMenuOptions->mBuffer[i++].copy(item);
    }

    mMainOptionsList->addStringData(mainMenuOptions->mBuffer, "TxtContent");

    mCurrentList = mMainOptionsList;
    mCurrentMenu = mMainOptions;
}

void StageSceneStateDebugMenu::init() {
    initNerve(&nrvStageSceneStateDebugMenuMainMenu, 0);
}

void StageSceneStateDebugMenu::appear(void) {
    mCurrentMenu->startAppear("Appear");
    al::NerveStateBase::appear();
}

void StageSceneStateDebugMenu::kill(void) {
    mCurrentMenu->startEnd("End");
    al::NerveStateBase::kill();
}

al::MessageSystem* StageSceneStateDebugMenu::getMessageSystem(void) const {
    return mMsgSystem;
}

void StageSceneStateDebugMenu::exeMainMenu() {
    if (al::isFirstStep(this)) {
        mInput->reset();

        mCurrentList->activate();

        mCurrentList->appearCursor();

        decided = false;
    }

    mInput->update();

    mCurrentList->update();

    if (mInput->isTriggerUiUp()) {
        mCurrentList->up();
    }

    if (mInput->isTriggerUiDown()) {
        mCurrentList->down();
    }

    if (rs::isTriggerUiCancel(mHost)) {
        kill();
    }

    if (rs::isTriggerUiDecide(mHost)) {
        al::startHitReaction(mCurrentMenu, "決定", 0);
        mCurrentList->endCursor();
        mCurrentList->decide();
        decided = true;
    }

    if (decided && mCurrentList->isDecideEnd()) {
        switch (mCurrentList->mCurSelected) {
        case Option::RECONNECT_LOGGER:{
            Logger::reconnect();
            al::setNerve(this, &nrvStageSceneStateDebugMenuMainMenu);  // reset
            break;
        }
        case Option::LOG_HEAPS: {
            AmongWriteStream amongStream;
            sead::BufferWriteStream stream(&amongStream, amonger, sizeof(amonger));
            sead::HeapMgr::dumpTreeYAML(stream);
            stream.flush();
            al::setNerve(this, &nrvStageSceneStateDebugMenuMainMenu);  // reset
            break;
        }
        }
    }
}

namespace {
NERVE_IMPL(StageSceneStateDebugMenu, MainMenu)
}