#include <heap/seadDisposer.h>


class ModifierManager {
    SEAD_SINGLETON_DISPOSER(ModifierManager)
    ModifierManager();
    ~ModifierManager();
public:
    void setModifier();
};