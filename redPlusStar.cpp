#include <game.h>
#include <sfx.h>
#include <g3dhax.h>
const char* RedPlusANL [] = { "I_star", NULL };

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
    // bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
    // bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther); 
	//bool collisionCat8_FencePunch(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther); 
	// bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);

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
    S16Vec nullRot = {0,0,0}; 
    ec efScale = {0.05f, 0.05f, 0.05f};
	Vec oneVec = {1.0f, 1.0f, 1.0f};
    int gfxTimer;

    dStageActor_c* goomba; // <- These were used for testing
    Vec goombapos;
    // this->goombapos = (Vec){pos.x, pos.y - 32, pos.z}; // <-Use these to test things
    // this->goomba = CreateActor(EN_KURIBO, 0, this->goombapos, 0, 0);

    USING_STATES(daRedPlusStar_c);
    DECLARE_STATE(Existing);
    DECLARE_STATE(Collected);
};
// not finished, don't forget to add to NewerProjectKP or whaterv
// and also replace an actor, very important