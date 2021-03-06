/*---------------------------------------------------------------------------/
/  Petit FatFs - FAT file system module include file  R0.02    (C)ChaN, 2009
/----------------------------------------------------------------------------/
/ Petit FatFs module is an open source software to implement FAT file system to
/ small embedded systems. This is a free software and is opened for education,
/ research and commercial developments under license policy of following trems.
/
/  Copyright (C) 2009, ChaN, all right reserved.
/
/ * The Petit FatFs module is a free software and there is NO WARRANTY.
/ * No restriction on use. You can use, modify and redistribute it for
/   personal, non-profit or commercial use UNDER YOUR RESPONSIBILITY.
/ * Redistributions of source code must retain the above copyright notice.
/
/----------------------------------------------------------------------------*/

#include "integer.h"

/*---------------------------------------------------------------------------/
/ Petit FatFs Configuration Options
/
/ CAUTION! Do not forget to make clean the project after any changes to
/ the configuration options.
/
/----------------------------------------------------------------------------*/
#ifndef _FATFS
#define _FATFS

namespace PFFS {


#ifdef _USE_READ
#undef _USE_READ
#endif

#define    _USE_READ    1    /* pf_read(): 0:Remove ,1:Enable */

#ifdef _USE_DIR
#undef _USE_DIR
#endif

#define    _USE_DIR    1    /* pf_opendir() and pf_readdir(): 0:Remove ,1:Enable */

#ifdef _USE_LSEEK
#undef _USE_LSEEK
#endif

#define    _USE_LSEEK    1    /* pf_lseek(): 0:Remove ,1:Enable */

#ifdef _USE_WRITE
#undef _USE_WRITE
#endif

#define    _USE_WRITE    1    /* pf_write(): 0:Remove ,1:Enable */

#ifdef _FS_FAT32
#undef _FS_FAT32
#endif

#define _FS_FAT32    1    /* 0:Supports FAT12/16 only, 1:Enable FAT32 supprt */


#ifdef _CODE_PAGE
#undef _CODE_PAGE
#endif

#define    _CODE_PAGE    1
/* Defines which code page is used for path name. Supported code pages are:
/  932, 936, 949, 950, 437, 720, 737, 775, 850, 852, 855, 857, 858, 862, 866,
/  874, 1250, 1251, 1252, 1253, 1254, 1255, 1257, 1258 and 1 (ASCII only).
/  SBCS configurations except for 1 requiers a case conversion table. This
/  might occupy 128 bytes on the RAM on some platforms, e.g. avr-gcc. */


#ifdef _WORD_ACCESS
#undef _WORD_ACCESS
#endif

#define _WORD_ACCESS    0 //jonne 1 gives hard fault with lpc11u68
/* The _WORD_ACCESS option defines which access method is used to the word
/  data in the FAT structure.
/
/   0: Byte-by-byte access. Always compatible with all platforms.
/   1: Word access. Do not choose this unless following condition is met.
/
/  When the byte order on the memory is big-endian or address miss-aligned
/  word access results incorrect behavior, the _WORD_ACCESS must be set to 0.
/  If it is not the case, the value can also be set to 1 to improve the
/  performance and code efficiency. */


/* End of configuration options. Do not change followings without care.     */
/*--------------------------------------------------------------------------*/

#if _FS_FAT32
#ifdef CLUST
#undef CLUST
#endif

#define    CLUST    DWORD
#else
#ifdef CLUST
#undef CLUST
#endif

#define    CLUST    WORD
#endif


/* File system object structure */

typedef struct _FATFS_ {
    BYTE    fs_type;    /* FAT sub type */
    BYTE    csize;        /* Number of sectors per cluster */
    BYTE    flag;        /* File status flags */
    BYTE    csect;        /* File sector address in the cluster */
    WORD    n_rootdir;    /* Number of root directory entries (0 on FAT32) */
    BYTE*    buf;        /* Pointer to the disk access buffer */
    CLUST    max_clust;    /* Maximum cluster# + 1. Number of clusters is max_clust - 2 */
    DWORD    fatbase;    /* FAT start sector */
    DWORD    dirbase;    /* Root directory start sector (Cluster# on FAT32) */
    DWORD    database;    /* Data start sector */
    DWORD    fptr;        /* File R/W pointer */
    DWORD    fsize;        /* File size */
    CLUST    org_clust;    /* File start cluster */
    CLUST    curr_clust;    /* File current cluster */
    DWORD    dsect;        /* File current data sector */
} FATFS;



/* Directory object structure */

typedef struct _DIR_ {
    WORD    index;        /* Current read/write index number */
    BYTE*    fn;            /* Pointer to the SFN (in/out) {file[8],ext[3],status[1]} */
    CLUST    sclust;        /* Table start cluster (0:Static table) */
    CLUST    clust;        /* Current cluster */
    DWORD    sect;        /* Current sector */
} FATDIR;



/* File status structure */

typedef struct _FILINFO_ {
    DWORD    fsize;        /* File size */
    WORD    fdate;        /* Last modified date */
    WORD    ftime;        /* Last modified time */
    BYTE    fattrib;    /* Attribute */
    char    fname[13];    /* File name */
} FILINFO;



/* File function return code (FRESULT) */

typedef enum {
    FR_OK = 0,            /* 0 */
    FR_DISK_ERR,        /* 1 */
    FR_NOT_READY,        /* 2 */
    FR_NO_FILE,            /* 3 */
    FR_NO_PATH,            /* 4 */
    FR_NOT_OPENED,        /* 5 */
    FR_NOT_ENABLED,        /* 6 */
    FR_NO_FILESYSTEM    /* 7 */
} FRESULT;



/*--------------------------------------------------------------*/
/* Petit FatFs module application interface                     */

FRESULT pf_mount (FATFS*);                        /* Mount/Unmount a logical drive */
FRESULT pf_open (const char*);                    /* Open a file */
FRESULT pf_read (void*, WORD, WORD*);            /* Read data from the open file */
FRESULT pf_write (const void*, WORD, WORD*);    /* Write data to the open file */
FRESULT pf_lseek (DWORD);                        /* Move file pointer of the open file */
FRESULT pf_opendir (FATDIR*, const char*);            /* Open a directory */
FRESULT pf_readdir (FATDIR*, FILINFO*);            /* Read a directory item from the open directory */



/*--------------------------------------------------------------*/
/* Flags and offset address                                     */

/* File status flag (FATFS.flag) */

#ifdef FA_OPENED
#undef FA_OPENED
#endif

#define    FA_OPENED    0x01
#ifdef FA_WPRT
#undef FA_WPRT
#endif

#define    FA_WPRT        0x02
#ifdef FA__WIP
#undef FA__WIP
#endif

#define    FA__WIP        0x40


/* FAT sub type (FATFS.fs_type) */

#ifdef FS_FAT12
#undef FS_FAT12
#endif

#define FS_FAT12    1
#ifdef FS_FAT16
#undef FS_FAT16
#endif

#define FS_FAT16    2
#ifdef FS_FAT32
#undef FS_FAT32
#endif

#define FS_FAT32    3


/* File attribute bits for directory entry */

#ifdef AM_RDO
#undef AM_RDO
#endif

#define    AM_RDO    0x01    /* Read only */
#ifdef AM_HID
#undef AM_HID
#endif

#define    AM_HID    0x02    /* Hidden */
#ifdef AM_SYS
#undef AM_SYS
#endif

#define    AM_SYS    0x04    /* System */
#ifdef AM_VOL
#undef AM_VOL
#endif

#define    AM_VOL    0x08    /* Volume label */
#ifdef AM_LFN
#undef AM_LFN
#endif

#define AM_LFN    0x0F    /* LFN entry */
#ifdef AM_DIR
#undef AM_DIR
#endif

#define AM_DIR    0x10    /* Directory */
#ifdef AM_ARC
#undef AM_ARC
#endif

#define AM_ARC    0x20    /* Archive */
#ifdef AM_MASK
#undef AM_MASK
#endif

#define AM_MASK    0x3F    /* Mask of defined bits */


/* FatFs refers the members in the FAT structures with byte offset instead
/ of structure member because there are incompatibility of the packing option
/ between various compilers. */

#ifdef BS_jmpBoot
#undef BS_jmpBoot
#endif

#define BS_jmpBoot            0
#ifdef BS_OEMName
#undef BS_OEMName
#endif

#define BS_OEMName            3
#ifdef BPB_BytsPerSec
#undef BPB_BytsPerSec
#endif

#define BPB_BytsPerSec        11
#ifdef BPB_SecPerClus
#undef BPB_SecPerClus
#endif

#define BPB_SecPerClus        13
#ifdef BPB_RsvdSecCnt
#undef BPB_RsvdSecCnt
#endif

#define BPB_RsvdSecCnt        14
#ifdef BPB_NumFATs
#undef BPB_NumFATs
#endif

#define BPB_NumFATs            16
#ifdef BPB_RootEntCnt
#undef BPB_RootEntCnt
#endif

#define BPB_RootEntCnt        17
#ifdef BPB_TotSec16
#undef BPB_TotSec16
#endif

#define BPB_TotSec16        19
#ifdef BPB_Media
#undef BPB_Media
#endif

#define BPB_Media            21
#ifdef BPB_FATSz16
#undef BPB_FATSz16
#endif

#define BPB_FATSz16            22
#ifdef BPB_SecPerTrk
#undef BPB_SecPerTrk
#endif

#define BPB_SecPerTrk        24
#ifdef BPB_NumHeads
#undef BPB_NumHeads
#endif

#define BPB_NumHeads        26
#ifdef BPB_HiddSec
#undef BPB_HiddSec
#endif

#define BPB_HiddSec            28
#ifdef BPB_TotSec32
#undef BPB_TotSec32
#endif

#define BPB_TotSec32        32
#ifdef BS_55AA
#undef BS_55AA
#endif

#define BS_55AA                510

#ifdef BS_DrvNum
#undef BS_DrvNum
#endif

#define BS_DrvNum            36
#ifdef BS_BootSig
#undef BS_BootSig
#endif

#define BS_BootSig            38
#ifdef BS_VolID
#undef BS_VolID
#endif

#define BS_VolID            39
#ifdef BS_VolLab
#undef BS_VolLab
#endif

#define BS_VolLab            43
#ifdef BS_FilSysType
#undef BS_FilSysType
#endif

#define BS_FilSysType        54

#ifdef BPB_FATSz32
#undef BPB_FATSz32
#endif

#define BPB_FATSz32            36
#ifdef BPB_ExtFlags
#undef BPB_ExtFlags
#endif

#define BPB_ExtFlags        40
#ifdef BPB_FSVer
#undef BPB_FSVer
#endif

#define BPB_FSVer            42
#ifdef BPB_RootClus
#undef BPB_RootClus
#endif

#define BPB_RootClus        44
#ifdef BPB_FSInfo
#undef BPB_FSInfo
#endif

#define BPB_FSInfo            48
#ifdef BPB_BkBootSec
#undef BPB_BkBootSec
#endif

#define BPB_BkBootSec        50
#ifdef BS_DrvNum32
#undef BS_DrvNum32
#endif

#define BS_DrvNum32            64
#ifdef BS_BootSig32
#undef BS_BootSig32
#endif

#define BS_BootSig32        66
#ifdef BS_VolID32
#undef BS_VolID32
#endif

#define BS_VolID32            67
#ifdef BS_VolLab32
#undef BS_VolLab32
#endif

#define BS_VolLab32            71
#ifdef BS_FilSysType32
#undef BS_FilSysType32
#endif

#define BS_FilSysType32        82

#ifdef MBR_Table
#undef MBR_Table
#endif

#define MBR_Table            446

#ifdef DIR_Name
#undef DIR_Name
#endif

#define    DIR_Name            0
#ifdef DIR_Attr
#undef DIR_Attr
#endif

#define    DIR_Attr            11
#ifdef DIR_NTres
#undef DIR_NTres
#endif

#define    DIR_NTres            12
#ifdef DIR_CrtTime
#undef DIR_CrtTime
#endif

#define    DIR_CrtTime            14
#ifdef DIR_CrtDate
#undef DIR_CrtDate
#endif

#define    DIR_CrtDate            16
#ifdef DIR_FstClusHI
#undef DIR_FstClusHI
#endif

#define    DIR_FstClusHI        20
#ifdef DIR_WrtTime
#undef DIR_WrtTime
#endif

#define    DIR_WrtTime            22
#ifdef DIR_WrtDate
#undef DIR_WrtDate
#endif

#define    DIR_WrtDate            24
#ifdef DIR_FstClusLO
#undef DIR_FstClusLO
#endif

#define    DIR_FstClusLO        26
#ifdef DIR_FileSize
#undef DIR_FileSize
#endif

#define    DIR_FileSize        28



/*--------------------------------*/
/* Multi-byte word access macros  */

#if _WORD_ACCESS == 1    /* Enable word access to the FAT structure */
#ifdef LD_WORD
#undef LD_WORD
#endif

#define    LD_WORD(ptr)        (WORD)(*(WORD*)(BYTE*)(ptr))
#ifdef LD_DWORD
#undef LD_DWORD
#endif

#define    LD_DWORD(ptr)        (DWORD)(*(DWORD*)(BYTE*)(ptr))
#ifdef ST_WORD
#undef ST_WORD
#endif

#define    ST_WORD(ptr,val)    *(WORD*)(BYTE*)(ptr)=(WORD)(val)
#ifdef ST_DWORD
#undef ST_DWORD
#endif

#define    ST_DWORD(ptr,val)    *(DWORD*)(BYTE*)(ptr)=(DWORD)(val)
#else                    /* Use byte-by-byte access to the FAT structure */
#ifdef LD_WORD
#undef LD_WORD
#endif

#define    LD_WORD(ptr)        (WORD)(((WORD)*(BYTE*)((ptr)+1)<<8)|(WORD)*(BYTE*)(ptr))
#ifdef LD_DWORD
#undef LD_DWORD
#endif

#define    LD_DWORD(ptr)        (DWORD)(((DWORD)*(BYTE*)((ptr)+3)<<24)|((DWORD)*(BYTE*)((ptr)+2)<<16)|((WORD)*(BYTE*)((ptr)+1)<<8)|*(BYTE*)(ptr))
#ifdef ST_WORD
#undef ST_WORD
#endif

#define    ST_WORD(ptr,val)    *(BYTE*)(ptr)=(BYTE)(val); *(BYTE*)((ptr)+1)=(BYTE)((WORD)(val)>>8)
#ifdef ST_DWORD
#undef ST_DWORD
#endif

#define    ST_DWORD(ptr,val)    *(BYTE*)(ptr)=(BYTE)(val); *(BYTE*)((ptr)+1)=(BYTE)((WORD)(val)>>8); *(BYTE*)((ptr)+2)=(BYTE)((DWORD)(val)>>16); *(BYTE*)((ptr)+3)=(BYTE)((DWORD)(val)>>24)
#endif


/*--------------------------------------------------------*/
/* DBCS code ranges and SBCS extend char conversion table */

#if _CODE_PAGE == 932    /* Japanese Shift-JIS */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0x81    /* DBC 1st byte range 1 start */
#ifdef _DF1E
#undef _DF1E
#endif

#define _DF1E    0x9F    /* DBC 1st byte range 1 end */
#ifdef _DF2S
#undef _DF2S
#endif

#define _DF2S    0xE0    /* DBC 1st byte range 2 start */
#ifdef _DF2E
#undef _DF2E
#endif

#define _DF2E    0xFC    /* DBC 1st byte range 2 end */
#ifdef _DS1S
#undef _DS1S
#endif

#define _DS1S    0x40    /* DBC 2nd byte range 1 start */
#ifdef _DS1E
#undef _DS1E
#endif

#define _DS1E    0x7E    /* DBC 2nd byte range 1 end */
#ifdef _DS2S
#undef _DS2S
#endif

#define _DS2S    0x80    /* DBC 2nd byte range 2 start */
#ifdef _DS2E
#undef _DS2E
#endif

#define _DS2E    0xFC    /* DBC 2nd byte range 2 end */

#elif _CODE_PAGE == 936    /* Simplified Chinese GBK */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0x81
#ifdef _DF1E
#undef _DF1E
#endif

#define _DF1E    0xFE
#ifdef _DS1S
#undef _DS1S
#endif

#define _DS1S    0x40
#ifdef _DS1E
#undef _DS1E
#endif

#define _DS1E    0x7E
#ifdef _DS2S
#undef _DS2S
#endif

#define _DS2S    0x80
#ifdef _DS2E
#undef _DS2E
#endif

#define _DS2E    0xFE

#elif _CODE_PAGE == 949    /* Korean */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0x81
#ifdef _DF1E
#undef _DF1E
#endif

#define _DF1E    0xFE
#ifdef _DS1S
#undef _DS1S
#endif

#define _DS1S    0x41
#ifdef _DS1E
#undef _DS1E
#endif

#define _DS1E    0x5A
#ifdef _DS2S
#undef _DS2S
#endif

#define _DS2S    0x61
#ifdef _DS2E
#undef _DS2E
#endif

#define _DS2E    0x7A
#ifdef _DS3S
#undef _DS3S
#endif

#define _DS3S    0x81
#ifdef _DS3E
#undef _DS3E
#endif

#define _DS3E    0xFE

#elif _CODE_PAGE == 950    /* Traditional Chinese Big5 */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0x81
#ifdef _DF1E
#undef _DF1E
#endif

#define _DF1E    0xFE
#ifdef _DS1S
#undef _DS1S
#endif

#define _DS1S    0x40
#ifdef _DS1E
#undef _DS1E
#endif

#define _DS1E    0x7E
#ifdef _DS2S
#undef _DS2S
#endif

#define _DS2S    0xA1
#ifdef _DS2E
#undef _DS2E
#endif

#define _DS2E    0xFE

#elif _CODE_PAGE == 437    /* U.S. (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x9A,0x90,0x41,0x8E,0x41,0x8F,0x80,0x45,0x45,0x45,0x49,0x49,0x49,0x8E,0x8F,0x90,0x92,0x92,0x4F,0x99,0x4F,0x55,0x55,0x59,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 720    /* Arabic (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x45,0x41,0x84,0x41,0x86,0x43,0x45,0x45,0x45,0x49,0x49,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x49,0x49,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 737    /* Greek (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x92,0x92,0x93,0x94,0x95,0x96,0x97,0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87, \
                0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0xAA,0x92,0x93,0x94,0x95,0x96,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0x97,0xEA,0xEB,0xEC,0xE4,0xED,0xEE,0xE7,0xE8,0xF1,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 775    /* Baltic (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x9A,0x91,0xA0,0x8E,0x95,0x8F,0x80,0xAD,0xED,0x8A,0x8A,0xA1,0x8D,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0x95,0x96,0x97,0x97,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xE0,0xA3,0xA3,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xB5,0xB6,0xB7,0xB8,0xBD,0xBE,0xC6,0xC7,0xA5,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE3,0xE8,0xE8,0xEA,0xEA,0xEE,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 850    /* Multilingual Latin 1 (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
                0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 852    /* Latin 2 (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT

#endif

#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xDE,0x8F,0x80,0x9D,0xD3,0x8A,0x8A,0xD7,0x8D,0x8E,0x8F,0x90,0x91,0x91,0xE2,0x99,0x95,0x95,0x97,0x97,0x99,0x9A,0x9B,0x9B,0x9D,0x9E,0x9F, \
                0xB5,0xD6,0xE0,0xE9,0xA4,0xA4,0xA6,0xA6,0xA8,0xA8,0xAA,0x8D,0xAC,0xB8,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBD,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC6,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD2,0xD5,0xD6,0xD7,0xB7,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE3,0xD5,0xE6,0xE6,0xE8,0xE9,0xE8,0xEB,0xED,0xED,0xDD,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xEB,0xFC,0xFC,0xFE,0xFF}

#elif _CODE_PAGE == 855    /* Cyrillic (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x81,0x81,0x83,0x83,0x85,0x85,0x87,0x87,0x89,0x89,0x8B,0x8B,0x8D,0x8D,0x8F,0x8F,0x91,0x91,0x93,0x93,0x95,0x95,0x97,0x97,0x99,0x99,0x9B,0x9B,0x9D,0x9D,0x9F,0x9F, \
                0xA1,0xA1,0xA3,0xA3,0xA5,0xA5,0xA7,0xA7,0xA9,0xA9,0xAB,0xAB,0xAD,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB6,0xB6,0xB8,0xB8,0xB9,0xBA,0xBB,0xBC,0xBE,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD3,0xD3,0xD5,0xD5,0xD7,0xD7,0xDD,0xD9,0xDA,0xDB,0xDC,0xDD,0xE0,0xDF, \
                0xE0,0xE2,0xE2,0xE4,0xE4,0xE6,0xE6,0xE8,0xE8,0xEA,0xEA,0xEC,0xEC,0xEE,0xEE,0xEF,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF8,0xFA,0xFA,0xFC,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 857    /* Turkish (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0x98,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x98,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9E, \
                0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA6,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xDE,0x59,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 858    /* Multilingual Latin 1 + Euro (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x9A,0x90,0xB6,0x8E,0xB7,0x8F,0x80,0xD2,0xD3,0xD4,0xD8,0xD7,0xDE,0x8E,0x8F,0x90,0x92,0x92,0xE2,0x99,0xE3,0xEA,0xEB,0x59,0x99,0x9A,0x9D,0x9C,0x9D,0x9E,0x9F, \
                0xB5,0xD6,0xE0,0xE9,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC7,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD1,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE5,0xE5,0xE6,0xE7,0xE7,0xE9,0xEA,0xEB,0xED,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 862    /* Hebrew (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT

#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0x41,0x49,0x4F,0x55,0xA5,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0x21,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 866    /* Russian (OEM) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0x90,0x91,0x92,0x93,0x9d,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F,0xF0,0xF0,0xF2,0xF2,0xF4,0xF4,0xF6,0xF6,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 874    /* Thai (OEM, Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1250 /* Central Europe (Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xA3,0xB4,0xB5,0xB6,0xB7,0xB8,0xA5,0xAA,0xBB,0xBC,0xBD,0xBC,0xAF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _CODE_PAGE == 1251 /* Cyrillic (Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x82,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x80,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x8D,0x8E,0x8F, \
                0xA0,0xA2,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB2,0xA5,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xA3,0xBD,0xBD,0xAF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF}

#elif _CODE_PAGE == 1252 /* Latin 1 (Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0xAd,0x9B,0x8C,0x9D,0xAE,0x9F, \
                0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _CODE_PAGE == 1253 /* Greek (Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xA2,0xB8,0xB9,0xBA, \
                0xE0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xFB,0xBC,0xFD,0xBF,0xFF}

#elif _CODE_PAGE == 1254 /* Turkish (Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x8A,0x9B,0x8C,0x9D,0x9E,0x9F, \
                0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0x9F}

#elif _CODE_PAGE == 1255 /* Hebrew (Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEE,0xEF,0xF0,0xF1,0xF2,0xF3,0xF4,0xF5,0xF6,0xF7,0xF8,0xF9,0xFA,0xFB,0xFC,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1256 /* Arabic (Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x8C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0x41,0xE1,0x41,0xE3,0xE4,0xE5,0xE6,0x43,0x45,0x45,0x45,0x45,0xEC,0xED,0x49,0x49,0xF0,0xF1,0xF2,0xF3,0x4F,0xF5,0xF6,0xF7,0xF8,0x55,0xFA,0x55,0x55,0xFD,0xFE,0xFF}

#elif _CODE_PAGE == 1257 /* Baltic (Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0x9C,0x9D,0x9E,0x9F, \
                0xA0,0xA1,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xA8,0xB9,0xAA,0xBB,0xBC,0xBD,0xBE,0xAF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xFF}

#elif _CODE_PAGE == 1258 /* Vietnam (OEM, Windows) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0
#ifdef _EXCVT
#undef _EXCVT
#endif

#define _EXCVT {0x80,0x81,0x82,0x83,0x84,0x85,0x86,0x87,0x88,0x89,0x8A,0x8B,0x8C,0x8D,0x8E,0x8F,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0x9B,0xAC,0x9D,0x9E,0x9F, \
                0xA0,0x21,0xA2,0xA3,0xA4,0xA5,0xA6,0xA7,0xA8,0xA9,0xAA,0xAB,0xAC,0xAD,0xAE,0xAF,0xB0,0xB1,0xB2,0xB3,0xB4,0xB5,0xB6,0xB7,0xB8,0xB9,0xBA,0xBB,0xBC,0xBD,0xBE,0xBF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD0,0xD1,0xD2,0xD3,0xD4,0xD5,0xD6,0xD7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xDE,0xDF, \
                0xC0,0xC1,0xC2,0xC3,0xC4,0xC5,0xC6,0xC7,0xC8,0xC9,0xCA,0xCB,0xEC,0xCD,0xCE,0xCF,0xD0,0xD1,0xF2,0xD3,0xD4,0xD5,0xD6,0xF7,0xD8,0xD9,0xDA,0xDB,0xDC,0xDD,0xFE,0x9F}

#elif _CODE_PAGE == 1    /* ASCII (for only non-LFN cfg) */
#ifdef _DF1S
#undef _DF1S
#endif

#define _DF1S    0

#else
#error Unknown code page
#endif



/* Character code support macros */

#ifdef IsUpper
#undef IsUpper
#endif

#define IsUpper(c)    (((c)>='A')&&((c)<='Z'))
#ifdef IsLower
#undef IsLower
#endif

#define IsLower(c)    (((c)>='a')&&((c)<='z'))

#if _DF1S        /* DBCS configuration */

#ifdef _DF2S    /* Two 1st byte areas */
#ifdef IsDBCS1
#undef IsDBCS1
#endif

#define IsDBCS1(c)    (((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E) || ((BYTE)(c) >= _DF2S && (BYTE)(c) <= _DF2E))
#else            /* One 1st byte area */
#ifdef IsDBCS1
#undef IsDBCS1
#endif

#define IsDBCS1(c)    ((BYTE)(c) >= _DF1S && (BYTE)(c) <= _DF1E)
#endif

#ifdef _DS3S    /* Three 2nd byte areas */
#ifdef IsDBCS2
#undef IsDBCS2
#endif

#define IsDBCS2(c)    (((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E) || ((BYTE)(c) >= _DS3S && (BYTE)(c) <= _DS3E))
#else            /* Two 2nd byte areas */
#ifdef IsDBCS2
#undef IsDBCS2
#endif

#define IsDBCS2(c)    (((BYTE)(c) >= _DS1S && (BYTE)(c) <= _DS1E) || ((BYTE)(c) >= _DS2S && (BYTE)(c) <= _DS2E))
#endif

#else            /* SBCS configuration */

#ifdef IsDBCS1
#undef IsDBCS1
#endif

#define IsDBCS1(c)    0
#ifdef IsDBCS2
#undef IsDBCS2

#endif

#define IsDBCS2(c)    0

#endif /* _DF1S */

} // namespace PFFS

#endif /* _FATFS */

