#include "framedata.h"
#include "framedata_load.h"
#include <fstream>
#include "misc.h"
#include <cstring>
#include <sstream>
#include <iomanip>

int maxCount = 0;
std::set<int> numberSet;

Sequence::Sequence():
psts(0), level(0), flag(0),
empty(true),
initialized(false)
{}

void FrameData::initEmpty()
{
	Free();
	m_nsequences = 1000;
	m_sequences.resize(m_nsequences);
	m_loaded = 1;
}

bool FrameData::load(const char *filename, bool patch) {
	// allow loading over existing data
	
	char *data;
	unsigned int size;
	
	if (!ReadInMem(filename, data, size)) {
		return 0;
	}
	
	// verify header
	if (memcmp(data, "Hantei6DataFile", 15)) {
		delete[] data;
		
		return 0;
	}

	//Names are utf8 because the file was saved by this tool.
	bool utf8 = ((unsigned char*)data)[31] == 0xFF;
	
	// initialize the root
	unsigned int *d = (unsigned int *)(data + 0x20);
	unsigned int *d_end = (unsigned int *)(data + size);
	if (memcmp(d, "_STR", 4)) {
		delete[] data;
		return 0;
	}

	test.filename = filename;
	
	unsigned int sequence_count = d[1];
	
	if(!patch)
		Free();
	
	if(sequence_count > m_nsequences)
		m_sequences.resize(sequence_count);
	m_nsequences = sequence_count;

	d += 2;	
	// parse and recursively store data
	d = fd_main_load(d, d_end, m_sequences, m_nsequences, utf8);
	
	// cleanup and finish
	delete[] data;
	
	m_loaded = 1;
	return 1;
}


#define VAL(X) ((const char*)&X)
#define PTR(X) ((const char*)X)

void FrameData::save(const char *filename)
{
	std::ofstream file(filename, std::ios_base::out | std::ios_base::binary);
	if (!file.is_open())
		return;
	
	for(auto& seq : m_sequences)
	for(auto &frame : seq.frames)
	for(auto it = frame.hitboxes.begin(); it != frame.hitboxes.end();)
	{
		Hitbox &box = it->second;
		//Delete degenerate boxes when exporting.
		if( (box.xy[0] == box.xy[2]) ||
			(box.xy[1] == box.xy[3]) )
		{
			frame.hitboxes.erase(it++);
		}
		else
		{
			//Fix inverted boxes. Don't know if needed.
			if(box.xy[0] > box.xy[2])
				std::swap(box.xy[0], box.xy[2]);
			if(box.xy[1] > box.xy[3])
				std::swap(box.xy[1], box.xy[3]);
			++it;
		}
	}

	char header[32] = "Hantei6DataFile";

	//Set special byte to know if the file was written by our tool
	header[31] = 0xFF; 

	file.write(header, sizeof(header));

	uint32_t size = get_sequence_count();
	file.write("_STR", 4); file.write(VAL(size), 4);

	for(uint32_t i = 0; i < get_sequence_count(); i++)
	{
		file.write("PSTR", 4); file.write(VAL(i), 4);
		WriteSequence(file, &m_sequences[i]);
		file.write("PEND", 4);
	}

	file.write("_END", 4);
	file.close();
}

void FrameData::saveChar(std::string filename)
{
	struct BoxSizes
	{
		int8_t greens;
		int8_t reds;
		int8_t collision;
	};

	struct CharFileHeader //header for .char files.
	{
		char signature[32];
		uint32_t version;
		uint16_t sequences_n;
	};

	struct Frame_property
	{
		int32_t spriteIndex = 0;
		int32_t duration = 0;
		int32_t jumpTo = 0;
		int32_t jumpType = 0;
		int32_t relativeJump = false;

		uint32_t flags = 0;
		int32_t vel[2] = {0}; // x,y
		int32_t accel[2] = {0};
		int32_t movementType[2] = {0}; //Add or set X,Y

		int16_t cancelType[2] = {};
		int32_t state = 0;
		
		float spriteOffset[2]; //x,y
		int16_t loopN;
		int16_t chType;
		float scale[2];
		float color[4];
		int32_t blendType = 0;
		float rotation[3]; //XYZ
	};

	struct Frame
	{
		Frame_property frameProp;
		//Attack_property attackProp;
		//Boxes are defined by BL, BR, TR, TL points, in that order.
		std::vector<int> greenboxes;
		std::vector<int> redboxes;
		std::vector<int> colbox;
	};

	struct seqProp
	{
		int level = 0;
		int landFrame = 0;
		int zOrder = 0;
	};

	struct SequenceCHAR
	{
		seqProp props;
		std::vector<Frame> frames;
		std::string name;
	};

	std::vector<SequenceCHAR> sequences;

	//next, fjump, check loop, sjump,
	int jumpTable[3] = {3,0,1}; 
	int stateTable[3] = {0,2,1}; 
	int cancelTable[4] = {0,2,1,3};

	for(int i = 0; i < m_nsequences; i++)
	{
		sequences.push_back({});
		auto &seq = m_sequences[i];
		auto &cseq = sequences[i];
		cseq.name = seq.name;
		cseq.props.level = seq.level;
		
		int counterType = 0;
		for(int j = 0; j < seq.frames.size(); j++)
		{
			
			cseq.frames.push_back({});
			auto &frame = seq.frames[j];
			auto &cframe = cseq.frames[j];
			
			if(frame.AF.landJump > 0)
				cseq.props.landFrame = frame.AF.landJump;
			
			
			if (frame.AS.counterType > 0)
				counterType = 1;
			if (frame.AS.counterType == 3)
				counterType = 0;

			auto &p = cframe.frameProp;
			
			p.spriteIndex = frame.AF.spriteId;
			p.duration = frame.AF.duration;
			p.jumpTo = frame.AF.jump;
			p.jumpType = jumpTable[frame.AF.aniType];
			if(frame.AF.aniFlag & 0x2)
				p.jumpType = 2;
			if(frame.AF.aniFlag & 0x4)
				p.relativeJump = true;
			p.loopN = frame.AF.loopCount;
			p.spriteOffset[0] = frame.AF.offset_x;
			p.spriteOffset[1] = frame.AF.offset_y;
			memcpy(p.scale, frame.AF.scale, sizeof(float)*2);
			memcpy(p.color, frame.AF.rgba, sizeof(float)*4);
			p.blendType = frame.AF.blend_mode-1; //1-indexed to 0-indexed
			if(p.blendType < 1)
				p.blendType = 0;
			memcpy(p.rotation, frame.AF.rotation, sizeof(float)*3);
			
			p.state = stateTable[frame.AS.stanceState];

			//0 Never. 1 Always. 2 Any hit. 3 Successful Hit.
			p.cancelType[0] = frame.AS.cancelNormal;
			p.cancelType[1] = frame.AS.cancelSpecial;
			p.chType = counterType;
			p.flags = frame.AS.canMove;
			p.flags |= (frame.AS.hitsNumber > 0) << 31;
			//p.MinCancelLvl = frame.AS.cancelNormal > 0;
			memcpy(p.accel, frame.AS.accel, sizeof(float)*2);
			memcpy(p.vel, frame.AS.speed, sizeof(float)*2);
			p.vel[1] = -p.vel[1];
			p.accel[1] = -p.accel[1];
			auto movFlags = frame.AS.movementFlags; 
			if(movFlags & 0x1)
				p.movementType[1] = 1; //keep; set-set; add-set; add-add
			if(movFlags & 0x2)
				p.movementType[1] = 2;
			if(movFlags & 0x10)
				p.movementType[0] = 1;
			if(movFlags & 0x20)
				p.movementType[0] = 2;
			
			int gC, rC;
			for(auto it = frame.hitboxes.begin(); it != frame.hitboxes.end(); it++)
			{
				Hitbox &box = it->second;
				box.xy[1] = -box.xy[1];
				box.xy[3] = -box.xy[3];
				if(it->first == 0)
				{
					cframe.colbox.resize(4);
					memcpy(cframe.colbox.data(), box.xy, sizeof(int)*4);
				}
				else if(it->first >= 1 && it->first <= 8)
				{
					gC = cframe.greenboxes.size();
					cframe.greenboxes.resize(gC+4);
					memcpy(cframe.greenboxes.data()+gC, box.xy, sizeof(int)*4);
				}
				else if(it->first >= 25)
				{
					rC = cframe.redboxes.size();
					cframe.redboxes.resize(rC+4);
					memcpy(cframe.redboxes.data()+rC, box.xy, sizeof(int)*4);
				}
			}
		}
	}
	
	std::ofstream file(filename, std::ios_base::out | std::ios_base::binary);
	if (!file.is_open())
		return;

	#define rv(X) ((char*)&X)
	#define rptr(X) ((char*)X)
	
	CharFileHeader header;
	header.sequences_n = sequences.size();
	header.version = 99'3;
	strncpy_s(header.signature, "AFGECharacterFile", 32);
	file.write(rv(header), sizeof(CharFileHeader));

	for (uint16_t i = 0; i < header.sequences_n; ++i)
	{
		auto &currSeq = sequences[i];
		uint8_t namelength = currSeq.name.size();
		file.write(rv(namelength), sizeof(namelength));
		file.write(rptr(currSeq.name.data()), namelength);

		file.write(rv(currSeq.props), sizeof(seqProp));

		uint8_t seqlength = currSeq.frames.size();
		file.write(rv(seqlength), sizeof(seqlength));
		for (uint8_t i2 = 0; i2 < seqlength; ++i2)
		{
			auto &currFrame = currSeq.frames[i2];

			//How many boxes are used per frame
			BoxSizes bs;
			bs.greens = currFrame.greenboxes.size();
			bs.reds = currFrame.redboxes.size();
			bs.collision = currFrame.colbox.size();
			file.write(rv(bs), sizeof(BoxSizes));

			file.write(rv(currFrame.frameProp), sizeof(Frame_property));

			file.write(rptr(currFrame.greenboxes.data()), sizeof(int) * bs.greens);
			file.write(rptr(currFrame.redboxes.data()), sizeof(int) * bs.reds);
			file.write(rptr(currFrame.colbox.data()), sizeof(int) * bs.collision);
		}
	}

	file.close();
	return;
}

void FrameData::Free() {
	m_sequences.clear();
	m_nsequences = 0;
	m_loaded = 0;
}

int FrameData::get_sequence_count() {
	if (!m_loaded) {
		return 0;
	}
	return m_nsequences;
}

Sequence* FrameData::get_sequence(int n) {
	if (!m_loaded) {
		return 0;
	}
	
	if (n < 0 || (unsigned int)n >= m_nsequences) {
		return 0;
	}
	
	return &m_sequences[n];
}

std::string FrameData::GetDecoratedName(int n)
{
		std::stringstream ss;
		ss.flags(std::ios_base::right);
		
		ss << std::setfill('0') << std::setw(3) << n << " ";
		
		if(!m_sequences[n].empty)
		{
			bool noFrames = m_sequences[n].frames.empty();
			if(noFrames)
				ss << u8"〇 ";
				
			if(m_sequences[n].name.empty() && m_sequences[n].codeName.empty() && !noFrames)
			{
					ss << u8"Untitled";
			}
		}

		ss << m_sequences[n].name;
		if(!m_sequences[n].codeName.empty())
			ss << " - " << m_sequences[n].codeName;
		return ss.str();
}

FrameData::FrameData() {
	m_nsequences = 0;
	m_loaded = 0;
}

FrameData::~FrameData() {
	Free();
}
