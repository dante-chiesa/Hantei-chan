#ifndef FRAMEDATA_H_GUARD
#define FRAMEDATA_H_GUARD

#include <string>
#include <vector>

#include "hitbox.h"

#include <set>
extern std::set<int> numberSet;
extern int maxCount;

struct Frame_AF {
	// rendering data
	int spriteId;
	bool usePat;

	int		offset_y;
	int		offset_x;

	int		duration;

	/* Animation action
	0 (default): End
	1: Next
	2: Jump to frame
	*/
	int aniType;

	// Bit flags. First 4 bits only
	unsigned int aniFlag;


	int		blend_mode;

	float rgba[4]{1,1,1,1};

	float rotation[3]{}; //XYZ

	float scale[2]{1,1};//xy

	//Depends on aniflag.
	//If (0)end, it jumps to the number of the sequence
	//If (2)jump, it jumps to the number of the frame of the seq.
	//It seems to do nothing if the aniflag is 1(next).
	int jump;

	
	int landJump; //Jumps to this frame if landing.
	//1-5: Linear, Fast end, Slow end, Fast middle, Slow Middle. The last type is not used in vanilla
	int interpolationType; 
	int priority; // Default is 0. Used in throws and dodge.
	int loopCount; //Times to loop, it's the frame just before the loop.
	int loopEnd; //The frame number is not part of the loop.
	
	bool AFRT; //Makes rotation respect EF scale.
};

struct Frame_AS {
	
	//Acceleration is always set if their corresponding XY speed flags are set.
	//To only set accel, set the add flags with 0 speed. 
	unsigned int movementFlags;
	int speed[2];
	int accel[2];
	int maxSpeedX;

	bool canMove;

	int stanceState;
	int cancelNormal;
	int cancelSpecial;
	int counterType;

	int hitsNumber;
	int invincibility;
	unsigned int statusFlags[2];

	//sinewave thing
	//0 Flags - Similar to ASV0
	//1,2 Distance X,Y
	//3,4 Frames per cycle X,Y
	//5,6 Phases X,Y. Use (0.75, 0) for CCW circles
	unsigned int sineFlags;
	int sineParameters[4];
	float sinePhases[2];
	
};

struct Frame_AT {
	unsigned int guard_flags;
	unsigned int otherFlags;

	int correction;
	//default = 0, is set only if lower
	//1 multiplicative
	//2 substractive
	int correction_type; 

	int damage;
	int red_damage;
	int guard_damage;
	int meter_gain;

	//Stand, Air, Crouch
	int guardVector[3];
	int hitVector[3];
	int gVFlags[3];
	int hVFlags[3];

	int hitEffect;
	int soundEffect; //Changes the audio

	int addedEffect; //Lasting visual effect after being hit

	bool hitgrab;

	//Affects untech time and launch vector, can be negative.
	float extraGravity;

	int breakTime;
	int untechTime;
	int hitStopTime; //Default value zero. Overrides common values.
	int hitStop; //From common value list
	int blockStopTime; //Needs flag 16 (0 indexed) to be set
};

struct Frame_EF {
	int		type;
	int		number;
	int		parameters[12];
};

struct Frame_IF {
	int		type;
	int		parameters[9]; //Max used value is 9. I don't know if parameters beyond have any effect..
};

struct Frame {
	Frame_AF AF;
	Frame_AS AS;
	Frame_AT AT;

	std::vector<Frame_EF> EF;
	std::vector<Frame_IF> IF;

	BoxList hitboxes;
};

struct Sequence {
	// sequence property data
	std::string	name;
	std::string codeName;
	
	int psts;
	int level;
	int flag;

	bool empty;
	bool initialized;

	std::vector<Frame> frames;

	Sequence();
};

class FrameData {
private:
	unsigned int	m_nsequences;

	
public:

	bool		m_loaded;
	std::vector<Sequence> m_sequences;
	void initEmpty();
	bool load(const char *filename, bool patch = false);
	void save(const char *filename);
	void saveChar(std::string filename);

	//Probably unnecessary.
	//bool load_move_list(Pack *pack, const char *filename);

	int get_sequence_count();

	Sequence* get_sequence(int n);
	std::string GetDecoratedName(int n);

	void Free();

	FrameData();
	~FrameData();
};

void WriteSequence(std::ofstream &file, const Sequence *seq);


#endif /* FRAMEDATA_H_GUARD */
