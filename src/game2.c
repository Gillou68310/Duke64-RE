#include "common.h"
#include "duke3d.h"

typedef struct
{
    /*0x00*/ u32 unk0;
    /*0x04*/ u32 game_code;
    /*0x08*/ u16 company_code;
    /*0x0A*/ u8 ext_name[4];
    /*0x0E*/ u8 game_name[18];
} unk801CBDE0;

extern u8 D_800D38D4[];
extern volatile s8 D_800D3918;

extern char D_801CBDEA[];
extern char D_801CBDEE[];
extern unk801CBDE0 D_801CBDE0;

void func_80051D40(void)
{
    D_800D3918 = 1;
    func_800518A0();
}

void func_80051D6C(void)
{
    func_800518E4();
    D_800D3918 = 0;
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_80051D98.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_80052000.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_800524F0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_80052660.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_800528AC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/display_savegame.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/display_loadgame.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/display_pickslot.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_80054B8C.s")

void func_80054DE0(s32);
#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_80054DE0.s")

void func_80055060(void)
{
    func_80054B8C(0);
    func_80054B8C(1);
}

void loadgame(void)
{
    func_80054DE0(0);
    func_80054DE0(1);
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_800550B0.s")

char func_800551F8();
#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_800551F8.s")

void func_80055374(void)
{
    s32 i, j;

    D_801CBDE0.company_code = 0x3458;
    D_801CBDE0.game_code = 0x4E444E45;
    clearbufbyte(D_801CBDEE, 0, 0x10);
    clearbufbyte(D_801CBDEA, 0, 4);
    sprintf(D_801CBDEE, "DUKE NUKEM L %d", levelnum + 1);
    D_801CBDEA[0] = func_800551F8();
    for (i = 0; i < 16; i++)
    {
        for (j = 0; j != 68; j++)
        {
            if (D_801CBDE0.game_name[i] == D_800D38D4[j])
            {
                D_801CBDE0.game_name[i] = j;
                break;
            }
        }
    }
}

/*TODO*/
static const char D_80103E48[] = "nf = %d\n";
static const char D_80103E54[] = "osPfsDeleteFile = %d\n";
static const char D_80103E6C[] = "1\n";
static const char D_80103E70[] = "2\n";
static const char D_80103E74[] = "osPfsAllocateFile = %d\n";
static const char D_80103E8C[] = "3\n";
static const char D_80103E90[] = "osPfsReadWriteFile = %d\n";
static const char D_80103EAC[] = "4\n";
static const char D_80103EB0[] = "SIZEOF DUKESAVE GAME %d\n";
static const char D_80103ECC[] = "nf = %d\n";
static const char D_80103ED8[] = "osPfsFindFile\n";
static const char D_80103EE8[] = "ret = %d\n";
static const char D_80103EF4[] = "osPfsReadWriteFile\n";
static const char D_80103F08[] = "ret = %d\n";

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_80055450.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game2/func_80055598.s")
