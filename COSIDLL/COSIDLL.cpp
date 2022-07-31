// COSIDLL.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"

//#include <FL/FL.h>
//#include <FL/Fl_File_Chooser.H>
#include <stdlib.h>
#include <stdio.h>

#include "yazedc.h"
#include "COSIDLL.h"

BOOL APIENTRY DllMain(HANDLE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	return TRUE;
}

/////////////////////////////////////////////////////////////////
// My DLL Stuff
/////////////////////////////////////////////////////////////////

char* CALLBACK _Name(void) {
	return "rveach's COSI DLL Plugin";
}

unsigned long CALLBACK _Version(void) {
	return version<<16|revision<<8|build;
}

size_t CALLBACK DLLs_FileRead(void *memory, unsigned long size1, unsigned long size2, FILE *infile) {
	return fread(memory, size1, size2, infile);
}

void CALLBACK Set_FileRead(OUT_FileRead filefunc) {
	OUTSIDE_FileRead = filefunc;
}

char* CALLBACK _DecompressSector(FILE *modef, FILE *dataf, unsigned long sectorpos, bool redochecksum) {
	memset(returnsector, 0, 2352);
	newsize = 0;

	OUTSIDE_FileRead(&data, 1, 1, modef);

	switch (data & 7) {
	case AUDIO_SECTOR:
		if (data & 32)		OUTSIDE_FileRead(&newsize, 1, 2, modef);
		else				newsize = 2352;

		OUTSIDE_FileRead(returnsector, 1, newsize, dataf);
		
	case MODE_0:
		return returnsector;
		break;
	}

	memset(returnsector + 1, 0xFF, 10);

	returnsector[14] = to_BCD((unsigned char)(sectorpos % 75));			//m
	returnsector[13] = to_BCD((unsigned char)((sectorpos / 75) % 60));	//s
	returnsector[12] = to_BCD((unsigned char)((sectorpos / 75) / 60));	//f

	returnsector[15] = ((data & 7) >= 2 ? 2 : (data & 7));

	switch (data & 7) {
	case MODE_1:
		if (data & 32)		OUTSIDE_FileRead(&newsize, 1, 2, modef);
		else				newsize = 2048;

		OUTSIDE_FileRead(returnsector + 16, 1, newsize, dataf);

		if (redochecksum) {
			if ((data & 24) != 24)
				do_encode_L2((unsigned char*)returnsector, data & 7);
		}

		memset(returnsector + 2068, 0, 8);
		if (data & 8)
			OUTSIDE_FileRead(returnsector + 2064, 1, 4, modef);
		if (data & 16)
			OUTSIDE_FileRead(returnsector + 2076, 1, 276, modef);
		break;
	case MODE_2:
		if (data & 32)		OUTSIDE_FileRead(&newsize, 1, 2, modef);
		else				newsize = 2336;
			
		OUTSIDE_FileRead(returnsector + 16, 1, newsize, dataf);
		break;
	case MODE_2_FORM_1:
		if (data & 32)		OUTSIDE_FileRead(&newsize, 1, 2, modef);
		else				newsize = 2048;
			
		OUTSIDE_FileRead(returnsector + 24, 1, newsize, dataf);

		OUTSIDE_FileRead(returnsector + 16, 1, 4, modef);
		memcpy(returnsector + 20, returnsector + 16, 4);

		if (redochecksum) {
			if ((data & 24) != 24)
				do_encode_L2((unsigned char*)returnsector, data & 7);
		}

		if (data & 8)
			OUTSIDE_FileRead(returnsector + 2072, 1, 4, modef);
		if (data & 16)
			OUTSIDE_FileRead(returnsector + 2076, 1, 276, modef);
		break;
	case MODE_2_FORM_2:
		if (data & 32)		OUTSIDE_FileRead(&newsize, 1, 2, modef);
		else				newsize = 2324;
			
		OUTSIDE_FileRead(returnsector + 24, 1, newsize, dataf);

		OUTSIDE_FileRead(returnsector + 16, 1, 4, modef);
		memcpy(returnsector + 20, returnsector + 16, 4);

		if (redochecksum) {
			if (!(data & 8))
				do_encode_L2((unsigned char*)returnsector, data & 7);
		}

		if (data & 8)
			OUTSIDE_FileRead(returnsector + 2348, 1, 4, modef);
		break;
	default:
		return NULL;
	}

	returnsector[14] = to_BCD((unsigned char)(sectorpos % 75));			//m
	returnsector[13] = to_BCD((unsigned char)((sectorpos / 75) % 60));	//s
	returnsector[12] = to_BCD((unsigned char)((sectorpos / 75) / 60));	//f

	returnsector[15] = ((data & 7) >= 2 ? 2 : (data & 7));

	return returnsector;
}

bool CALLBACK _CompressSector(char* sector, char* modedata, char* sectordata, unsigned long &modesize, unsigned long &sectorsize, bool removetrailingzeros) {
	unsigned long start;

	modesize = 1;
	sectorsize = 0;

	if (memcmp(sector, "\x00\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\x00", 12) == 0) {
		if (sector[15] == 1) {
			modedata[0] = MODE_1;
			start = 16;
			sectorsize = 2048;
		} else if (sector[15] == 2) {
			if (memcmp(sector + 16, sector + 20, 4) == 0) {
				if (sector[18] & 0x20) {
					modedata[0] = MODE_2_FORM_2;
					sectorsize = 2324;
				} else {
					modedata[0] = MODE_2_FORM_1;
					sectorsize = 2048;
				}

				start = 24;
			} else {
				modedata[0] = MODE_2;
				start = 16;
				sectorsize = 2336;
			}
		} else {
			return false;
		}
	} else {
		start = 0;

		a = 0;
		while (a < 2352) {
			if (sector[a] != 0) break;
			a++;
		}

		if (a == 2352) {
			modedata[0] = MODE_0;
			sectorsize = 0;
		} else {
			modedata[0] = AUDIO_SECTOR;
			sectorsize = 2352;
		}
	}

	if (modedata[0] == MODE_0) return true;

	newsize = (short)sectorsize;
	if (removetrailingzeros) {
		while (newsize) {
			if (sector[start + newsize - 1] == 0) newsize--;
			else break;
		}

		if (newsize != sectorsize) {
			modedata[0] |= 32;
			sectorsize = newsize;
			*(unsigned short*)(modedata + modesize) = newsize;

			modesize += 2;
		}
	}

	memcpy(sectordata, sector + start, sectorsize);

	switch (modedata[0] & 7) {
	case MODE_1:
		memcpy(copysector, sector, 2352);
		do_encode_L2((unsigned char*)copysector, modedata[0] & 7);

		if (memcmp(copysector + 2064, sector + 2064, 4) != 0) {
			modedata[0] |= 8;
			memcpy(modedata + modesize, sector + 2064, 4);
			modesize += 4;
		}
		if (memcmp(copysector + 2076, sector + 2076, 276) != 0) {
			modedata[0] |= 16;
			memcpy(modedata + modesize, sector + 2076, 276);
			modesize += 276;
		}
		break;
	case MODE_2_FORM_1:
		memcpy(copysector, sector, 2352);
		do_encode_L2((unsigned char*)copysector, modedata[0] & 7);

		memcpy(modedata + modesize, sector + 16, 4);
		modesize += 4;

		if (memcmp(copysector + 2072, sector + 2072, 4) != 0) {
			modedata[0] |= 8;
			memcpy(modedata + modesize, sector + 2072, 4);
			modesize += 4;
		}
		if (memcmp(copysector + 2076, sector + 2076, 276) != 0) {
			modedata[0] |= 16;
			memcpy(modedata + modesize, sector + 2076, 276);
			modesize += 276;
		}
		break;
	case MODE_2_FORM_2:
		memcpy(copysector, sector, 2352);
		do_encode_L2((unsigned char*)copysector, modedata[0] & 7);

		memcpy(modedata + modesize, sector + 16, 4);
		modesize += 4;

		if (memcmp(copysector + 2348, sector + 2348, 4) != 0) {
			modedata[0] |= 8;
			memcpy(modedata + modesize, sector + 2348, 4);
			modesize += 4;
		}
		break;
	}

	return true;
}

bool CALLBACK CloseFiles() {
	memset(modefilename, 0, MAX_PATH);
	memset(datafilename, 0, MAX_PATH);
	sectorpos = 0;

	if (filestatus != 0) {
		if (filestatus & 1)
			fclose(modefile);
		if (filestatus & 2)
			fclose(datafile);
	}
	return true;
}

bool CALLBACK SetFilesShort(char* filen) {
	if (filestatus != 0) {
		CloseFiles();
	}

	strncpy(modefilename, filen, MAX_PATH);
	strcat(modefilename, ".mode");
	strncpy(datafilename, filen, MAX_PATH);
	strcat(datafilename, ".data");

	if (modefile = fopen(modefilename, "rb")) {
		filestatus |= 1;
		
		if (datafile = fopen(datafilename, "rb"))
			filestatus |= 2;
	}

	if ((filestatus & 3) != 3) {
		CloseFiles();
		return false;
	}

	BuildCOSIList();

	fseek(modefile, 0, SEEK_SET);
	fseek(datafile, 0, SEEK_SET);

	return true;
}

bool CALLBACK SetFiles(char* modefilen, char* datafilen) {
	if (filestatus != 0) {
		CloseFiles();
	}

	strncpy(modefilename, modefilen, MAX_PATH);
	strncpy(datafilename, datafilen, MAX_PATH);

	if (modefile = fopen(modefilename, "rb")) {
		filestatus |= 1;
		
		if (datafile = fopen(datafilename, "rb"))
			filestatus |= 2;
	}

	if ((filestatus & 3) != 3) {
		CloseFiles();
		return false;
	}

	BuildCOSIList();

	fseek(modefile, 0, SEEK_SET);
	fseek(datafile, 0, SEEK_SET);

	return true;
}

char* CALLBACK GetModeFileName() {
	return modefilename;
}

char* CALLBACK GetDataFileName() {
	return datafilename;
}

char* CALLBACK ReadFileSector() {
	if (filestatus == 0) return NULL;
	
	if (feof(modefile)) return NULL;
	if (feof(datafile)) return NULL;

	sectorpos++;
	return _DecompressSector(modefile, datafile, (sectorpos + 149), true);
}

bool CALLBACK SkipReadFileSector() {
	if (filestatus == 0) return false;

	if (feof(modefile)) return false;
	if (feof(datafile)) return false;

	sectorpos++;

	fread(&data, 1, 1, modefile);
	if (data & 32) {
		fread(&newsize, 1, 2, modefile);
	}

	switch (data & 7) {
	case MODE_0:
		break;
	case MODE_1:
		if (data & 32)		fseek(datafile, newsize, SEEK_CUR);
		else				fseek(datafile, 2048, SEEK_CUR);
		break;
	case MODE_2:
		if (data & 32)		fseek(datafile, newsize, SEEK_CUR);
		else				fseek(datafile, 2236, SEEK_CUR);
		break;
	case MODE_2_FORM_1:
		if (data & 32)		fseek(datafile, newsize, SEEK_CUR);
		else				fseek(datafile, 2048, SEEK_CUR);
		fseek(modefile, 4, SEEK_CUR);
	break;
	case MODE_2_FORM_2:
		if (data & 32)		fseek(datafile, newsize, SEEK_CUR);
		else				fseek(datafile, 2324, SEEK_CUR);
		fseek(modefile, 4, SEEK_CUR);
		break;
	case AUDIO_SECTOR:
		if (data & 32)		fseek(datafile, newsize, SEEK_CUR);
		else				fseek(datafile, 2352, SEEK_CUR);
		break;
	}

	if (data & 8)		fseek(modefile, 4, SEEK_CUR);
	if (data & 16)		fseek(modefile, 276, SEEK_CUR);

	return true;
}

bool CALLBACK SeekFileSector(unsigned long sector) {
	if (filestatus == 0) return false;

	if (sector != sectorpos) {
		a = sector / 5;
		if (a >= listsize) a = listsize;
		sectorpos = a*5;

		if (a == 0) {
			fseek(modefile, 0, SEEK_SET);
			fseek(datafile, 0, SEEK_SET);
		} else {
			fseek(modefile, cosipositions[a].modfpos, SEEK_SET);
			fseek(datafile, cosipositions[a].datfpos, SEEK_SET);
		}

		while (sectorpos != sector) {
			if (!SkipReadFileSector()) break;
		}
	}

	return true;
}

bool CALLBACK SeekFilePosition(unsigned long position) {
	return SeekFileSector(position / 2352);
}

unsigned long CALLBACK GetFileCurrentPosition() {
	return sectorpos * 2352;
}

unsigned long CALLBACK GetFileCurrentSector() {
	return sectorpos;
}

void CALLBACK InitCOSI() {
	OUTSIDE_FileRead = DLLs_FileRead;
	modefile = NULL;
	datafile = NULL;
	filestatus = 0;
	CloseFiles();

	memset(returnsector, 0, 2352);
	memset(returnmode, 0, 2352);
	memset(returndata, 0, 2352);
}

void CALLBACK DeInitCOSI() {
	CloseFiles();
}

void BuildCOSIList() {
	//a=predfilepos, b=modpos, c=sectorpos
	a = b = c = 0;
	listsize = 0;

	fseek(modefile, 0, SEEK_SET);

	while (!feof(modefile)) {
		if (!(c % 5)) {
			cosipositions[listsize].datfpos = a;
			cosipositions[listsize].modfpos = b;

			listsize++;
		}

		if (fread(&data, 1, 1, modefile) != 1)
			break;

		b++;
		c++;

		newsize = 0;
		if (data & 32) {
			if (fread(&newsize, 1, 2, modefile) != 2)
				break;

			b += 2;
		}

		switch (data & 7) {
		case MODE_0:
			break;
		case MODE_1:
			if (data & 32)		a += newsize;
			else				a += 2048;
			break;
		case MODE_2:
			if (data & 32)		a += newsize;
			else				a += 2336;
			break;
		case MODE_2_FORM_1:
			if (data & 32)		a += newsize;
			else				a += 2048;
			b += 4;
			break;
		case MODE_2_FORM_2:
			if (data & 32)		a += newsize;
			else				a += 2324;
			b += 4;
			break;
		case AUDIO_SECTOR:
			if (data & 32)		a += newsize;
			else				a += 2352;
			break;
		}

		if (data & 8)		b += 4;
		if (data & 16)		b += 276;

		if (fseek(modefile, b, SEEK_SET))
			break;
	}
}

/////////////////////////////////////////////////////////////////
// PSX Plugin DLL Stuff
/////////////////////////////////////////////////////////////////

//cdtype
//	0		BIN/IMG
//	1		COSI
//	2		ZIP
//	3		ZIPed COSI

char * CALLBACK PSEgetLibName(void) {
	return _Name();
}

unsigned long CALLBACK PSEgetLibType(void) {
	return  PSE_LT_CDR;
}

unsigned long CALLBACK PSEgetLibVersion(void) {
	return _Version();
}

// init: called once at library load

long CALLBACK CDRinit(void) {
	getProgramName(cdfilename);

	oldfilename[0] = cdfilename[0] = 0;
	cdtype = 0;

	fileopen = false;
	subfile = file = NULL;

	InitCOSI();

	oldfilename[0] = cdfilename[0] = 0;

	/*ZeroMemory(&ofn,sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.hInstance = NULL;
	ofn.lpstrCustomFilter = NULL;
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
	ofn.lpstrDefExt = NULL;
	ofn.lCustData = 0;
	ofn.lpfnHook = NULL;
	ofn.lpTemplateName = NULL;

	ofn.lpstrFile = cdfilename;
	ofn.lpstrInitialDir = NULL;
	ofn.lpstrTitle = "Load CD Image";
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(cdfilename);

	ofn.lpstrFilter = "All Acceptable Files\0*.img;*.bin;*.mode;*.cosi\0All Files\0*.*\0\0";*/

	return 0;
}

// shutdown: called once at final exit

long CALLBACK CDRshutdown(void) {
	DeInitCOSI();

	if (fileopen) {
		if (cdtype == 0) {
			fclose(file);
		}

		fileopen = false;
	}

	return 0;
}

// open: called, when games starts/cd has been changed

//Fl_File_Chooser		*fc;

long CALLBACK CDRopen(void) {
	unsigned long a;

	memset(sector, 0, 2352);

	cdfilename[0] = 0;

	//causes heap errors
	/*if (!GetOpenFileName(&ofn))
		return PSE_CDR_ERR_SUCCESS;*/

	//strcpy(cdfilename, "C:\\Documents and Settings\\048723\\Desktop\\Documents\\PSX\\ROMs\\cotton\\cotton.bin");

	//strcpy(cdfilename, fl_file_chooser("Load CD Image", "Common image files (*.{bin,img,mode})", oldfilename, 1));
	//fc = new Fl_File_Chooser(".", "Common image files (*.{bin,img,mode})", Fl_File_Chooser::SINGLE, "Load CD Image");

	//#ifdef __LINUX__
	//Fl::wait();
	//#endif

	if ((!strcmp(cdfilename, oldfilename)) && (fileopen)) {
		/////////same file opened again
	} else {
		if (fileopen) {
			if (cdtype == 1) {
				CloseFiles();
			} else if (cdtype == 0) {
				fclose(file);
			}
		}

		strcpy(oldfilename, cdfilename);

		lastaccessedsector = 0;
		memset(lastreadtime, 0, 3);

		cdinfo.sectorsize = 2352;
		cdinfo.roffset = 0;

		/////////Get file type
		b = a = 0;
		do {
			a = b+1;
			chr = strchr(cdfilename + a, '.');
			b = chr-cdfilename;
		} while (chr != NULL);

		if (!strcmp(cdfilename+a, "mode")) {
			/////////COSI
			cdtype = 1;
			
			cdfilename[a-1] = 0;
			if (!SetFilesShort(cdfilename)) {
				cdfilename[a-1] = '.';
				return PSE_CDR_ERR_SUCCESS;
			}
			cdfilename[a-1] = '.';
		} else {
			file = fopen(cdfilename, "rb");

			if (!file)	return PSE_CDR_ERR_SUCCESS;

			if (!strcmp(cdfilename+a, "zip")) {
				/////////ZIP
				cdtype = 2;
			} else {
				cdtype = 0;
			}
		}

		//FIX: open sub file
		//FIX: open cue file
	}

	fileopen = true;

	return PSE_CDR_ERR_SUCCESS;
}

// close: called when emulation stops

long CALLBACK CDRclose(void) {
	if (cdtype == 1) {
				CloseFiles();
			} else if (cdtype == 0) {
				fclose(file);
			}
	return 0;
}

// test: always fine

long CALLBACK CDRtest(void) {
	return PSE_CDR_ERR_SUCCESS;
}            

// gettn: first/last track num

//FIX: ???
long CALLBACK CDRgetTN(unsigned char *buffer) {
	if (!fileopen) return -1;

	buffer[0] = 1;
	buffer[1] = 0;	//max number of tracks

	return 0;
}

// gettd: track addr

long CALLBACK CDRgetTD(unsigned char track, unsigned char *buffer) {
	if (!fileopen) return -1;
	unsigned char b = track;

	//if (not epsxe or pcsx*)
		//convert b from bcd to int

	//if ((b) && (cdinfo.tracks.size() <= b)) {
		//get time from cue list
	//} else {
		//get last track's time
	//}

	//if (not epsxe or pcsx*)
		//convert buffer from msf to fsm

	buffer[0] = 0;
	buffer[1] = 2;
	buffer[2] = 0;

	b=buffer[0];
	buffer[0]=buffer[2];
	buffer[2]=b;

	//convert the times to bcd

	return 0;
}

// readtrack: start reading at given address

long CALLBACK CDRreadTrack(unsigned char *timein) {
	if (!fileopen) return -1;

	unsigned long addr = time2addrB(timein);
	//set cdda play to false

	if (addr == lastaccessedsector)
		return PSE_CDR_ERR_SUCCESS;

	if (cdtype == 1) {
		if (addr != lastaccessedsector+1)
			SeekFileSector(addr);

		memcpy(sector, ReadFileSector(), 2352);
	} else if (cdtype == 2) {
		/////////ZIP file
	} else if (cdtype == 3) {
		/////////ZIPed COSI
	} else if (cdtype == 0) {
		if (addr != lastaccessedsector+1)
			fseek(file, addr*cdinfo.sectorsize, SEEK_SET);

		if (fread(sector + cdinfo.roffset, 1, cdinfo.sectorsize, file) != cdinfo.sectorsize)
			return PSE_CDR_ERR_NOREAD;
	}

	lastaccessedsector = addr;
	memcpy(lastreadtime, timein, 3);

	return PSE_CDR_ERR_SUCCESS;
}

// getbuffer: will be called after readtrack, to get ptr 
//            to data

unsigned char * CALLBACK CDRgetBuffer(void) {
	return (unsigned char*)(sector + 12);
}

// getsubbuffer: will also be called after readtrack, to 
//               get ptr to subdata, or NULL
           
unsigned char * CALLBACK CDRgetBufferSub(void) {
	if (subfile) {
		fseek(subfile, lastaccessedsector * 96, SEEK_SET);
		fread(sub, 1, 96, subfile);
		return sub;
	} else
		return NULL;
}

// audioplay: PLAYSECTOR is NOT BCD coded !!!

long CALLBACK CDRplay(unsigned char * sector) {
	//return theCD->playTrack(CDTime(sector, msfint));
	return PSE_CDR_ERR_SUCCESS;
}

// audiostop: stops cdda playing

long CALLBACK CDRstop(void) {
	//return theCD->stopTrack();
	return PSE_CDR_ERR_SUCCESS;
}

// getdriveletter

char CALLBACK CDRgetDriveLetter(void) {
	return 0;
}

// configure: shows config window

long CALLBACK CDRconfigure(void) {
	return PSE_CDR_ERR_SUCCESS;
}

// about: shows about window

long CALLBACK CDRabout(void) {
	//DialogBox(hInst,MAKEINTRESOURCE(IDD_ABOUT), GetActiveWindow(),(DLGPROC)AboutDlgProc);
	return PSE_CDR_ERR_SUCCESS;
}

// getstatus: pcsx func... poorly supported here
//            problem is: func will be called often, which
//            would block all of my cdr reading if I would use
//            lotsa scsi commands

struct CdrStat 
{
 unsigned long Type;
 unsigned long Status;
 unsigned char Time[3]; // current playing time
};

struct CdrStat ostat;

// reads cdr status
// type:
// 0x00 - unknown
// 0x01 - data
// 0x02 - audio
// 0xff - no cdrom
// status:
// 0x00 - unknown
// 0x02 - error
// 0x08 - seek error
// 0x10 - shell open
// 0x20 - reading
// 0x40 - seeking
// 0x80 - playing
// time:
// byte 0 - minute
// byte 1 - second
// byte 2 - frame


long CALLBACK CDRgetStatus(struct CdrStat *stat) {
	if (!fileopen) return -1;

	//memset(stat, 0, sizeof(struct CdrStat));

	//if (theCD->isPlaying()) {
		//stat->Type = 0x02;
	    //stat->Status = 0x80;
	//} else {
		stat->Type = 1;
		stat->Status = 0x20;
	//}

	stat->Time[0] = lastreadtime[0];
	stat->Time[1] = lastreadtime[1];
	stat->Time[2] = lastreadtime[2];
	return 0;
}