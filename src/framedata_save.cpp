#include "framedata.h"
#include <fstream>
#include <cstdint>

#define VAL(X) ((const char*)&X)
#define PTR(X) ((const char*)X)

// The order of these things is a bit different from the order the original game files use.
// (Because I haven't figured out the proper order lol)
// I don't know if it can cause trouble but it's something to keep in mind.

void WriteAF(std::ofstream &file, const Frame_AF *af)
{
	file.write("AFST", 4);

	file.write("AFGP", 4);
	uint32_t pat = af->usePat;
	file.write(VAL(pat), 4);
	file.write(VAL(af->spriteId), 4);

	//AFY can fuck off
	if(af->offset_x || af->offset_y){
		file.write("AFOF", 4);
		file.write(VAL(af->offset_x), 4);
		file.write(VAL(af->offset_y), 4);
	}

	if(af->duration >=0 && af->duration < 10){
		char t = af->duration + '0';
		file.write("AFD", 3);
		file.write(VAL(t), 1);
	}
	else{
		file.write("AFDL", 4);
		file.write(VAL(af->duration), 4);
	}

	if(af->aniType){
		char t = af->aniType + '0';
		file.write("AFF", 3);
		file.write(VAL(t), 1);
	}

	if(af->aniFlag){
		file.write("AFFE", 4);
		file.write(VAL(af->aniFlag), 4);
	}

	if(af->blend_mode){
		file.write("AFAL", 4);
		int anormalized = af->rgba[3]*255.f;
		file.write(VAL(af->blend_mode), 4);
		file.write(VAL(anormalized), 4);
	}
	else if (af->rgba[3] != 1.f){
		int type = 1;
		int anormalized = af->rgba[3]*255.f;
		file.write("AFAL", 4);
		file.write(VAL(type), 4);
		file.write(VAL(anormalized), 4);
	}

	if(	af->rgba[0] != 1.f ||
		af->rgba[1] != 1.f ||
		af->rgba[2] != 1.f)
	{
		int anormalized[3];
		for(int i = 0; i < 3; i++)
			anormalized[i] = af->rgba[i]*255.f;
		file.write("AFRG", 4);
		file.write(PTR(anormalized), 3*sizeof(float));
	}

	if(af->rotation[0]){
		file.write("AFAX", 4);
		file.write(VAL(af->rotation[0]), sizeof(float));
	}
	if(af->rotation[1]){
		file.write("AFAY", 4);
		file.write(VAL(af->rotation[1]), sizeof(float));
	}
	if(af->rotation[2]){
		file.write("AFAZ", 4);
		file.write(VAL(af->rotation[2]), sizeof(float));
	}
	if(	af->scale[0] != 1.f ||
		af->scale[1] != 1.f){
		file.write("AFZM", 4);
		file.write(PTR(af->scale), 2*sizeof(float));
	}

	if(af->jump){
		file.write("AFJP", 4);
		file.write(VAL(af->jump), 4);
	}
	if(af->interpolationType){
		file.write("AFHK", 4);
		file.write(VAL(af->interpolationType), 4);
	}
	if(af->priority){
		file.write("AFPR", 4);
		file.write(VAL(af->priority), 4);
	}
	if(af->loopCount){
		file.write("AFCT", 4);
		file.write(VAL(af->loopCount), 4);
	}
	if(af->loopEnd){
		file.write("AFLP", 4);
		file.write(VAL(af->loopEnd), 4);
	}
	if(af->landJump){
		file.write("AFJC", 4);
		file.write(VAL(af->landJump), 4);
	}
	if(af->AFRT){
		int val = af->AFRT;
		file.write("AFRT", 4);
		file.write(VAL(val), 4);
	}
	
	file.write("AFED", 4);
}

void WriteAS(std::ofstream &file, const Frame_AS *as)
{
	file.write("ASST", 4);

	if((as->movementFlags & 0x11) == 0x11 && 
		as->speed[0] == 0 &&
		as->speed[1] == 0 &&
		as->accel[0] == 0 &&
		as->accel[1] == 0
	){
		file.write("ASVX", 4);
	}
	else if(as->movementFlags != 0 || 
		as->speed[0] != 0 ||
		as->speed[1] != 0 ||
		as->accel[0] != 0 ||
		as->accel[1] != 0
	){
		file.write("ASV0", 4);
		file.write(VAL(as->movementFlags), 4);
		file.write(PTR(as->speed), 2*4);
		file.write(PTR(as->accel), 2*4);
	}

	if(as->canMove){
		int val = as->canMove;
		file.write("ASMV", 4);
		file.write(VAL(val), 4);
	}
	if(as->stanceState){
		char t = as->stanceState + '0';
		file.write("ASS", 3); //lmaop
		file.write(VAL(t), 1);
	}
	if(as->cancelNormal){
		file.write("ASCN", 4);
		file.write(VAL(as->cancelNormal), 4);
	}
	if(as->cancelSpecial){
		file.write("ASCS", 4);
		file.write(VAL(as->cancelSpecial), 4);
	}
	if(as->counterType){
		file.write("ASCT", 4);
		file.write(VAL(as->counterType), 4);
	}
	if(as->statusFlags[0])
	{
		file.write("ASF0", 4);
		file.write(VAL(as->statusFlags[0]), 4);
	}
	if(as->statusFlags[1])
	{
		file.write("ASF1", 4);
		file.write(VAL(as->statusFlags[1]), 4);
	}
	if(as->maxSpeedX){
		file.write("ASMX", 4);
		file.write(VAL(as->maxSpeedX), 4);
	}
	if(as->sineFlags)
	{
		file.write("AST0", 4);
		file.write(VAL(as->sineFlags), 4);
		file.write(PTR(as->sineParameters), 4*4);
		file.write(PTR(as->sinePhases), 2*sizeof(float));
	}
	if(as->hitsNumber){
		file.write("ASAA", 4);
		file.write(VAL(as->hitsNumber), 4);
	}
	if(as->invincibility){
		file.write("ASYS", 4);
		file.write(VAL(as->invincibility), 4);
	}


	file.write("ASED", 4);
}


void WriteAT(std::ofstream &file, const Frame_AT *at)
{
	file.write("ATST", 4);

	if(at->guard_flags){
		file.write("ATGD", 4);
		file.write(VAL(at->guard_flags), 4);
	}
	if(at->correction != 100){
		file.write("ATHS", 4);
		file.write(VAL(at->correction), 4);
	}
	{ //Always
		short d[4];
		d[0] = at->red_damage;
		d[1] = at->damage;
		d[2] = at->guard_damage;
		d[3] = at->meter_gain;
		file.write("ATVV", 4);
		file.write(PTR(d), 2*4);
	}
	if(at->correction_type){
		file.write("ATHT", 4);
		file.write(VAL(at->correction_type), 4);
	}
	if(true){
		constexpr int three = 3;
		int val[three];
		
		file.write("ATHV", 4);
		file.write(VAL(three), 4);
		for(int i = 0; i < 3; i++)
			val[i] = at->hitVector[i] | (at->hVFlags[i] << 8);

		file.write(PTR(val), sizeof(val));

		file.write("ATGV", 4);
		file.write(VAL(three), 4);
		for(int i = 0; i < 3; i++)
			val[i] = at->guardVector[i] | (at->gVFlags[i] << 8);

		file.write(PTR(val), sizeof(val));
	}
	if(at->otherFlags){
		file.write("ATF1", 4);
		file.write(VAL(at->otherFlags), 4);
	}
	if(at->hitEffect || at->soundEffect){
		file.write("ATHE", 4);
		file.write(VAL(at->hitEffect), 4);
		file.write(VAL(at->soundEffect), 4);
	}
	if(at->addedEffect){
		file.write("ATKK", 4);
		file.write(VAL(at->addedEffect), 4);
	}
	if(at->hitgrab){
		int val = at->hitgrab;
		file.write("ATNG", 4);
		file.write(VAL(val), 4);
	}
	if(at->extraGravity){
		file.write("ATUH", 4);
		file.write(VAL(at->extraGravity), 4);
	}
	if(at->breakTime){
		file.write("ATBT", 4);
		file.write(VAL(at->breakTime), 4);
	}
	if(at->hitStopTime){
		file.write("ATSN", 4);
		file.write(VAL(at->hitStopTime), 4);
	}
	if(at->untechTime){
		file.write("ATSU", 4);
		file.write(VAL(at->untechTime), 4);
	}
	if(at->hitStop){
		file.write("ATSP", 4);
		file.write(VAL(at->hitStop), 4);
	}
	if(at->blockStopTime){
		file.write("ATGN", 4);
		file.write(VAL(at->blockStopTime), 4);
	}
	file.write("ATED", 4);
}

void WriteEF(std::ofstream &file, const std::vector<Frame_EF> &ef)
{
	constexpr int paramN = 12;
	for(int i = 0; i < ef.size(); i++)
	{
		file.write("EFST", 4);
		file.write(VAL(i), 4);
		file.write("EFTP", 4);
		file.write(VAL(ef[i].type), 4);
		file.write("EFNO", 4);
		file.write(VAL(ef[i].number), 4);
		file.write("EFPR", 4);
		file.write(VAL(paramN), 4);
		file.write(PTR(ef[i].parameters), 12*4);
		file.write("EFED", 4);
	}
}

void WriteIF(std::ofstream &file, const std::vector<Frame_IF> &ef)
{
	constexpr int paramN = 9;
	for(int i = 0; i < ef.size(); i++)
	{
		file.write("IFST", 4);
		file.write(VAL(i), 4);
		file.write("IFTP", 4);
		file.write(VAL(ef[i].type), 4);
		file.write("IFPR", 4);
		file.write(VAL(paramN), 4);
		file.write(PTR(ef[i].parameters), 9*4);
		file.write("IFED", 4);
	}
}

void WriteFrame(std::ofstream &file, const Frame *frame)
{
	file.write("FSTR", 4);
	WriteAF(file, &frame->AF);
	WriteAS(file, &frame->AS);


	bool hasAt = false;
	if(!frame->hitboxes.empty())
	{
		auto maxhurt = frame->hitboxes.lower_bound(25);
		if(maxhurt != frame->hitboxes.begin())
			--maxhurt;

		if(maxhurt->first < 25)
		{
			int val = maxhurt->first+1;
			file.write("FSNH", 4);
			file.write(VAL(val), 4);
		}

		auto maxhit = --(frame->hitboxes.end());
		if(maxhit->first >= 25)
		{
			int val = maxhit->first-25+1;
			file.write("FSNA", 4);
			file.write(VAL(val), 4);
			hasAt = true;
		}
	}

	if(!frame->EF.empty())
	{
		int val = frame->EF.size();
		file.write("FSNE", 4);
		file.write(VAL(val), 4);
	}
	if(!frame->IF.empty())
	{
		int val = frame->IF.size();
		file.write("FSNI", 4);
		file.write(VAL(val), 4);
	}

	constexpr Frame_AT defAT{};
	if(!!memcmp(&frame->AT, &defAT, sizeof(Frame_AT)) || hasAt)
		WriteAT(file, &frame->AT);

	for(const auto& box : frame->hitboxes)
	{
		int index = box.first;
		if(box.first >= 25)
		{
			index -= 25;
			file.write("HRAT", 4);
		}
		else
			file.write("HRNM", 4);

		file.write(VAL(index), 4);
		file.write(PTR(box.second.xy), 4*4);
	}
	
	WriteEF(file, frame->EF);
	WriteIF(file, frame->IF);

	file.write("FEND", 4);
}

void WriteSequence(std::ofstream &file, const Sequence *seq)
{
	//Not used by melty blood, probably.
/* 	if(!seq->codeName.empty()){
		uint32_t size = seq->codeName.size();
		file.write("PTCN", 4);
		file.write(VAL(size), 4);
		file.write(PTR(seq->codeName.data()), size);
	} */
	if(seq->psts){
		file.write("PSTS", 4);
		file.write(VAL(seq->psts), 4);
	}
	if(seq->level){
		file.write("PLVL", 4);
		file.write(VAL(seq->level), 4);
	}
	if(seq->flag){
		file.write("PFLG", 4);
		file.write(VAL(seq->flag), 4);
	}
	if(!seq->name.empty()){
		char buf[32]{};
		uint32_t size = 32;
		strncpy(buf, seq->name.c_str(), 32);
		buf[31] = 0;
		file.write("PTT2", 4);
		file.write(VAL(size), 4);
		file.write(PTR(buf), 32);
	}

	constexpr Frame_AT defAT{};
	if(!seq->frames.empty())
	{
		uint32_t data[8]{};
		data[0] = data[7] = seq->frames.size();
		for(const auto& frame : seq->frames)
		{
			data[1] += frame.hitboxes.size();
			data[2] += frame.EF.size();
			data[3] += frame.IF.size();

			//Do not write if default constructed.
			data[4] += (!!memcmp(&frame.AT, &defAT, sizeof(Frame_AT)));

			//Find number of duplicates and write ASSM instead. Not necessary and very low priority.
			data[6]	+= 1;
		}

		uint32_t size = sizeof(data);

		file.write("PDS2", 4);
		file.write(VAL(size), 4);
		file.write(PTR(data), size);

		for(const auto& frame : seq->frames)
		{
			WriteFrame(file, &frame);
		}
	}
}