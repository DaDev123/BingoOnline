#pragma once

#include "al/scene/SceneObjHolder.h"
#include "logger.hpp"
#include "prim/seadNamable.h"
#include "prim/seadSafeString.h"

class ModifierBase : public sead::INamable, public al::IUseSceneObjHolder {
public:
    ModifierBase(const char* name) { mName = name; }
    virtual ~ModifierBase() = default;
    al::SceneObjHolder* getSceneObjHolder() const override { return mSceneObjHolder; }
    virtual void begin() {
        mIsActive = true;
        Logger::log("Begin\n");
    }
    virtual void update() = 0;
    virtual void end() {
        mIsActive = false;
        Logger::log("End\n");
    }

private:
    sead::FixedSafeString<0x80> mName;
    al::SceneObjHolder* mSceneObjHolder;
    bool mIsActive;
};
