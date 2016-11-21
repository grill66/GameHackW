#pragma once

#define WARROIR_PLAIN_ATTACK	0x8B4D
#define WARRIOR_SKILL_1			0x8CDD
#define WARRIOR_SKILL_4			0xA321
#define WARRIOR_SKILL_6			0xA641

extern bool isHooked;
extern bool isMacroOn;
extern bool isGettingLoc;
extern bool battletime;
extern bool isEliminated;
extern bool setTarget;



// Attack Info
typedef struct MACROATTACKINFO {
	char Index[8];
	unsigned short AttackType;
	float X;
	float Y;
	float Z;
	short Dir;
} *PMACROATTACKINFO;



extern MACROATTACKINFO playerattackinfo;	// player battle information
extern MACROATTACKINFO targetinfo;			// target battle information


int PacketReplacer(unsigned char *data, int *length);
int EntityReplacer(WCHAR *str, WCHAR *target, WCHAR chr, int len);
int PlayMacro();