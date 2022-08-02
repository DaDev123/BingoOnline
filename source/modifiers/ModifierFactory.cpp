#include "modifiers/ModifierFactory.hpp"
#include "modifiers/ModifierBase.hpp"
#include "modifiers/gravity/GravityModifier.hpp"

typedef ModifierBase* (*createMode)(const char* name);

template <class T>
ModifierBase* createModifier(const char* name)
{
    return new T(name);
};

__attribute((used)) const al::NameToCreator<createMode> modeTable[] = {
    {"Gravity", &createModifier<GravityModifier>},
};

constexpr const char* modeNames[] = {
    "Galaxy Gravity"
};

ModifierFactory::ModifierFactory(const char *fName){
            this->factoryName = fName;
            this->actorTable = modeTable;
            this->factoryCount = sizeof(modeTable)/sizeof(modeTable[0]);
        };

char* ModifierFactory::getModeName(int idx) const {
    if(idx >= 0 && (size_t)idx < sizeof(modifierTable)/sizeof(modifierTable[0]))
        return modifierTable[idx];
    return nullptr;
}

int ModifierFactory::getModeCount() {
    return sizeof(modeTable)/sizeof(modeTable[0]);
}