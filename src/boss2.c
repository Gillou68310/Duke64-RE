#include "common.h"
#include "duke3d.h"

extern u8 boss2_ROM_START[];
extern u8 boss2_ROM_END[];

extern s32 D_801CD820;
extern s8 D_801CD824;
extern s8 D_801CD825;
extern s8 D_801CD826;
extern s32 D_801CD828;
extern s32 D_801CD82C;
extern f32 D_801CD830;
extern u8 D_801CD834;
extern char* D_801CD838;
extern f32 D_801CD83C;

void func_80056150(void)
{
    D_801CD834 = 0xC8;
    allocache2((long *)&D_801CD838, 0x187E0, &D_801CD834);
    copyfromrom(D_8025FEC0, (u32)boss2_ROM_START, 0xB3F8);
    RNCDecompress(D_8025FEC0, D_801CD838);
    D_801CD828 = 0;
    D_801CD82C = 0;
    D_801CD830 = 0.0f;
    D_801CD820 = 0;
    D_801CD824 = 1;
    D_801CD825 = 1;
    D_801CD826 = 0x14;
    D_801CD83C = 0.0f;
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/boss2/func_80056200.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/boss2/animateboss2.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/boss2/func_8005669C.s")
