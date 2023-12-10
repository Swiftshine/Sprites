/* massively unfinished. state of this actor is unknown */

#include "game/actor/stage/multistateactor.h"
#include "game/graphics/model/modelnw.h"
#include "game/graphics/model/platformmodel.h"
#include "game/collision/solid/polylinecollider.h"
#include "game/collision/hitboxcollider.h"
#include "game/collision/collidermgr.h"
#include "game/sound/effects.h"
#include "log.h"

class UpDownMushroom : public MultiStateActor {
    SEAD_RTTI_OVERRIDE_IMPL(UpDownMushroom, MultiStateActor);
public:
    UpDownMushroom(const ActorBuildInfo* buildInfo);
    virtual ~UpDownMushroom() { }

    u32 onCreate() override;
    u32 onExecute() override;
    u32 onDraw() override;

    bool isUp;
    bool eventActivated;
    float stretchSpeed;
    float stretchDistance;
    float topLength;
    bool currentlyStretched;
    float destinationY;
    u32 touchTimer;


    ModelWrapper* stemModel;
    PlatformModel topModel;
    PolylineCollider polylineCollider;
    static HitboxCollider::Info collisionInfo;
    static void collisionCallback(HitboxCollider* hcSelf, HitboxCollider* hcOther);

    DECLARE_STATE(UpDownMushroom, Stationary);
    DECLARE_STATE(UpDownMushroom, Move);
    // DECLARE_STATE(UpDownMushroom, Stay);
};

CREATE_STATE(UpDownMushroom, Stationary);
CREATE_STATE(UpDownMushroom, Move);
// CREATE_STATE(UpDownMushroom, Stay);

REGISTER_PROFILE(UpDownMushroom, ProfileID::UpDownMushroom);
PROFILE_RESOURCES(ProfileID::UpDownMushroom, Profile::LoadResourcesAt::Course, "star_coin", "lift_kinoko_yoko", "lift_kinoko_shiso"); // placeholder

HitboxCollider::Info UpDownMushroom::collisionInfo = {
    Vec2f(0.0f, 16.0f), Vec2f(16.0f, 0.0f), HitboxCollider::Shape::Rectangle, 5, 0, 0x824F, 0xFFFBFFFF, 0, &UpDownMushroom::collisionCallback
};

void UpDownMushroom::collisionCallback(HitboxCollider* hcSelf, HitboxCollider* hcOther) {
    // UpDownMushroom* self = static_cast<UpDownMushroom*>(hcSelf->owner);

    // if (self->states.currentState == &UpDownMushroom::StateID_Stationary) {
    //     self->doStateChange(&UpDownMushroom::StateID_Move);
    // } else if (self->states.currentState = &UpDownMushroom::StateID_Move) {
    //     if (self->currentlyStretched && self->position.y == self->destinationY) {
    //         //...huh
    //     }
    // }
}

UpDownMushroom::UpDownMushroom(const ActorBuildInfo* buildInfo)
    : MultiStateActor(buildInfo)
    , eventActivated(this->settings1 >> 0x1C & 0xF) // nybble 5
    , isUp(this->settings1 >> 0x18 & 0xF) // nybble 6
    , stretchSpeed(static_cast<float>(this->settings1 >> 0x14 & 0xF)) // nybble 7
    , stretchDistance(static_cast<float>(this->settings1 >> 0x10 & 0xF) * 16) // nybble 8
    , topLength(static_cast<float>(this->settings1 >> 0xC & 0xF) * 16) // nybble 9
    , stemModel(nullptr)
    , polylineCollider()
{ }


u32 UpDownMushroom::onCreate() {
    this->position.y -= 8.0f;
    this->position.x += 8.0f;

    if (this->stretchSpeed == 0.0f) {
        this->stretchSpeed = 1.0f;
    }

    if (this->topLength == 0.0f) {
        this->topLength = 3.0f;
    }

    u8 type = (this->isUp) ? PlatformModel::Type::PinkMushroom : PlatformModel::Type::YellowMushroom;
    this->topModel.create(type, this->topLength / 16.0f);


    // placeholder until i can *actually* get a stem model
    this->stemModel = ModelWrapper::create("star_coin", "star_coinA");

    this->collisionInfo.radius = Vec2f(this->topLength / 2.0f, 1.0f);
    this->hitboxCollider.init(this, &UpDownMushroom::collisionInfo);
    this->addHitboxColliders();

    Vec2f points[2] = {
        Vec2f(-this->topLength / 2.0f, 12.0f), Vec2f(this->topLength / 2.0f, 12.0f)
    };

    PolylineCollider::Info polylineColliderInfo = { Vec2f(0.0f, 0.0f), 0, 0, points, 0 };
    
    this->polylineCollider.init(this, polylineColliderInfo, 2);
    this->polylineCollider.setType(ColliderBase::Type::Solid);

    ColliderMgr::instance()->add(&this->polylineCollider);
    
    this->destinationY = (this->isUp) ? this->position.y + this->stretchDistance : this->position.y - this->stretchDistance;
    this->doStateChange(&UpDownMushroom::StateID_Stationary);
    return this->onExecute();
}

u32 UpDownMushroom::onExecute() {
    Mtx34 mtx;
    mtx.makeRTIdx(this->rotation, this->position);
    this->stemModel->setMtx(mtx);
    this->stemModel->setScale(this->scale);
    this->stemModel->updateModel();
    this->stemModel->updateAnimations();

    Vec3f platformPos = this->position;
    platformPos.y += 4;
    platformPos.x -= topLength / 2;
    this->topModel.update(platformPos, this->topLength);

    this->polylineCollider.rotation = this->rotation.z;
    this->polylineCollider.execute();

    return 1;
}


u32 UpDownMushroom::onDraw() {
    this->stemModel->draw();
    this->topModel.draw();
    return 1;
}


/** STATE: Stationary */

void UpDownMushroom::beginState_Stationary() { }
void UpDownMushroom::executeState_Stationary() { }
void UpDownMushroom::endState_Stationary() { }

/** STATE: Move */

void UpDownMushroom::beginState_Move() { }
void UpDownMushroom::executeState_Move() {
    if (this->isUp) {
        if (!this->currentlyStretched) {
            if (this->position.y != this->destinationY) {
                this->position.y++;
                this->currentlyStretched = false;
            } else {
                this->currentlyStretched = true;
            }
        }


    } else {
        if (this->position.y != this->destinationY) {
            this->position.y--;
        }
    }
}
void UpDownMushroom::endState_Move() { }

