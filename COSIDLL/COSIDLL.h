/////////////////////////////////////////////////////////////////

#define btoi(b)      ((b)/16*10 + (b)%16)

unsigned char to_BCD(unsigned char x) {
	return (((x / 10) << 4) | (x % 10));
}

unsigned long time2addrB(unsigned char *time) {
	unsigned long addr;

	addr = btoi(time[0])*60;
	addr = (addr + btoi(time[1]))*75;
	addr += btoi(time[2]);
	addr -= 150;
	return addr;
}

void getProgramName(char* toReturn) {
	unsigned long a, b;
	char* chr;
#ifdef _WINDOWS
	if (GetModuleFileName(NULL, toReturn, 1024) == 0) {
		strcpy(toReturn, "epsxe");
	}
#else
	toReturn = getenv("_");
	if (toReturn == NULL) {
		strcpy(roReturn, "pcsx");
	}
#endif

	b = a = 0;
	do {
		a = b+1;
		chr = strchr(toReturn + a, '\\');
		b = chr-toReturn;
	} while (chr != NULL);

	b = strchr(toReturn + a, '.') - toReturn;
	toReturn[b] = 0;
	memcpy(toReturn, toReturn + a, strlen(toReturn + a)+1);

	a = 0;
	while (toReturn[a]) {
		toReturn[a] = tolower(toReturn[a]);
		a++;
	}
}

/////////////////////////////////////////////////////////////////
// My DLL Stuff
/////////////////////////////////////////////////////////////////

#define PSE_LT_CDR					1
#define PSE_LT_GPU					2
#define PSE_LT_SPU					4
#define PSE_LT_PAD					8

#define PSE_CDR_ERR_SUCCESS			0
#define PSE_CDR_ERR					-40
#define PSE_CDR_ERR_NOREAD			PSE_CDR_ERR - 1

const unsigned long version  =  1;
const unsigned long revision =  1;
const unsigned long build    =  0;

#define MODE_0			0
#define MODE_1			1
#define MODE_2			2
#define MODE_2_FORM_0	2
#define MODE_2_FORM_1	3
#define MODE_2_FORM_2	4
#define AUDIO_SECTOR	5


//
//	0 - File Close
//	1 - Mode File Open
//	2 - Data File Open
//
int filestatus;

unsigned long sectorpos;
FILE *modefile, *datafile;

char modefilename[MAX_PATH], datafilename[MAX_PATH];
char returnsector[2352], copysector[2352];
char returnmode[2352], returndata[2352];

struct _cosipos {
	unsigned long modfpos, datfpos;
};

_cosipos cosipositions[0xFFFF];
unsigned long listsize;


unsigned long a, b, c;
unsigned short newsize;
unsigned char data;

typedef	size_t			(CALLBACK* OUT_FileRead)			(void *memory, unsigned long size1, unsigned long size2, FILE *infile);

OUT_FileRead	OUTSIDE_FileRead;

void BuildCOSIList();

/////////////////////////////////////////////////////////////////
// PSX Plugin DLL Stuff
/////////////////////////////////////////////////////////////////

bool fileopen;
char cdfilename[400], oldfilename[400], sector[2352], *chr;
unsigned char sub[96], status, lastreadtime[3], cdtype;
unsigned long lastaccessedsector;

//OPENFILENAME ofn;

FILE *file, *subfile;

struct _tracks {
	unsigned long pregaptime, starttime;
};

struct _cdinfo {
	//std::vector<_tracks> tracks;
	unsigned short sectorsize, roffset;
};

_cdinfo cdinfo;