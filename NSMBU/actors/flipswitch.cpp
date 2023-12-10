#include "game/actor/actormgr.h"
#include "tsuru/actor/blockwrapper.h"
#include "game/graphics/model/modelnw.h"
#include "game/eventmgr.h"
#include "game/sound/effects.h"
#include "log.h"
class FlipSwitchMgr;

class FlipSwitch : public BlockWrapper {
    SEAD_RTTI_OVERRIDE_IMPL(FlipSwitch, BlockWrapper);

public:
    FlipSwitch(const ActorBuildInfo* buildInfo);
    virtual ~FlipSwitch() { }


    u32 onCreate() override;
    u32 onExecute() override;
    u32 onDraw() override;


    void spawnItemUp() override;
    void spawnItemDown() override;

    ModelWrapper* model;
    FlipSwitchMgr* mgr;
    u8 mgrID;
    bool wasHit;
    bool isSmall; // 3 blocks if false, 2 blocks if true

    void beginState_Used() override;
};


class FlipSwitchMgr : public StageActor {
    SEAD_RTTI_OVERRIDE_IMPL(FlipSwitchMgr, StageActor);

public:
    FlipSwitchMgr(const ActorBuildInfo* buildInfo);
    virtual ~FlipSwitchMgr() {}

    u32 onCreate() override;
    u32 onExecute() override;

    u8 targetEventID;
    u32 activatedCount;
    u32 targetCount;
    u8 id;
};

PROFILE_RESOURCES(ProfileID::FlipSwitch, Profile::LoadResourcesAt::Course, "block_flipstch");
REGISTER_PROFILE(FlipSwitch, ProfileID::FlipSwitch);
REGISTER_PROFILE(FlipSwitchMgr, ProfileID::FlipSwitchMgr);

FlipSwitch::FlipSwitch(const ActorBuildInfo* buildInfo)
    : BlockWrapper(buildInfo)
    , mgrID(buildInfo->eventID1 & 0xFF) // nybble 1-2
    , isSmall(buildInfo->eventID2 >> 0x4 & 0xF) // nybble 3
    , model(nullptr)
    , wasHit(false)
    , mgr(nullptr)
{ }

FlipSwitchMgr::FlipSwitchMgr(const ActorBuildInfo* buildInfo)
    : StageActor(buildInfo)
    , id(buildInfo->eventID1 & 0xFF) // nybble 1-2
    , targetEventID((buildInfo->eventID2 & 0xFF) - 1) // nybbles 3-4
    , targetCount(buildInfo->settings1 >> 0x18 & 0xFF) // nybbles 5-6
    , activatedCount(0) 
{ }


u32 FlipSwitch::onCreate() {
    if (!BlockWrapper::init()) {
        return 2;
    }

    if (this->isSmall) {
        this->model = ModelWrapper::create("block_flipstch", "block_flipstch", 0, 2);
        this->model->playTexPatternAnim("flip_two_wide");
    } else {
        this->model = ModelWrapper::create("block_flipstch", "block_flipstch_wide", 0, 2);
        this->model->playTexPatternAnim("flip_three_wide");
    }

    this->model->texPatternAnims[0]->frameCtrl.speed = 0;
    this->model->texPatternAnims[0]->frameCtrl.currentFrame = 0; // untouched

    this->position.x += 8;
    this->position.y -= 8;
    // come back to this and modify for 2 block wide
    
    this->rectCollider.points[0].x -= 16.0f / (isSmall + 1);
    this->rectCollider.points[1].x += 16.0f / (isSmall + 1);
    this->rectCollider.points[2].x += 16.0f / (isSmall + 1);
    this->rectCollider.points[3].x -= 16.0f / (isSmall + 1);


    if (BlockWrapper::stateType == StateType::UsedBlock) {
        doStateChange(&FlipSwitch::StateID_Used);
    }

    return BlockWrapper::onExecute();
}


u32 FlipSwitch::onExecute() {
    Mtx34 mtx;
    mtx.makeRTIdx(this->rotation, this->position + Vec3f(0.0f, 8.0f, 0.0f));

    this->model->setMtx(mtx);
    this->model->updateAnimations();
    this->model->updateModel();

    for (Actor** it = ActorMgr::instance()->actors.start.buffer; it != ActorMgr::instance()->actors.end.buffer; ++it) { // search for FlipSwitchMgr actors with a matching ID
        if (*it != nullptr) {
            Actor& actor = **it;
            if (actor.getProfileID() == ProfileID::FlipSwitchMgr) {
                FlipSwitchMgr* mgr = static_cast<FlipSwitchMgr*>(&actor);
                if (mgr->id == this->mgrID) {
                    this->mgr = mgr;
                    break;
                }
            }
        }
    }

    return BlockWrapper::onExecute();
}

u32 FlipSwitch::onDraw() {
    this->model->draw();

    return 1;
}

u32 FlipSwitchMgr::onCreate() {

    return this->onExecute();
}

u32 FlipSwitchMgr::onExecute() {
    if (this->activatedCount >= this->targetCount) {
        // play a sound effect
        EventMgr::instance()->set(this->targetEventID, 0, true);
        this->isDeleted = true;
        
        // look for any individual panels with a matching manager id and set them all to be used
        for (Actor** it = ActorMgr::instance()->actors.start.buffer; it != ActorMgr::instance()->actors.end.buffer; ++it) {
            if (*it != nullptr) {
                Actor& actor = **it;
                if (actor.getProfileID() == ProfileID::FlipSwitch) {
                    FlipSwitch* individualBlock = static_cast<FlipSwitch*>(&actor);
                    if (individualBlock->mgrID == this->id)
                        individualBlock->doStateChange(&FlipSwitch::StateID_Used);
                }
            }
        }
    }
    
    return 1;
}



// block stuffs

void FlipSwitch::spawnItemUp() {
    // determine if our assigned manager's target event has been activated
    for (Actor** it = ActorMgr::instance()->actors.start.buffer; it != ActorMgr::instance()->actors.end.buffer; ++it) {
        if (*it != nullptr) {
            Actor& actor = **it;
            if (actor.getProfileID() == ProfileID::FlipSwitchMgr) {
                FlipSwitchMgr* manager = static_cast<FlipSwitchMgr*>(&actor);
                if (this->mgrID == manager->id) {
                    if (EventMgr::instance()->isActive(manager->targetEventID)) {
                        this->doStateChange(&BlockBase::StateID_Used);
                        break;
                    }
                    else {
                        this->model->texPatternAnims[0]->frameCtrl.currentFrame = !this->model->texPatternAnims[0]->frameCtrl.currentFrame;
                        this->wasHit = !this->wasHit;
                        if (this->wasHit) {
                            // play sound effect
                            this->mgr->activatedCount++;
                        }
                        else {
                            // play sound effect
                            this->mgr->activatedCount--;
                        }
                        this->doStateChange(&StateID_Active);
                    }
                }

            }
        }
    }
}

void FlipSwitch::spawnItemDown() {
    FlipSwitch::spawnItemUp();
}


// state

void FlipSwitch::beginState_Used() {
    this->_1AAE = 0; // bouncy block bug fix
    this->model->texPatternAnims[0]->frameCtrl.currentFrame = 2; // completed
    BlockWrapper::beginState_Used();
}

