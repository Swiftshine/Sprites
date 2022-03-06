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
    int movementTimer;
    bool amIMoving; // If false, then the STOP state will not change to the MOVE state.
    int verticalOrHorizontal; // In which way am I moving? 0 is just there, 1 is vertical, 2 is horizontal.
    bool upOrDown; // Are we moving up or down?
    bool leftOrRight; // Are we moving left or right?
    bool haveIShot; // Have I fired a fireball yet?
    S16Vec nullRot = {0,0,0}; // For effects
    Vec efScale = {0.05f, 0.05f, 0.05f};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
    bool wasShotWithIce; // Did I get hit by an ice projectile?
    int gfxTimer;
    int speedMultiplier = (settings >> 24 & 0xF); // Nybble 6
    int speed = 1;

    void handleSomeOptions();

    dStageActor_c* goomba; // <- These were used for testing
    Vec goombapos;
    // this->goombapos = (Vec){pos.x, pos.y - 32, pos.z}; // <-Use these to test things
    // this->goomba = CreateActor(EN_KURIBO, 0, this->goombapos, 0, 0);

    USING_STATES(daFireFuzzy_c);
    DECLARE_STATE(Move);
    DECLARE_STATE(Stop);
    DECLARE_STATE(Shoot);
    DECLARE_STATE(Die);
};

CREATE_STATE(daFireFuzzy_c, Move);
CREATE_STATE(daFireFuzzy_c, Stop);
CREATE_STATE(daFireFuzzy_c, Shoot);
CREATE_STATE(daFireFuzzy_c, Die);

// Misc Functions
void daFireFuzzy_c::handleSomeOptions() {
    if (this->speedMultiplier != 0) {
    this->speed = this->speed * this->speedMultiplier;
    }
}

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
// Model and animation stuff
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

// onCreate, onExecute, onDelete, onDraw
daFireFuzzy_c* daFireFuzzy_c::build() {
    void *buffer = AllocFromGameHeap1(sizeof(daFireFuzzy_c));
    return new(buffer) daFireFuzzy_c;
}
int daFireFuzzy_c::onCreate() {
    allocator.link(-1, GameHeaps[0], 0, 0x20);

    resFile.data = getResource("chorobon_flame", "g3d/chorobon_flame.brres");
    nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("chorobon_flame");
    bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
    SetupTextures_Enemy(&bodyModel, 0);
    nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("run");
    this->animationChr.setup(mdl, anmChr, &this->allocator, 0);

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

    handleSomeOptions();

    switch (settings >> 28 & 0xF) { // Nybble 5
    // Value        Behavior
    // 0            Vertical movement; Up first
    // 1            Vertical movement; Down first
    // 2            Horizontal movement; Left first
    // 3            Horizontal movement; Right first
    // else         Stationary
    case 0: {
        this->amIMoving = true;
        this->verticalOrHorizontal = 1;
        this->upOrDown = true;
        doStateChange(&StateID_Stop);
        break;
    }
    case 1: {
        this->amIMoving = true;
        this->verticalOrHorizontal = 1;
        this->upOrDown = false;
        doStateChange(&StateID_Stop);
        break;
    }
    case 2: {
        this->amIMoving = true;
        this->verticalOrHorizontal = 2;
        this->leftOrRight = true;
        doStateChange(&StateID_Stop);
        break;
    }
    case 3: {
        this->amIMoving = true;
        this->verticalOrHorizontal = 2;
        this->leftOrRight = false;
        doStateChange(&StateID_Stop);
        break;
    }
    case 4: {
        this->amIMoving = false;
        doStateChange(&StateID_Stop);
        break;
    }
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

// States, cleaned up this time
    // Move state
void daFireFuzzy_c::beginState_Move() {
    this->timer = 0;
    this->gfxTimer = 0;
    this->movementTimer = 0;
    bindAnimChr_and_setUpdateRate("run", 1, 0.0, 1.0);
}
void daFireFuzzy_c::executeState_Move() {


if (amIMoving) {
    if (this->verticalOrHorizontal != 2) { // If we are moving vertically...
        if (this->upOrDown) { //
            this->pos.y -=this->speed;
            this->haveIShot = false;
        }
        else { // ...and moving downwards...
            this->pos.y +=this->speed;
            this->haveIShot = false;
        }
    }
    else { // If we are moving horizontally...
        if (this->leftOrRight) { // 
            this->pos.x +=this->speed;
            this->haveIShot = false;
        }
        else { // ...and moving rightwards...
            this->pos.x -=this->speed;
            this->haveIShot = false;
        }
    }

    if (this->gfxTimer == 30) {
        this->gfxTimer = 0;
        SpawnEffect("Wm_mr_fireball_hit", 0, &this->pos, &nullRot, &oneVec);
    }

    if (this->movementTimer == 150) { // I eventually plan to add in a "movement time" option, but I need to fix this entire thing first.
        this->movementTimer = 0;
        doStateChange(&StateID_Stop);
    }

    this->timer +=1;
    this->gfxTimer +=1;
    this->movementTimer +=1;
    }
}

void daFireFuzzy_c::endState_Move() { }
    // Stop state
void daFireFuzzy_c::beginState_Stop() {
    this->timer = 0;
    this->movementTimer = 0;
}

void daFireFuzzy_c::executeState_Stop() {
    if (timer == 120 && !haveIShot) {
        doStateChange(&StateID_Shoot);
    }

    if (timer == 150 && amIMoving) {
        this->upOrDown = !this->upOrDown;
        this->leftOrRight = !this->leftOrRight;
        doStateChange(&StateID_Move);
    }

    if (timer == 150 && !amIMoving) {
        doStateChange(&StateID_Shoot);
    }

    this->timer +=1;
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
    if (timer == 30) {
            doStateChange(&StateID_Stop);
    }
    this->timer +=1;
}

void daFireFuzzy_c::endState_Shoot() { }
    // Die state
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
// not done yet