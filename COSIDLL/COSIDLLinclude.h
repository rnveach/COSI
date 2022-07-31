
#if !defined COSI_DLL_INCLUDE_H
#define COSI_DLL_INCLUDE_H 1

#define	LoadDLLFunction(lib, name)	\
	 DLL_##name = (##name) GetProcAddress((HMODULE)lib,###name)

typedef	size_t			(CALLBACK* OUT_FileRead)			(void *memory, unsigned long size1, unsigned long size2, FILE *infile);

typedef	char*			(CALLBACK* _Name)					(void);
typedef	unsigned long	(CALLBACK* _Version)				(void);
typedef	char*			(CALLBACK* _DecompressSector)		(FILE *modef, FILE *dataf, unsigned long sectorpos, bool redochecksum);
typedef	bool			(CALLBACK* _CompressSector)			(char* sector, char* modedata, char* sectordata, unsigned long &modesize, unsigned long &sectorsize, bool removetrailingzeros);
typedef	bool			(CALLBACK* CloseFiles)				(void);
typedef	bool			(CALLBACK* SetFiles)				(char* modefilen, char* datafilen);
typedef	char*			(CALLBACK* GetModeFileName)			(void);
typedef	char*			(CALLBACK* GetDataFileName)			(void);
typedef	char*			(CALLBACK* ReadFileSector)			(void);
typedef	bool			(CALLBACK* SeekFileSector)			(unsigned long sector);
typedef	bool			(CALLBACK* SeekFilePosition)		(unsigned long position);
typedef	unsigned long	(CALLBACK* GetFileCurrentPosition)	(void);
typedef	unsigned long	(CALLBACK* GetFileCurrentSector)	(void);
typedef	void			(CALLBACK* InitCOSI)				(void);
typedef	void			(CALLBACK* DeInitCOSI)				(void);
typedef	void			(CALLBACK* Set_FileRead)			(OUT_FileRead filefunc);

#define CreateDLLVariable(name)	\
	; ##name	DLL_##name ;

/*
	CreateDLLVariable(_Name);
	CreateDLLVariable(_Version);
	CreateDLLVariable(_DecompressSector);
	CreateDLLVariable(_CompressSector);
	CreateDLLVariable(CloseFiles);
	CreateDLLVariable(SetFiles);
	CreateDLLVariable(GetModeFileName);
	CreateDLLVariable(GetDataFileName);
	CreateDLLVariable(ReadFileSector);
	CreateDLLVariable(SeekFileSector);
	CreateDLLVariable(SeekFilePosition);
	CreateDLLVariable(GetFileCurrentPosition);
	CreateDLLVariable(GetFileCurrentSector);
	CreateDLLVariable(Set_FileRead);
*/

/*
	LoadDLLFunction(Lib, _Name);
	LoadDLLFunction(Lib, _Version);
	LoadDLLFunction(Lib, _DecompressSector);
	LoadDLLFunction(Lib, _CompressSector);
	LoadDLLFunction(Lib, CloseFiles);
	LoadDLLFunction(Lib, SetFiles);
	LoadDLLFunction(Lib, GetModeFileName);
	LoadDLLFunction(Lib, GetDataFileName);
	LoadDLLFunction(Lib, ReadFileSector);
	LoadDLLFunction(Lib, SeekFileSector);
	LoadDLLFunction(Lib, SeekFilePosition);
	LoadDLLFunction(Lib, GetFileCurrentPosition);
	LoadDLLFunction(Lib, GetFileCurrentSector);
	LoadDLLFunction(Lib, InitCOSI);
	LoadDLLFunction(Lib, DeInitCOSI);
	LoadDLLFunction(Lib, Set_FileRead);
*/

/*
size_t CALLBACK MAIN_FileRead(void *memory, unsigned long size1, unsigned long size2, FILE *infile) {
	return fread(memory, size1, size2, infile);
}
*/

#endif