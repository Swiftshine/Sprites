#include <game.h>
#include <sfx.h>
#include <g3dhax.h>
const char* FireFuzzyANL [] = { "chorobon_flame", "kuribo", NULL };

class daFireFuzzy_c : public dEn_c {
public:
    int onCreate();
    int onExecute();
    int onDelete();
    int onDraw();

    void updateModelMatrices();
    void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

    static daFireFuzzy_c* build();
    void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
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
    mEf::es2 effects[7];

    int timer;
    int upwardMovementTimer;
    int downwardMovementTimer;
    bool upOrDown;
    bool haveIShot;
    S16Vec nullRot = {0,0,0}; // For effects
    Vec efScale = {0.05f, 0.05f, 0.05f};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
    bool wasShotWithIce;
    int gfxTimer;
    int howAmIMoving; // 0 is "move upwards first", 1 is "move downwards first"
                      // 2 is "move left first", 3 is "move right first", 4 is completely stationary.

    dStageActor_c* goomba; // <- These were used for testing
    Vec goombapos;

    USING_STATES(daFireFuzzy_c);
    DECLARE_STATE(MoveUp);
    DECLARE_STATE(MoveDown);
    DECLARE_STATE(Stop);
    DECLARE_STATE(Shoot);
    DECLARE_STATE(Die);
};

CREATE_STATE(daFireFuzzy_c, MoveUp);
CREATE_STATE(daFireFuzzy_c, MoveDown);
CREATE_STATE(daFireFuzzy_c, Stop);
CREATE_STATE(daFireFuzzy_c, Shoot);
CREATE_STATE(daFireFuzzy_c, Die);

// Collisions
void daFireFuzzy_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->_vf220(apOther->owner); // Damages the player upon normal contact
}
bool daFireFuzzy_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->_vf220(apOther->owner); // Damages the player upon sliding contact
    return true;
}
bool daFireFuzzy_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->_vf220(apOther->owner); // Damages the player when he attempts to drill it
    return true;
}
bool daFireFuzzy_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->_vf220(apOther->owner);
    return true;
}
bool daFireFuzzy_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->_vf220(apOther->owner);
    return true;
}
bool daFireFuzzy_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Die); // Dies when touching starman Mario
    return true;
}
bool daFireFuzzy_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Die); // Dies when a player shoots up from a pipe cannon
    return true;
}
bool daFireFuzzy_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Die); // Dies when the sprite touches a "rolling object"; eg. Any object (that isn't a player) that can be thrown at it
    return true;
}
bool daFireFuzzy_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Shoot); // lol did you really think a fire enemy would be killed by fire? you fool
    return true;
}
bool daFireFuzzy_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Die); // Dies when the sprite is hit by (probably a player's) thrown hammer
    return true;
}
bool daFireFuzzy_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Shoot);
    return true;
}
bool daFireFuzzy_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
    wasShotWithIce = true; 
    timer = 0;
    doStateChange(&StateID_Die); // Kill it, but it dies out like a flame
    return true;
}
bool daFireFuzzy_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->_vf220(apOther->owner);
    return true;
}
// Model and animation stuff probably
void daFireFuzzy_c::updateModelMatrices() {
    matrix.translation(pos.x, pos.y, pos.z);
    void applyRotationYXZ(s16 *x, s16 *y, s16 *z);

    bodyModel.setDrawMatrix(matrix);
    bodyModel.setScale(&scale);
    bodyModel.calcWorld(false);
}
void daFireFuzzy_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
    nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
    this->animationChr.bind(&this->bodyModel, anmChr, unk);
    this->bodyModel.bindAnim(&this->animationChr, unk2);
    this->animationChr.setUpdateRate(rate);
}

// Important functions
daFireFuzzy_c* daFireFuzzy_c::build() {
    void *buffer = AllocFromGameHeap1(sizeof(daFireFuzzy_c));
    return new(buffer) daFireFuzzy_c;
}
int daFireFuzzy_c::onCreate() {

    howAmIMoving = (settings >> 16 & 0xFFFF); // idk how this works yet but i'll figure it out

    allocator.link(-1, GameHeaps[0], 0, 0x20);

    resFile.data = getResource("chorobon_flame", "g3d/chorobon_flame.brres");
    nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("chorobon_flame");
    bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
    SetupTextures_Enemy(&bodyModel, 0);
    nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("run");
    this->animationChr.setup(mdl,anmChr, &this->allocator, 0);

    allocator.unlink();

    ActivePhysics::Info HitMeBaby;
    HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 0.0; 
	HitMeBaby.xDistToEdge = 16.0; 
	HitMeBaby.yDistToEdge = 16.0; 
	HitMeBaby.category1 = 0x3; 
	HitMeBaby.category2 = 0x0; 
	HitMeBaby.bitfield1 = 0x4F; 
	HitMeBaby.bitfield2 = 0xFFFFFFFF; 
	HitMeBaby.unkShort1C = 0; 
	HitMeBaby.callback = &dEn_c::collisionCallback; 
	this->aPhysics.initWithStruct(this, &HitMeBaby); 
	this->aPhysics.addToList(); 

	this->scale.x = 1.0; 
	this->scale.y = 1.0; 
	this->scale.z = 1.0; 

	bindAnimChr_and_setUpdateRate("run", 1, 0.0, 1.0);

    if (howAmIMoving == 4) {
        doStateChange(&StateID_Stop);
    }
    else if (howAmIMoving == 0 || howAmIMoving == 1) {
    doStateChange(&StateID_MoveUp);
    }

    this->onExecute();
    return true;
}

int daFireFuzzy_c::onExecute() {
    acState.execute();
    updateModelMatrices();
    bodyModel._vf1C();

    if (this->animationChr.isAnimationDone()) {
        this->animationChr.setCurrentFrame(0.0);
    }
    return true;
}

int daFireFuzzy_c::onDelete() {
    return true;
}

int daFireFuzzy_c::onDraw() {
    bodyModel.scheduleForDrawing();
    return true;
}

// States
    // Upwards movement state
void daFireFuzzy_c::beginState_MoveUp() {
    this->timer = 0;
    this->upwardMovementTimer = 0;
    bindAnimChr_and_setUpdateRate("run", 1, 0.0, 1.0);
    // this->goombapos = (Vec){pos.x, pos.y - 32, pos.z}; <-These were used for testing
    // this->goomba = CreateActor(EN_KURIBO, 0, this->goombapos, 0, 0);
}
void daFireFuzzy_c::executeState_MoveUp() {
    if (upwardMovementTimer == 150) { // After 2.5 seconds of upward movement...
        this->upwardMovementTimer = 0;
        doStateChange(&StateID_Stop);
    }
    if (gfxTimer == 30) {
        gfxTimer = 0;
        SpawnEffect("Wm_mr_fireball_hit", 0, &this->pos, &nullRot, &oneVec);
        
    }
    this->pos.y +=1;
    this->timer +=1;
    this->gfxTimer +=1;
    this->upwardMovementTimer +=1;
}

void daFireFuzzy_c::endState_MoveUp() { }

    // Downwards movement state
void daFireFuzzy_c::beginState_MoveDown() {
    this->timer = 0;
    this->downwardMovementTimer = 0;
    bindAnimChr_and_setUpdateRate("run", 1, 0.0, 1.0);
}
void daFireFuzzy_c::executeState_MoveDown() {
    if (downwardMovementTimer == 150) { // After 2.5 seconds of downward movement...
        this->downwardMovementTimer = 0;
        doStateChange(&StateID_Stop);
    }
    if (gfxTimer == 30) {
        gfxTimer = 0;
        SpawnEffect("Wm_mr_fireball_hit", 0, &this->pos, &nullRot, &oneVec);
        
    }
    this->pos.y -=1;
    this->timer +=1;
    this->gfxTimer +=1;
    this->downwardMovementTimer +=1;
}
void daFireFuzzy_c::endState_MoveDown() { }

    // Stop (or stationary) state
void daFireFuzzy_c::beginState_Stop() {
    this->timer = 0;
    this->upwardMovementTimer = 0;
    this->downwardMovementTimer = 0;

}

void daFireFuzzy_c::executeState_Stop() {
    
    if (timer == 120 && !haveIShot) { // After 2 seconds of staying still and having not shot before...
        doStateChange(&StateID_Shoot);
    }
    if (timer == 150) { // After 2.5 seconds of staying still after having shot...
        if (howAmIMoving == 0 || howAmIMoving == 1) { // If moving up or down
            if (upOrDown) {
            this->upOrDown = false;
            haveIShot = false;
            // this->goombapos = (Vec){pos.x, pos.y - 32, pos.z};
            // this->goomba = CreateActor(EN_KURIBO, 0, this->goombapos, 0, 0); <- this was used for testing
            doStateChange(&StateID_MoveUp);
            }
            else {
            this->upOrDown = true;
            haveIShot = false;
            doStateChange(&StateID_MoveDown);
            }
        }
        // else if 2 or 3
        else if (howAmIMoving == 4) {
            doStateChange(&StateID_Shoot);
        }
    }
    
    this->timer += 1;

}

void daFireFuzzy_c::endState_Stop() { }

    // Shoot state
void daFireFuzzy_c::beginState_Shoot() {
    this->timer = 0;
    haveIShot = false;
 }
void daFireFuzzy_c::executeState_Shoot() { 
    
    if (timer == 10) {
        PlaySound(this, SE_EMY_PAKKUN_FIRE);
        SpawnEffect("Wm_en_explosion", 0, &this->pos, &nullRot, &oneVec);
        
        
        this->direction = dSprite_c__getXDirectionOfFurthestPlayerRelativeToVEC3(this, this->pos); // copied from mrsun.cpp (angry sun)
        float neg = -1.0;
        if (this->direction == 0) {neg = 1.0;}
        dStageActor_c *spawner = CreateActor(106, 0, this->pos, 0, 0);
        spawner->speed.x = 6.0 * neg;
        spawner->speed.y = -2.5;
        spawner->pos.z = 5550.0;
            
        spawner = CreateActor(106, 0, this->pos, 0, 0);
        spawner->speed.x = 0.0 * neg;
        spawner->speed.y = -6.0;
        spawner->pos.z = 5550.0;
        
        spawner = CreateActor(106, 0, this->pos, 0, 0);
        spawner->speed.x = 3.5 * neg;
        spawner->speed.y = -6.0;
        spawner->pos.z = 5550.0;

        haveIShot = true;
    }
    if (timer == 30)
        doStateChange(&StateID_Stop);
    this->timer +=1;
}


void daFireFuzzy_c::endState_Shoot() { }

    // Die State
void daFireFuzzy_c::beginState_Die() {
    this->timer = 0;
    PlaySound(this, SE_EMY_DOWN);
    if (this->wasShotWithIce) {
        SpawnEffect("Wm_en_landsmoke", 0, &this->pos, &nullRot, &oneVec);
    }
    else {
        SpawnEffect("Wm_en_explosion", 0, &this->pos, &nullRot, &oneVec);
    }

    this->removeMyActivePhysics();
}
void daFireFuzzy_c::executeState_Die() {
    dEn_c::dieFall_Execute();
    
    
    this->Delete(1);
    
    this->timer +=1;
}
void daFireFuzzy_c::endState_Die() { }
