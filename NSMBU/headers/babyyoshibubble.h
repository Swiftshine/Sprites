// some offsets are a little wrong, but 
// i've lost my Ghidra project, so i
// don't remember which


#include "game/actor/stage/stageactor.h"
#include "game/graphics/model/modelnw.h"

class BabyYoshiBubble : public StageActor {
public:
    ENUM_CLASS(BubbleReward,
        Coins = 0,
        Mushroom = 1,
        FireFlower = 2,
        IceFlower = 3,
        OneUp = 4,
        Acorn = 5
    );

public:
    u32 _27C;           // set by nybble 11; direction related
    ModelWrapper* model;
    u32 _2B0;
    u32 _2b4
    u8[24] padding1;
    BubbleReward::__type__ bubbleReward;
    u32 _2D4;
    u32 _2D8;
    u32 _2DC;
    u32 coinsPerBurst; // used for whenever the gamepad touches the bubble and does a coin burst
    u32 targetActorID;
    bool shouldSpawnReward;
    u8[3] padding2;
    u32 _2EC;
    u32 _2F0;
    u32 _2F4;
    Vec3f rewardPos;
    u32 _304;
    u32 _308;
    u32 _30C;
    Vec3f _310;
    u32 _31C;
    u32 _320;
    u32 _324;
    u32 rotationRelated;
    u32 _32C;
    u32 _330;
    Vec3u _334;
    Vec3u _340;
    u32 _34C;
    u32 _350;
    u32 _354;
    u32 _358;
    Vec2f bubbleSpeed;
    u32 _364
    u32 _368;
    void* callback; // ?
    Actor* targetActor;
    u32 _374;
    u32 _378;
    f32 _37C;
    u32 _380;
    u32 _384;
    BabyYoshiBubble* self;
    u32 _38C;
    u8[10] stateExecuter;
    u8[10] states;
    u32 _3B0;
};

static_assert(sizeof(BabyYoshiBubble) == 0x3B4, "BabyYoshiBubble is not the correct size.");

