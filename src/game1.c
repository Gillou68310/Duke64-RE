#include "common.h"
#include "duke3d.h"

extern s16 D_800D3774;
extern s16 D_800D3778;

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/gameplay.s")

s16 func_8004DCB8(void);
#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/func_8004DCB8.s")

void func_8004DED8(void)
{
    sound(0xC);
    menu_levelnum = func_8004DCB8();
    levelnum = menu_levelnum;
    ud.secretlevel = 0;
    menu_inputfreeze = 0x32;
    func_80055450();
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/display_bonus.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102850.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102864.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102870.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_8010287C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102888.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102898.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801028A4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801028B0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801028BC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801028C8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801028D4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801028E0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801028F0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102904.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102918.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102928.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102934.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102940.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_8010294C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_8010295C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102968.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102974.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102980.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_8010298C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102998.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801029A8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801029B4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801029C0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801029C8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801029D0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801029D8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801029EC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801029F4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801029FC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/func_8004E3F8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/func_8004ECF0.s")

void func_8004F96C(s16 arg0)
{
    if (arg0 != 0)
    {
        D_800D3778 += 1;
        D_800D3774 = 0;
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/func_8004F9A0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/display_intro.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/func_80050754.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/func_80050A10.s")

void func_80050E18(u8 arg0, s32 y, char *string)
{
    s16 a;

    if ((y >= 0) && (y <= SCREEN_HEIGHT))
    {
        if (y > 0xC0)
        {
            a = (-y * 16) + 0xD00;
        }
        else
        {
            if ((y * 16) - 0x100 > 0x100)
                a = 0x100;
            else
                a = (y * 16) - 0x100;
        }
        setrotatespriteflags(0xFF, 0xFF, 0xFF, a);
        if (arg0 == 1)
            gametext(-1, y, string);
        else
            menutext(-1, y, string);
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102B34.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102BC8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102C7C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102D14.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102D74.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102D94.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102DAC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102DE4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102E38.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102EB0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102F1C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102F84.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80102FAC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80103004.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_80103054.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_801030A4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/D_8010310C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/game1/func_80050EE4.s")
