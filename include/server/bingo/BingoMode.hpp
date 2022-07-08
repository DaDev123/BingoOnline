#pragma once

#include <math.h>
#include <heap/seadFrameHeap.h>
#include "al/camera/CameraTicket.h"
#include "heap/seadExpHeap.h"
#include "heap/seadFrameHeap.h"
#include "server/gamemode/GameModeBase.hpp"
#include "server/gamemode/GameModeInfoBase.hpp"
#include "server/gamemode/GameModeConfigMenu.hpp"
#include "server/gamemode/GameModeTimer.hpp"

enum BingoObjective {
    CaptainToad,
    BuyHats,
    BuyFullCostumes,
    BuyMoons,
    JaxiCalls,
    Checkpoints,
    SubAreaMoons,
    KeyMoons,
    NutMoons,
    SeedMoons,
    PlantSeeds,
    SlotsMoons,
    HintArts,
    HintArtMoons,
    LakituMoons,
    WarpPaintings,
    Stickers,
    Souvenirs,
    SphynxQuestions,
    Bit8Moons,
    RockMoons,
    MoonShardMoons,
    StoryMoons,
    MusicMoons,
    LifeUpHearts,
    CrateMoons,
    OutfitMoons,
    TimerMoons,
    UniqueCaptures,
    WorldPeace,
    GoombaMoons,
    GroundPoundMoons,
    SpecificMoons, // specific objective metadata required
    RegionalCoins,
    FishingMoons,
    Nuts,
    SpecificCaptures
};

struct BingoInfo : GameModeInfoBase {
    BingoInfo() { mMode = GameMode::BINGO; }
};

class BingoMode : public GameModeBase {
    public:
        BingoMode(const char* name);

        void init(GameModeInitInfo const& info) override;

        void begin() override;
        void update() override;
        void end() override;
};