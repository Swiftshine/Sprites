#include "game/task/coursetask.h"
#include "game/graphics/model/modelnw.h"
#include "game/actor/stage/stageactor.h"
#include "game/audio/gameaudio.h"
#include "game/effect/effect.h"
#include "sead/random.h"
#include "log.h"

class MultiCoin : public StageActor {
    SEAD_RTTI_OVERRIDE_IMPL(MultiCoin, StageActor);

public:
    MultiCoin(const ActorBuildInfo* buildInfo);
    virtual ~MultiCoin() { }

    u32 onCreate() override;
    u32 onExecute() override;
    u32 onDraw() override;

    static void collisionCallback(HitboxCollider* hcSelf, HitboxCollider* hcOther);

    static const HitboxCollider::Info collisionInfo;

    ModelWrapper* model;
    u32 coinsToAdd;
};

REGISTER_PROFILE(MultiCoin, ProfileID::MultiCoin);
PROFILE_RESOURCES(ProfileID::MultiCoin, Profile::LoadResourcesAt::Course, "star_coin");

const HitboxCollider::Info MultiCoin::collisionInfo = {
    Vec2f(0.0f, 0.0f), Vec2f(16.0f, 16.0f), HitboxCollider::Shape::Rectangle, 5, 0, 0x824F, 0x20208, 0, &MultiCoin::collisionCallback
};

MultiCoin::MultiCoin(const ActorBuildInfo* buildInfo)
    : StageActor(buildInfo)
    , model(nullptr)
    , coinsToAdd(0)
{ }

u32 MultiCoin::onCreate() {
    this->model = ModelWrapper::create("star_coin", "star_coinA");

    this->hitboxCollider.init(this, &MultiCoin::collisionInfo);
    this->addHitboxColliders();

    switch (this->settings1 >> 0x1C & 0xF) { // nybble 5
        case 1: {
            this->coinsToAdd = 20;
            break;
        }
        case 2: {
            this->coinsToAdd = 50;
            break;
        }
        case 3: {
            this->coinsToAdd = 100;
            break;
        }
        case 4: {
            this->coinsToAdd = sead::randS32(1, 100);
            break;
        }
        default: {
            this->coinsToAdd = 10;
            break;
        }
    }
    return this->onExecute();
}

u32 MultiCoin::onExecute() {
    Mtx34 mtx;
    mtx.makeRTIdx(this->rotation, this->position);

    this->rotation.y += fixDeg(5.0f);

    this->model->setMtx(mtx);
    this->model->updateModel();
    return 1;
}

u32 MultiCoin::onDraw() {
    this->model->draw();
    return 1;
}

void MultiCoin::collisionCallback(HitboxCollider* hcSelf, HitboxCollider* hcOther) {
    MultiCoin* self = (MultiCoin*)hcSelf->owner;
    CourseTask::instance()->incrementCoinCounter(0, self->coinsToAdd);
    GameAudio::startSoundMap(SoundEffects::SE_OBJ_GET_COIN_SHOWER, self->position);
    Vec3f newscale = 1.5f;
    Effect::spawn(RP_CoinNormalGet, &self->position, nullptr, &newscale);
    self->isDeleted = true;
}
