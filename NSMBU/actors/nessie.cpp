/* massively unfinished. model loading works though */

#include "game/actor/stage/multistateactor.h"
#include "game/movementhandler.h"
#include "game/graphics/model/modelnw.h"
#include "game/collision/collidermgr.h"
#include "game/collision/solid/rectcollider.h"
#include "game/audio/gameaudio.h"
// #include "game/playerlooktarget.h"
#include "log.h"

class Nessie : public MultiStateActor {
    SEAD_RTTI_OVERRIDE_IMPL(Nessie, MultiStateActor);

public:
    Nessie(const ActorBuildInfo* buildInfo);
    virtual ~Nessie() { }

    u32 onCreate() override;
    u32 onExecute() override;
    u32 onDraw() override;

    static void collisionCallback(HitboxCollider* hcSelf, HitboxCollider* hcOther);

    ModelWrapper* model;
    RectCollider rectCollider;
    static const HitboxCollider::Info collisionInfo;
    MovementHandler movementHandler;
    // PlayerLookTarget lookTarget;
    int counter;
    int behaviour;
    // TODO: add colour variants

    ENUM_CLASS(Behaviour,
        Swim,
        SwimPath,
        Wave
    );

    DECLARE_STATE(Nessie, Idle);
    DECLARE_STATE(Nessie, Bounce);
};

CREATE_STATE(Nessie, Idle);
CREATE_STATE(Nessie, Bounce);

REGISTER_PROFILE(Nessie, ProfileID::Nessie);

PROFILE_RESOURCES(ProfileID::Nessie, Profile::LoadResourcesAt::Course, "raidonno");

Nessie::Nessie(const ActorBuildInfo* buildInfo)
    : MultiStateActor(buildInfo)
    , model(nullptr)
    , movementHandler()
    // , lookTarget(&this->id)
    , counter(0)
    , behaviour(this->eventID1 >> 0x4 & 0xF) // nybble 1
{ }

const HitboxCollider::Info Nessie::collisionInfo = {
    Vec2f(0.0f, 14.0f), Vec2f(11.0f, 14.0f), HitboxCollider::Shape::Rectangle, 3, 0, 0xFFFFFFFF, 0xFFFBFFFF, 0, &Nessie::collisionCallback
};

u32 Nessie::onCreate() {
    // this->lookTarget.ownerID = this->id;
    this->model = ModelWrapper::create("raidonno", "raidonno", 14);
    if (this->behaviour == Behaviour::Wave) {
        this->model->playSklAnim("GoodBye");
    } else {
        this->model->playSklAnim("Swim");
    }
    this->model->loopSklAnims(true);


    // scale (3dw models are larger than nsmbu models)
    this->scale = .25f;
    this->position += Vec3f(8.0f, -8.0f, 0.0f);
    this->rotation.x = fixDeg(20.0f);

    u32 movementMask = this->movementHandler.getMaskForMovementType(this->settings2 & 0xFF); // nybble 20
    this->movementHandler.link(this->position, movementMask, this->movementID /* <-- nybble 21-22*/ );

    this->hitboxCollider.init(this, &Nessie::collisionInfo, nullptr);
    this->addHitboxColliders();

    this->doStateChange(&Nessie::StateID_Idle);
    return this->onExecute();
}

u32 Nessie::onExecute() {
    this->states.execute();

    Mtx34 mtx;
    mtx.makeRTIdx(this->rotation, this->position);
    this->model->setMtx(mtx);
    this->model->setScale(this->scale);
    this->model->updateAnimations();
    this->model->updateModel();

    if (this->behaviour != Behaviour::SwimPath) {
        this->offscreenDelete(1);
    } else {
        this->movementHandler.execute();
        this->position = this->movementHandler.position;
    }

    return 1;
}

u32 Nessie::onDraw() {
    this->model->draw();
    return 1;
}


/** STATE: Idle */

void Nessie::beginState_Idle() {
    this->model->loopSklAnims(true);
    
    if (this->behaviour == Behaviour::Wave) {
        this->model->playSklAnim("GoodBye");
    } else {
        this->model->playSklAnim("Swim");
    }
}

void Nessie::executeState_Idle() {
    if (this->behaviour == Behaviour::Wave) {

    }
}
void Nessie::endState_Idle() { }

/** STATE:  */

void Nessie::beginState_Bounce() {
    this->model->loopSklAnims(false);
    this->model->playSklAnim("HitWater");
    this->counter = 0;
}

void Nessie::executeState_Bounce() {
    if (this->counter == 30) {
        this->doStateChange(&Nessie::StateID_Idle);
    } else {
        this->counter++;
    }
}
void Nessie::endState_Bounce() { }

void Nessie::collisionCallback(HitboxCollider* hcSelf, HitboxCollider* hcOther) {
    if (hcOther->owner->type != StageActor::Type::Player && hcOther->owner->type != StageActor::Type::Yoshi && hcOther->owner->type != StageActor::Type::BabyYoshi) {
        return;
    }

    Nessie* self = (Nessie*)hcSelf->owner;
    if (self->behaviour == Behaviour::Wave) { // collider does nothing when waving
        return;
    }

    self->doStateChange(&Nessie::StateID_Bounce);

    hcOther->owner->speed.y = 4.0f;
    hcOther->owner->isDeleted = true;


    
}

/*
TODO: add movement option
* path controlled upon being hit by player for first time

*/

