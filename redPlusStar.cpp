#include <game.h>
#include <sfx.h>
#include <g3dhax.h>

const char* RedPlusANL [] = { "I_star", "kuribo", NULL };
class daRedPlusStar_c : public dEn_c {
public:
    int onCreate();
    int onExecute();
    int onDelete();
    int onDraw();

    void updateModelMatrices();
    void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

    static daRedPlusStar_c* build();

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
    int star1;
    int star2;
    int star3;
    int star4;
    int star5;
    // some other variables i might need but i don't know just yet
    S16Vec nullRot = {0,0,0}; // For effects
    Vec efScale = {0.05f, 0.05f, 0.05f};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
    int gfxTimer;
    int collectionCount; // Once this number reaches 5, a powerup is awarded

    dStageActor_c* goomba; // <- These were used for testing
    Vec goombapos;
    // this->goombapos = (Vec){pos.x, pos.y - 32, pos.z}; // <-Use these to test things
    // this->goomba = CreateActor(EN_KURIBO, 0, this->goombapos, 0, 0);

    USING_STATES(daRedPlusStar_c);
    DECLARE_STATE(Existing);
    DECLARE_STATE(Collected);
};

CREATE_STATE(daRedPlusStar_c, Existing);
CREATE_STATE(daRedPlusStar_c, Collected);



void daRedPlusStar_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    this->collectionCount +=1;
    if (this->collectionCount == 5) {
        // this->collectionCount = 0;
        this->goombapos = (Vec){pos.x, pos.y + 32, pos.z}; // <-Use these to test things
        this->goomba = CreateActor(EN_KURIBO, 0, this->goombapos, 0, 0);
    }
    doStateChange(&StateID_Collected);
}
bool daRedPlusStar_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
    
    doStateChange(&StateID_Collected);
    return true;
}
bool daRedPlusStar_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Collected);
    return true;
}
bool daRedPlusStar_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Collected);
    return true;
}
bool daRedPlusStar_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Collected);
    return true;
}
bool daRedPlusStar_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Collected);
    return true;
}
bool daRedPlusStar_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Collected);
    return true;
}
bool daRedPlusStar_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Collected);
    return true;
}
bool daRedPlusStar_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
    // do nothing probably
    return true;
}
bool daRedPlusStar_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
    // do nothing probably
    return true;
}
bool daRedPlusStar_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
    // do nothing probably
    return true;
}
bool daRedPlusStar_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
    // do nothing probably
    return true;
}
bool daRedPlusStar_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
    doStateChange(&StateID_Collected);
    return true;
}

// Model and animation stuff probably
void daRedPlusStar_c::updateModelMatrices() {
    matrix.translation(pos.x, pos.y, pos.x);
    void applyRotationYXZ(s16 *x, s16 *y, s16 *z);

    bodyModel.setDrawMatrix(matrix);
    bodyModel.setScale(&scale);
    bodyModel.calcWorld(false);
}
void daRedPlusStar_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
    nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
    this->animationChr.bind(&this->bodyModel, anmChr, unk);
    this->bodyModel.bindAnim(&this->animationChr, unk2);
    this->animationChr.setUpdateRate(rate);
}

// Important functions
daRedPlusStar_c* daRedPlusStar_c::build() {
    void *buffer = AllocFromGameHeap1(sizeof(daRedPlusStar_c));
    return new(buffer) daRedPlusStar_c;
}
int daRedPlusStar_c::onCreate() {
    this->collectionCount = 0;

    allocator.link(-1, GameHeaps[0], 0, 0x20);

    resFile.data = getResource("I_star", "g3d/five_star.brres");
    nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("I_star");
    bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
    SetupTextures_Enemy(&bodyModel, 0);
    nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr("wait2");
    this->animationChr.setup(mdl, anmChr, &this->allocator, 0);
    
    allocator.unlink();

    ActivePhysics::Info HitMeBaby;
    HitMeBaby.xDistToCenter = 0.0; 
	HitMeBaby.yDistToCenter = 8.0; 
    HitMeBaby.xDistToEdge = 8.0; 
	HitMeBaby.yDistToEdge = 8.0; 
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

    bindAnimChr_and_setUpdateRate("wait2", 1, 0.0, 1.0);

    this->onExecute();
    return true;
}
int daRedPlusStar_c::onExecute() {
    acState.execute();
    updateModelMatrices();
    bodyModel._vf1C();

    if (this->animationChr.isAnimationDone()) {
        this->animationChr.setCurrentFrame(0.0);
    }
   
    return true;
}
int daRedPlusStar_c::onDelete() {
    return true;
}
int daRedPlusStar_c::onDraw() {
    bodyModel.scheduleForDrawing();
    return true;
}

// States
    // Existing
void daRedPlusStar_c::beginState_Existing() {
    this->timer = 0;
    bindAnimChr_and_setUpdateRate("wait2", 1, 0.0, 0.0);
}
void daRedPlusStar_c::executeState_Existing() {
    
    this->timer +=1;
}
void daRedPlusStar_c::endState_Existing() { }

    // Collected
void daRedPlusStar_c::beginState_Collected() {
    this->timer = 0;
    PlaySound(this, SE_EMY_DOWN); // testing
    this->removeMyActivePhysics();
   
}
void daRedPlusStar_c::executeState_Collected() {
    
    this->Delete(1);
    this->timer +=1;
}
void daRedPlusStar_c::endState_Collected() { }