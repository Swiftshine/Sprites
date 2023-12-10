#pragma once

#include "game/actor/stage/stateactor.h"
#include "game/graphics/model/modelnw.h"
#include "game/collision/hitboxcollider.h"
#include "game/effect/effect.h"


class CheckpointFlag : public StateActor {
    SEAD_RTTI_OVERRIDE(CheckpointFlag, StateActor);
public:
    StateActor stateActor;          // 0x0
    ModelWrapper* modelWrapper;      // 0x17C8
    HitboxCollider hitboxCollider;  // 0x17CC
    bool isUnderwater;              // 0x18F4
    EffectWrapper EffectWrapper;    // 0x18F8
    u8 _1960;                       // 0x1960
    u8 _1961;                       // 0x1961
    bool canBounceOnBouncyClouds;   // 0x1962
    u8 _1963;                       // 0x1963
    u8 _1964[192];                  // 0x1964 // class?
    u8 checkpointNumber;            // 0x1A24
};
