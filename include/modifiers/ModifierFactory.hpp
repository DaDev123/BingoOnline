#pragma once

#include "al/factory/Factory.h"

class ModifierBase;

typedef ModifierBase* (*createModifier)(const char* name);
__attribute((used)) extern const al::NameToCreator<createModifier> modifierTable[];

class ModifierFactory : public al::Factory<createModifier> {
    public:
        ModifierFactory(const char *fName);

        static const char* getModeName(int idx);
        static int getModeCount();
};