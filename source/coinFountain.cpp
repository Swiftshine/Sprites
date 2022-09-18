#include <common.h>
#include <game.h>

class dCoinFountain_c : public dEn_c {
public:

	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();

	
	

	static dCoinFountain_c* build();

    int coinCount;
    int numberOfSpouts;
    bool done;
	bool burst;
	int directionForBurst;
	// int spawnEffect; // save this for nybble 12, i guess.
	/*
	0 - none
	1 - effect
	2 - sfx
	3 - both
	*/
    u8 eventID;

	USING_STATES(dCoinFountain_c);
	DECLARE_STATE(Rest);
	DECLARE_STATE(Spout);
};

CREATE_STATE(dCoinFountain_c, Rest);
CREATE_STATE(dCoinFountain_c, Spout);

dCoinFountain_c* dCoinFountain_c::build() {
	void *buffer = AllocFromGameHeap1(sizeof(dCoinFountain_c));
	return new(buffer) dCoinFountain_c;
}
// what the fuck am i doing.

int dCoinFountain_c::onCreate() {

    this->coinCount = (settings >> 28 & 0xF);   // nybble 5 used for bursts
    this->numberOfSpouts = (settings >> 24 & 0xF);  // nybble 6; see below
	/* numberOfSpouts chart
	0 - 1 spout (facing right) - DURATION
	1 - 1 spout (facing left) - DURATION
	2 - 2 spouts (facing opposite directions)
	*/
    this->eventID = (settings >> 16 & 0xFF) - 1;    // nybble 7-8
	// this->spawnEffect = (settings >> 12 & 0xF); originally used for nybble 9 but not rn
	this->directionForBurst = (settings >> 12 & 0xF); // nybble 9
	/*
	0 - right
	1 - left
	2 - both
	*/

	if (this->directionForBurst <= 2) {
		this->burst = true;
	}
	else {
		this->burst = false;
	}
	this->done = false;



	this->pos.y -= 32.0;
	this->pos.x += 8.0;
	doStateChange(&StateID_Rest);



	this->onExecute();

	return true;
}

int dCoinFountain_c::onExecute() {
	acState.execute();

	return true;
}

int dCoinFountain_c::onDelete() {
	return true;
}

int dCoinFountain_c::onDraw() {
	return true;
}

void dCoinFountain_c::beginState_Rest() { }
void dCoinFountain_c::executeState_Rest() { 
	if (dFlagMgr_c::instance->active(this->eventID) && !this->done) {
		doStateChange(&StateID_Spout);
	}
	if (this->done) {
		this->Delete(1);
	}
}
void dCoinFountain_c::endState_Rest() { }

void dCoinFountain_c::beginState_Spout() {
	this->done = false;

	// if (this->spawnEvent) { do something later }
}
void dCoinFountain_c::executeState_Spout() { 

	if (!this->burst) {
		switch (this->numberOfSpouts) {
			case 0: {
				StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
				break;
			}
			case 1: {
				StageE4::instance->spawnCoinJump(pos, 1, 1, 0);
				break;
			}
			default: {
				StageE4::instance->spawnCoinJump(pos, 1, 1, 0);
				StageE4::instance->spawnCoinJump(pos, 0, 1, 0);
				break;
			}
		}
	}
	else { // burst is true
		if (this->directionForBurst < 1) {
			StageE4::instance->spawnCoinJump(pos, 1, this->coinCount, 0);
			StageE4::instance->spawnCoinJump(pos, 0, this->coinCount, 0);
		}
		else {
			StageE4::instance->spawnCoinJump(pos, this->directionForBurst, this->coinCount, 0);
		}
	}

	this->done = true;
	doStateChange(&StateID_Rest);
}
void dCoinFountain_c::endState_Spout() {} // doesn't have a .yaml just yet, so be aware of that


// notes


// in void spawnCoinJump(const VEC3 &pos, int direction, int coinCount, int layer), int direction is:
// 0: right
// 1: left