#include <game.h>
#include <sfx.h>
#include "boss.h"


const char* GoalRingANL [] = { "red_ring", NULL };

class dGoalRing_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

	static dGoalRing_c* build();

	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);


	mHeapAllocator_c allocator;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c animationChr;
	nw4r::g3d::ResFile resFile;

    int ringColor;
    int pause;
    bool isOnGround;
    int timer;
    static nw4r::snd::StrmSoundHandle victorySoundHandle;
    bool haveWeDoneAction;
    bool invisible;

	USING_STATES(dGoalRing_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Touched);
};

CREATE_STATE(dGoalRing_c, Wait);
CREATE_STATE(dGoalRing_c, Touched);

// externs
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

// collisions
void dGoalRing_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    if (!this->haveWeDoneAction) {
        doStateChange(&StateID_Touched);
    }

}
void dGoalRing_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    if (!this->haveWeDoneAction) {
        doStateChange(&StateID_Touched);
    }

}
void dGoalRing_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

}
bool dGoalRing_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dGoalRing_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dGoalRing_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dGoalRing_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dGoalRing_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dGoalRing_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dGoalRing_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dGoalRing_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dGoalRing_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dGoalRing_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dGoalRing_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
    playerCollision(apThis, apOther);
    return true;
}
bool dGoalRing_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
    playerCollision(apThis, apOther);
    return true;
}
bool dGoalRing_c::collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther) {
    playerCollision(apThis, apOther);
    return true;
}

void dGoalRing_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}


void dGoalRing_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}

dGoalRing_c* dGoalRing_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dGoalRing_c));
	return new(buffer) dGoalRing_c;
}


int dGoalRing_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);
    // spritedata
    this->ringColor = (settings >> 28 & 0xF); // nybble 5

    // vars
    this->invisible = false;

    
    switch (this->ringColor) {
        case 1: {
            resFile.data = getResource("red_ring", "g3d/orange_ring.brres");
            break;
        }
        case 2: {
            resFile.data = getResource("red_ring", "g3d/yellow_ring.brres");
            break;
        }
        case 3: {
            resFile.data = getResource("red_ring", "g3d/green_ring.brres");
            break;
        }
        case 4: {
            resFile.data = getResource("red_ring", "g3d/blue_ring.brres");
            break;
        }
        case 5: {
            resFile.data = getResource("red_ring", "g3d/violet_ring.brres");
            break;
        }
        case 6: {
            resFile.data = getResource("red_ring", "g3d/pink_ring.brres");
            break;
        }
        case 7: {
            resFile.data = getResource("red_ring", "g3d/white_ring.brres");
            break;
        }
        case 8: {
            resFile.data = getResource("red_ring", "g3d/gray_ring.brres");
            break;
        }
        case 9: {
            resFile.data = getResource("red_ring", "g3d/black_ring.brres");
            break;
        }
        case 10: {
            resFile.data = getResource("red_ring", "g3d/ice_ring.brres"); // freeze
            break;
        }
        case 11: {
            resFile.data = getResource("red_ring", "g3d/lava_ring.brres"); // flame
            break;
        }
        default: {
            resFile.data = getResource("red_ring", "g3d/red_ring.brres");
        }
    }
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("red_ring");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&bodyModel, 0);
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("red_ring");
	this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

	allocator.unlink(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0;

	ActivePhysics::Info HitMeBaby; 
	HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 0.0; 
	HitMeBaby.xDistToEdge = 16.0; 
	HitMeBaby.yDistToEdge = 24.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFFFFFF; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList();

	bindAnimChr_and_setUpdateRate("red_ring", 1, 0.0, 1.0);
    this->haveWeDoneAction = false;

	doStateChange(&StateID_Wait);



	this->onExecute();

	return true;
}

int dGoalRing_c::onExecute() {
	acState.execute();
	    updateModelMatrices();
	    bodyModel._vf1C();

	if (this->animationChr.isAnimationDone()) {
		this->animationChr.setCurrentFrame(0.0);
	}

	return true;
}

int dGoalRing_c::onDelete() {
	return true;
}

int dGoalRing_c::onDraw() {
    if (!this->invisible) {
	    bodyModel.scheduleForDrawing();
    }


	return true;
}

void dGoalRing_c::beginState_Wait() { }
void dGoalRing_c::executeState_Wait() { 
    this->rot.y += 0600;
}
void dGoalRing_c::endState_Wait() { }

void dGoalRing_c::beginState_Touched() { 
    this->timer = 0;
    StopBGMMusic();
    MakeMarioEnterDemoMode();
    this->removeMyActivePhysics();
    this->invisible = true;
}
void dGoalRing_c::executeState_Touched() {
    this->isOnGround = true;

    for(int i = 0; i < 4; i++) {
		if(Player_Active[i]) {
			if(!(GetPlayerOrYoshi(i)->collMgr.isOnTopOfTile())) {
				this->isOnGround = false;
			}
		}
	}

    if (this->isOnGround && !this->haveWeDoneAction) {
        PlaySoundWithFunctionB4(SoundRelatedClass, &this->victorySoundHandle, STRM_BGM_COURSE_CLEAR, 1); // hopefully wait about 10 seconds
        BossGoalForAllPlayers();
        while (this->pause != 600) {
            this->pause += 1;
            this->timer = 0;
        }
        this->haveWeDoneAction = true;
    }

    if (this->timer < 180) { // wait about 2 or so seconds?
        this->timer +=1;
    }
    else {
        ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
        // OSReport("Exiting stage!!!!! AAAA \n");
    }
}
void dGoalRing_c::endState_Touched() { }

// basically copying code from the exitStageManager