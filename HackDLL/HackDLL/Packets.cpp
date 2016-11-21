#include "stdafx.h"
#include "Packets.h"
#include "HackDLL.h"

#include "RecvHook.h"
#include "ChatHook.h"
#include "PlainPacketHook.h"

bool isHooked = false;
bool isMacroOn = false;
bool isGettingLoc = FALSE;
bool battletime = FALSE;
bool isEliminated = TRUE;
bool setTarget = FALSE;



MACROATTACKINFO playerattackinfo = { 0, };
MACROATTACKINFO targetinfo = { 0, };

HWND GameWindow = GetGameWindow();
BYTE **packets;
int curSet = 0;
int totalSet = 0;

UINT MappedKey_Return = MapVirtualKeyW(VK_RETURN, MAPVK_VK_TO_VSC);
UINT MappedKey_A = MapVirtualKeyW(0x41, MAPVK_VK_TO_VSC);
UINT MappedKey_Space = MapVirtualKeyW(VK_SPACE, MAPVK_VK_TO_VSC);


int PacketReplacer(unsigned char *data, int *length) {
	if (curSet >= totalSet) {
		curSet = 0;
		totalSet = 0;

		FILE *fp = fopen(filename, "r");
		fscanf(fp, "%d", &totalSet);

		printf("[*] totalSet : %d..\n", totalSet);
		packets = new PBYTE[totalSet];

		for (int i = 0; i < totalSet; ++i) {
			BYTE size = 0;
			fscanf(fp, "%02X", &size);

			PBYTE pca = packets[i] = new BYTE[size];
			pca[0] = size;

			for (int j = 1; j < size; ++j) {
				fscanf(fp, "%02X", &pca[j]);
			}
		}

		fclose(fp);
	}

	BYTE len = packets[curSet][0];
	printf("[%d] CurSet (length : %d) Replacing..\n", curSet, len);

	memcpy(data, packets[curSet], len);
	*length = len;

	++curSet;
	if (curSet >= totalSet) {
		isHooked = false;
	}

	return len;
}

int EntityReplacer(WCHAR *str, WCHAR *target, WCHAR chr, int len) {
	int tarlen = wcslen(target);
	WCHAR *tmp = wcsstr((WCHAR *)str, target);

	while (tmp) {
		*tmp = chr;
		wcscpy(tmp + 1, tmp + tarlen);

		len -= tarlen - 1;

		tmp = wcsstr(tmp + 1, target);
	}

	return len;
}


int TriggerPacket() { // Trigger Building and Sending Packet.

	PostMessageW(GameWindow, WM_KEYDOWN, VK_RETURN, MappedKey_Return);
	PostMessageW(GameWindow, WM_KEYUP, VK_RETURN, MappedKey_Return);
	Sleep(1000);

	PostMessageW(GameWindow, WM_KEYDOWN, VK_SPACE, MappedKey_Space);
	PostMessageW(GameWindow, WM_KEYUP, VK_SPACE, MappedKey_Space);
	Sleep(1000);

	PostMessageW(GameWindow, WM_KEYDOWN, VK_RETURN, MappedKey_Return);
	PostMessageW(GameWindow, WM_KEYUP, VK_RETURN, MappedKey_Return);
	Sleep(1000);

	PostMessageW(GameWindow, WM_KEYDOWN, VK_SPACE, MappedKey_Space);
	PostMessageW(GameWindow, WM_KEYUP, VK_SPACE, MappedKey_Space);

	return 0;
}

int PlayMacro() {
	FILE * fp;
	char buf[128] = { 0, };
	int i = 0;


	SetForegroundWindow(GameWindow);

	// Battle Cycle Starts...
	
	// Wait untill get the target
	while (setTarget == FALSE)
		Sleep(300);


	// Teleport character to location adjacent to TARGET
	sprintf(filename, "D:\\teleport.txt");
	TriggerPacket();

	printf("[ battletime : %d ]\n", battletime);

	while (battletime == FALSE) {	// Sleep untill target attack info is captured...
		Sleep(1000);
	}

	printf("[ break while... ]\n");
	
	isEliminated = FALSE;
	
	memcpy(buf, "\x2E\x00\x0B\x52", 4);
	memcpy(buf + 4, "\xDD\x8C\x0F\x00", 4);
	memcpy(buf + 8, (char *)&playerattackinfo.X, 4);
	memcpy(buf + 12, (char *)&playerattackinfo.Y, 4);
	memcpy(buf + 16, (char *)&playerattackinfo.Z, 4);
	memcpy(buf + 20, (char *)&playerattackinfo.Dir, 2);
	memset(buf + 22, 0, 24);

	
	fp = fopen("D:\\attack.txt", "w+");
	fprintf(fp, "1\n");

	for (i = 0; i < 46; i++)
		fprintf(fp, "%02X ", *((unsigned char *)&buf[i]));

	fclose(fp);
	
	sprintf(filename, "D:\\attack.txt");

	while (isEliminated == FALSE) {
		Sleep(1000);
		TriggerPacket();
	}

	printf("[ Battle Finished... ]\n");
	/*
	while (isEliminated == FALSE) {	
		
	}*/


	isMacroOn = FALSE;// 무한루프를 위해서는 지워야됨

	battletime = FALSE;
	isEliminated = TRUE;
	setTarget = FALSE;
	
	printf("[ Macro Finished... tid : %d]\n", GetCurrentThreadId());

	return 0;
}