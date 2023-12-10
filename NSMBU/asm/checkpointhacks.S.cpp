#include "game/actor/stage/stateactor.h"
#include "game/actor/stage/stageactor.h"
#include "game/effect/effect.h"
#include "game/collision/hitboxcollider.h"

extern void CheckpointFlagCollisionCallback(HitboxCollider*, HitboxCollider*);

extern "C" void HitboxCollider::init(StageActor*, const HitboxCollider::Info*, void**) {

}

extern "C" void SpawnStarEffectWithOffset(StateActor* self) {
    Vec3f effectPos = self->position;

    switch (self->movementID >> 0x4 & 0xF) { // nybble 21
        case 1: {
            effectPos.x += 32.0f;
        }
        case 2: {
            effectPos.x += 16.0f;
            effectPos.y -= 48.0f;
        }
        case 3: {
            effectPos.x -= 32.0f;
        }
        default: break;
    }


    Effect::spawn(0x67, &effectPos, nullptr, nullptr);
}


HitboxCollider::Info checkpointUpsideDownInfo = {
    Vec2f(0.0f, -24.0f),
    Vec2f(4.0f, 24.0f),
    HitboxCollider::Shape::Rectangle,
    3,
    0,
    5,
    0,
    0,
    &CheckpointFlagCollisionCallback
};

HitboxCollider::Info checkpointLeftInfo = {
    Vec2f(-24.0f, 0.0f),
    Vec2f(24.0f, -4.0f),
    HitboxCollider::Shape::Rectangle,
    3,
    0,
    5,
    0,
    0,
    &CheckpointFlagCollisionCallback
};

HitboxCollider::Info checkpointRightInfo = {
    Vec2f(24.0f, 0.0f),
    Vec2f(24.0f, -4.0f),
    HitboxCollider::Shape::Rectangle,
    3,
    0,
    5,
    0,
    0,
    &CheckpointFlagCollisionCallback
};

ASM_BEGIN
.data
// stored powerup
storedPowerup: .byte 0

// star effect offsets
thirtyTwo: .float 32.0


.text

.global RotateCheckpointModel
RotateCheckpointModel:
    // r3 = this->movementID
    lbz     r3, 0x18(r29)

    // nybble 21:
    // r3 = (r3 >> 0x4 & 0xF)
    srawi   r3, r3, 0x4
    andi.   r3, r3, 0xF

    // check for direction; if 0 or not 1-3, face upright
    cmpwi   r3, 0x1
    beq     modelRight
    cmpwi   r3, 0x2
    beq     modelUpsideDown
    cmpwi   r3, 0x3
    beq     modelLeft
    b       modelDefault

modelDefault:
    li r4, 0x0
    // this->rotation.z = r4
    stw r4, 0xa4(r29)
    // this->rotation.y = r4
    lis r4, 0xC000
    stw r4, 0xa0(r29)
    mr r3, r29
    blr
modelRight:
    lis r4, 0xC000
    // this->rotation.z = r4
    stw r4, 0xa4(r29)
    // this->rotation.y = r4
    stw r4, 0xa0(r29)
    mr r3, r29
    blr
modelUpsideDown:
    lis r4, 0x8000
    // this->rotation.z = r4
    stw r4, 0xa4(r29)
    // this->rotation.y = r4
    lis r4, 0xC000
    stw r4, 0xa0(r29)
    mr r3, r29
    blr
modelLeft:
    lis r4, 0x4000
    // this->rotation.z = r4
    stw r4, 0xa4(r29)
    // this->rotation.y = r4
    lis r4, 0xC000
    stw r4, 0xa0(r29)
    mr r3, r29
    blr


.global RotateCheckpointHitboxAndStoreNybble22
RotateCheckpointHitboxAndStoreNybble22:
    li r6, 0x0

    // r5 is currently the collisionInfo

    // r14 = nybble 22
    lbz r14, 0x18(r29)
    andi. r14, r14, 0xF
    // saving nybble 22 for later
    li r17, 0x0
    lis r17, storedPowerup@ha
    addi r17, r17, storedPowerup@l
    stb r14, 0x0(r17)


    // r14 = nybble 21
    lbz r14, 0x18(r29)
    srawi r14, r14, 0x4
    andi. r14, r14, 0xF

    cmpwi r14, 0x1
    beq hitboxRight
    cmpwi r14, 0x2
    beq hitboxUpsideDown
    cmpwi r14, 0x3
    beq hitboxLeft
    blr

hitboxUpsideDown:
    lis r14, checkpointUpsideDownInfo@ha
    addi r5, r14, checkpointUpsideDownInfo@l
    // mr r5, r14
    blr

hitboxLeft:
    lis r14, checkpointLeftInfo@ha
    addi r5, r14, checkpointLeftInfo@l
    // mr r5, r14
    blr

hitboxRight:
    lis r14, checkpointRightInfo@ha
    addi r5, r14, checkpointRightInfo@l
    // mr r5, r14
    blr

.global ChangeGivenPowerup
ChangeGivenPowerup:
    // r14 = nybble 22
    li r5, 0
    lis r15, storedPowerup@ha
    addi r15, r15, storedPowerup@l
    lbz r14, 0x0(r15)
    andi. r14, r14, 0xF

    cmpwi r14, 0x8
    bge noPowerup
    addi r4, r14, 0x1
    blr
noPowerup:
    li r4, 0x0
    blr

ASM_END
