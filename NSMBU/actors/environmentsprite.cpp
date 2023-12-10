#include "game/actor/actormgr.h"
#include "sead/array.h"
#include "game/direction.h"
#include "math/functions.h"
#include "game/graphics/model/modelnw.h"
#include "game/effect/effect.h"
#include "game/effect/effectid.h"
#include "game/actor/stage/multistateactor.h"
#include "game/playermgr.h"
#include "sead/random.h"
#include "math/bezier.h"
#include "math/easing.h"
#include "game/collision/collidermgr.h"
#include "log.h"

class EnvironmentSprite : public MultiStateActor {
    SEAD_RTTI_OVERRIDE_IMPL(EnvironmentSprite, MultiStateActor);

public:
    EnvironmentSprite(const ActorBuildInfo* buildInfo);
    virtual ~EnvironmentSprite() { }

    u32 onCreate() override;
    u32 onExecute() override;
    u32 onDraw() override;
    
    ModelWrapper* model;
    bool isCrab;
    u32 visualType;
    u32 wait;
    Easing easer;
    f32 startingX;
    f32 velocity;
    f32 targetX;
    f32 baseline;
    f32 t;
    bool hop;
    bool hopState;


    ENUM_CLASS(VisualType,
        WhiteBird,
        Parrot,
        BlueJay,
        Rosefinch,
        HouseSparrow,
        CrestedWHAT,
        Lorikeet,
        Crab
        /*
        TODO: additional sprite types
        * flamingo
        * duck
        * fireflies
        * bird that will kill you by exploding xd
        * butterflies
        */
    );

    DECLARE_STATE(EnvironmentSprite, Idle);
    DECLARE_STATE(EnvironmentSprite, Flee);
    DECLARE_STATE(EnvironmentSprite, Action);
};

CREATE_STATE(EnvironmentSprite, Idle);
CREATE_STATE(EnvironmentSprite, Flee);
CREATE_STATE(EnvironmentSprite, Action);

REGISTER_PROFILE(EnvironmentSprite, ProfileID::EnvironmentSprite);
PROFILE_RESOURCES(ProfileID::EnvironmentSprite, Profile::LoadResourcesAt::Course,
    "bird", 
    "parrot", 
    "blue_jay",
    "rosefinch",
    "house_sparrow",
    "crestedWHAT", // if you're reading this code after it's made public, i will NOT elaborate
    "lorikeet",
    "crab"
); // remember to add the others later

EnvironmentSprite::EnvironmentSprite(const ActorBuildInfo* buildInfo)
    : MultiStateActor(buildInfo)
    , model(nullptr)
    , velocity(1.5f)
    , wait(0)
    , isCrab(false)
    , visualType(this->eventID1 >> 0x4 & 0xF) // nybble 1
    , startingX(this->position.x)
{ }



u32 EnvironmentSprite::onCreate() {
    static const sead::SafeArray<const char*, 7> models = {
        "bird",
        "parrot",
        "blue_jay",
        "rosefinch",
        "house_sparrow",
        "crestedWHAT", // remember that there is a second texture for "individuality"
        "lorikeet"
    };

    if (this->visualType == 7) {
        this->isCrab = true;
        this->model = ModelWrapper::create("crab", "crab", 3);
        this->model->playSklAnim("Run");
        this->scale = Vec3f(0.17f, 0.17f, 0.17f);
    } else {
        this->model = ModelWrapper::create(models[this->visualType], models[this->visualType], 4, 1);
        this->model->playSklAnim("GroundWaitA");
        this->model->loopSklAnims(true);

        this->scale = (this->visualType == VisualType::WhiteBird) ? Vec3f(0.17f, 0.17f, 0.17f) : Vec3f(0.2f, 0.2f, 0.2f);
        
        if (this->visualType == VisualType::CrestedWHAT) {
            this->model->playTexPatternAnim("tit", 0); // this is the second texture for "individuality"
            this->model->texPatternAnims[0]->frameCtrl.startFrame = sead::randBool();
            this->model->texPatternAnims[0]->frameCtrl.speed = 0.0;
        }

        if (this->visualType == VisualType::Rosefinch) {
            this->model->playTexPatternAnim("unnamedbird", 0);
            this->model->texPatternAnims[0]->frameCtrl.startFrame = (this->eventID2 >> 0x4 & 0xF);
            this->model->texPatternAnims[0]->frameCtrl.speed = 0.0;
        }
        
        this->direction = sead::randBool();
        this->rotation.y = this->direction ? fixDeg(90.0f) : fixDeg(270.0f);
    }

    this->position += Vec3f(8.0f, -8.0f, 0.0f);

    // one tile is 16x16 pixels. the sensors should be placed accordingly.

    PhysicsMgr::Sensor belowSensor = { -6,  8,  0 };
    PhysicsMgr::Sensor aboveSensor = { -8,  8, 16 };
    PhysicsMgr::Sensor sidesSensor = {  8, 8, 8 };
    this->physicsMgr.init(this, &belowSensor, &aboveSensor, &sidesSensor);




    this->doStateChange(&EnvironmentSprite::StateID_Idle);
    
    return this->onExecute();
}

u32 EnvironmentSprite::onExecute() {
    this->states.execute();

    Mtx34 mtx;
    mtx.makeRTIdx(this->rotation, this->position);
    this->model->setMtx(mtx);
    this->model->setScale(this->scale);
    this->model->updateAnimations();
    this->model->updateModel();

    if (!(this->states.currentState()->isEqual(StateID_Flee))) {
        this->physicsMgr.processCollisions();
        Vec2f distToPlayer;
        // flee if the player is close
        if (this->distanceToPlayer(distToPlayer) > -1 && sead::Mathf::abs(distToPlayer.x) < 4.0f * 16.0f && sead::Mathf::abs(distToPlayer.y) < 6.0f * 16.0f) {
            this->doStateChange(&StateID_Flee);
        }
    }

    this->offscreenDelete(0);

    return 1;
}

u32 EnvironmentSprite::onDraw() {
    this->model->draw();
    
    return 1;
}

/** STATE: Idle */

void EnvironmentSprite::beginState_Idle() {
    this->baseline = this->position.x;
    if (this->isCrab) {
        this->model->playSklAnim("Run");
    }
}

void EnvironmentSprite::executeState_Idle() {
    if (this->isCrab) {
        if (this->direction == Direction::Right) {
            this->position.x += 1.0f;
        } else {
            this->position.x -= 1.0f;
        }

        if (sead::Mathf::abs(this->position.x - this->baseline) >= 16.0f * ((/*nybble 2*/ this->eventID1 & 0xF)+1)) {
            this->direction ^= 1;
            this->doStateChange(&StateID_Action);
        }
    } else {
        // change direction randomly
        if (sead::randU32(300) == 1) {
            this->direction = sead::randBool();
            this->rotation.y = (this->direction) ? fixDeg(90.0f) : fixDeg(270.0f);
        }

        if (sead::randU32(120) == 1) {
            this->doStateChange(&StateID_Action);
        }
    }
}
void EnvironmentSprite::endState_Idle() { }

/** STATE: Flee */
void EnvironmentSprite::beginState_Flee() {
    this->wait = 0;
    if (this->visualType == VisualType::WhiteBird) {
        this->model->playSklAnim("FlyWait");
        this->position.y -= 5;
    } else if (this->visualType != VisualType::Crab) {
        this->model->playSklAnim("Fly");
        this->position.y -= 4;
    } else {
        this->model->playSklAnim("Disappear");
        this->position = Vec3f(this->position.x, this->position.y - 8.0f, -3300.0f);
        this->model->loopSklAnims(false);
    }
}

void EnvironmentSprite::executeState_Flee() {
    // bird: fly away
    if (this->isCrab) {
        if (this->model->sklAnims[0]->frameCtrl.currentFrame == 11) {
            Vec3f effectScale = 1.15f;
            Effect::spawn(0x085, &this->position, nullptr, &effectScale);
        } else if (this->model->sklAnims[0]->frameCtrl.isDone()) {
            this->isDeleted = true;
        }
    } else {
        if (this->wait > 8) {
            this->position.x += (this->direction) ? this->velocity : -this->velocity;
            this->position.y += this->velocity;
            this->velocity += 0.05f;
        }

    }

    this->wait++;
}

void EnvironmentSprite::endState_Flee() { }

/** STATE: Action */

void EnvironmentSprite::beginState_Action() {
    if (this->isCrab) {
        this->model->playSklAnim("Wait");
        this->model->loopSklAnims(false);
    } else {
        this->baseline = this->position.y;
        this->targetX = this->position.x + (this->direction ? 1 : -1) * 16.0f;
        this->hop = sead::randBool();
        this->hopState = 0;
        this->t = 0.0f;
        
        if (this->hop) {
            this->easer.set(Easing::sineOut, 0.0f, 2.0f, 0.1f);
        }
    }
}

void EnvironmentSprite::executeState_Action() {
    if (this->isCrab) {
        if (this->model->sklAnims[0]->frameCtrl.isDone()) {
            this->doStateChange(&StateID_Idle);
        }
    } else {
        bool allDone = sead::Mathf::chase(&this->position.x, this->targetX, 0.75f);

        if (this->hop) {
            if (this->hopState == 0) {
                allDone = false;
                
                this->easer.ease(this->t);
                this->position = this->position.lerp(Vec3f(this->position.x, this->baseline + 4.0f, this->position.z), this->t);

                if (this->t >= 1.0f) {
                    this->easer.set(Easing::sineIn, 0.0f, 1.0f, 0.1f);
                    this->hopState = 1;
                }
            } else if (this->hopState == 1) {
                bool done = this->easer.ease(this->t);
                this->position = this->position.lerp(Vec3f(this->position.x, this->baseline, this->position.z), this->t);

                allDone &= done;
            }
        }

        if (allDone) {
            // this->direction ^= 1;
            // the sprite should only move ~three blocks; check if the sprite has moved more than one block and then turn back
            this->direction ^= 1;
            this->rotation.y = this->direction ? fixDeg(90.0f) : fixDeg(270.0f);

            this->doStateChange(&StateID_Idle);
        }
    }
}

void EnvironmentSprite::endState_Action() { }

