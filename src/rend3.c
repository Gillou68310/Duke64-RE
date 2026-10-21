#include "common.h"
#include "duke3d.h"

#define WALL_VERTEX(X, Y, Z, U, V) \
    wallvertex[i].x = X >> 1;      \
    wallvertex[i].z = Z >> 1;      \
    wallvertex[i].y = Y >> 1;      \
    wallvertex[i].u = (s32)U;      \
    wallvertex[i].v = (s32)V;      \
    i++;

extern u16 globaltileid;
extern f32 globaltilesizx;
extern f32 globaltilesizy;
extern f32 globaltiledimx;
extern f32 globalxrepeat;
extern f32 globalyrepeat;
extern f32 globalxpanning;
extern f32 globalypanning;
extern f32 globalwallvoffset;
extern f32 globaltiledimy;
extern f32 globaltilescalex;
extern f32 globaltilescaley;
extern f32 globalwallu1;
extern f32 globalwallu2;
extern f32 globalwallv1;
extern f32 globalwallv2;
extern f32 globalwallv3;
extern f32 globalwallv4;
extern s8 D_8026EC31;
extern float projmatrix[4][4];

void settileglobals(u16 tilenum)
{
    globaltileid = gettileid(tilenum);
    globaltilesizx = tileinfo[globaltileid].tilesizx;
    globaltilesizy = tileinfo[globaltileid].tilesizy;
    globaltiledimx = tileinfo[globaltileid].dimx;
    globaltiledimy = tileinfo[globaltileid].dimy;
    globaltilescalex = globaltilesizx / globaltiledimx;
    globaltilescaley = globaltilesizy / globaltiledimy;
}

void setwallglobals(u16 wallnum, s16 cstat)
{
    globalxrepeat = wall[wallnum].xrepeat / globaltilescalex;
    globalxpanning = wall[wallnum].xpanning / globaltilescalex;
    globalyrepeat = (wall[wallnum].yrepeat / globaltilescaley) / 4;

    if (cstat & 256)
    {
        globalypanning = (255 - wall[wallnum].ypanning);
        globalypanning = (globaltiledimy / 256) * globalypanning;
    }
    else
    {
        globalypanning = (wall[wallnum].ypanning);
        globalypanning = (globaltiledimy / 256) * globalypanning;
        globalyrepeat = -globalyrepeat;
    }

    globalwallvoffset = globalypanning;

    if ((cstat & 4) == 4)
        globalwallvoffset = (globaltiledimy * 32.0f) + (globalypanning * 32.0f);
    else
        globalwallvoffset = globalypanning * 32.0f;

    globalwallu1 = globalxpanning * 32.0f;
    globalwallu2 = (globalxrepeat * 8.0f * 32.0f) + globalxpanning * 32.0f;
}

void setwallglobals2(u16 wallnum, s16 cstat)
{
    globalxrepeat = wall[wallnum].xrepeat / globaltilescalex;
    globalxpanning = wall[wall[wallnum].nextwall].xpanning / globaltilescalex;
    globalyrepeat = (wall[wallnum].yrepeat / globaltilescaley) / 4;

    if ((cstat & 0x100) == 0x100)
    {
        globalypanning = (0xFF - wall[wall[wallnum].nextwall].ypanning);
        globalypanning = (globaltiledimy / 256) * globalypanning;
    }
    else
    {
        globalypanning = wall[wall[wallnum].nextwall].ypanning;
        globalypanning = (globaltiledimy / 256) * globalypanning;
        globalyrepeat = -globalyrepeat;
    }

    globalwallvoffset = globalypanning;

    if ((cstat & 4) == 4)
        globalwallvoffset = (globaltiledimy * 32.0f) + (globalypanning * 32.0f);
    else
        globalwallvoffset = globalypanning * 32.0f;

    globalwallu1 = globalxpanning * 32.0f;
    globalwallu2 = (globalxrepeat * 8.0f * 32.0f) + globalxpanning * 32.0f;
}

void handlewallcstatslope(s16 cstat)
{
    f32 f1, prev;
    s32 adjust;

    if ((cstat & 8) == 8)
    {
        prev = globalwallu1;
        globalwallu1 = globalwallu2;
        globalwallu2 = prev;
    }
    f1 = MIN(globalwallv1, MIN(globalwallv2, MIN(globalwallv3, globalwallv4)));
    if (f1 < -32760.0f)
    {
        adjust = f1 - (-32760.0f);
        adjust = ((adjust + 0xFFF) & ~0xFFF);
        globalwallv1 += adjust;
        globalwallv2 += adjust;
        globalwallv3 += adjust;
        globalwallv4 += adjust;
    }
    f1 = MAX(globalwallv1, MAX(globalwallv2, MAX(globalwallv3, globalwallv4)));
    if (f1 > 32760.0f)
    {
        adjust = f1 - 32760.0f;
        adjust = ((adjust + 0xFFF) & ~0xFFF);
        globalwallv1 -= adjust;
        globalwallv2 -= adjust;
        globalwallv3 -= adjust;
        globalwallv4 -= adjust;
    }
}

void handlewallcstat(s16 cstat)
{
    f32 f1, prev;
    s32 adjust;

    if ((cstat & 8) == 8)
    {
        prev = globalwallu1;
        globalwallu1 = globalwallu2;
        globalwallu2 = prev;
    }
    f1 = min(globalwallv1, globalwallv2);
    if (f1 < -32760.0f)
    {
        adjust = klabs2(f1) - 32760.0f;
        adjust = ((adjust + 0xFFF) & ~0xFFF);
        globalwallv1 += adjust;
        globalwallv2 += adjust;
        globalwallv3 += adjust;
        globalwallv4 += adjust;
    }
    f1 = max(globalwallv1, globalwallv2);
    if (f1 > 32760.0f)
    {
        adjust = f1 - 32760.0f;
        adjust = ((adjust + 0xFFF) & ~0xFFF);
        globalwallv1 -= adjust;
        globalwallv2 -= adjust;
        globalwallv3 -= adjust;
        globalwallv4 -= adjust;
    }
}

void wallcalc(s32 sectnum, s32 wallnum)
{
    s16 nextsector;
    s16 cstat;

    nextsector = wall[wallnum].nextsector;
    cstat = sector[sectnum].floorstat;
    cstat |= sector[sectnum].ceilingstat;
    if (nextsector != -1)
    {
        cstat |= sector[nextsector].floorstat;
        cstat |= sector[nextsector].ceilingstat;
    }

    if (cstat & 2)
        wallcalcslope(sectnum, wallnum);
    else
        wallcalcnoslope(sectnum, wallnum);
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend3/wallcalcslope.s")

s32 drawmaskwall(u16 wallnum)
{
    s32 x1, x2;
    s32 y1, y2;
    s32 ceilz1, floorz1;
    s32 floorz2, ceilz2;
    s32 z1, z2, z3;
    s16 cstat;
    u16 i;
    s32 ret;

    ret = 0;
    i = 0;
    wallpolycount = 0;

    x1 = wall[wallnum].x;
    y1 = wall[wallnum].y;
    x2 = wall[wall[wallnum].point2].x;
    y2 = wall[wall[wallnum].point2].y;
    ceilz1 = getceilzofslope(wall[wallnum].sectnum, x1, y1) >> 4;
    ceilz2 = getceilzofslope(wall[wallnum].sectnum, x2, y2) >> 4;
    z1 = getceilzofslope(wall[wallnum].nextsector, x1, y1) >> 4;
    z2 = getceilzofslope(wall[wallnum].nextsector, x2, y2) >> 4;

    if (ceilz1 < z1)
    {
        ceilz1 = z1;
        ceilz2 = z2;
    }

    floorz1 = getflorzofslope(wall[wallnum].sectnum, x1, y1) >> 4;
    floorz2 = getflorzofslope(wall[wallnum].sectnum, x2, y2) >> 4;
    z1 = getflorzofslope(wall[wallnum].nextsector, x1, y1) >> 4;
    z2 = getflorzofslope(wall[wallnum].nextsector, x2, y2) >> 4;

    if (z1 < floorz1)
    {
        floorz1 = z1;
        floorz2 = z2;
    }

    if ((wall[wallnum].cstat & 4) != 4)
    {
        z3 = MAX((sector[wall[wallnum].sectnum].ceilingz >> 4),
                 (sector[wall[wallnum].nextsector].ceilingz >> 4));
        cstat = wall[wallnum].cstat & ~4;
    }
    else
    {
        z3 = MIN((sector[wall[wallnum].sectnum].floorz >> 4),
                 (sector[wall[wallnum].nextsector].floorz >> 4));
        cstat = wall[wallnum].cstat & ~4;
    }

    settileglobals(wall[wallnum].overpicnum);
    setwallglobals(wallnum, cstat);

    globalwallv1 = ((z3 - ceilz1) * globalyrepeat) + globalwallvoffset;
    globalwallv2 = ((z3 - floorz1) * globalyrepeat) + globalwallvoffset;
    globalwallv3 = ((z3 - floorz2) * globalyrepeat) + globalwallvoffset;
    globalwallv4 = ((z3 - ceilz2) * globalyrepeat) + globalwallvoffset;
    handlewallcstatslope(cstat);

    WALL_VERTEX(x1, y1, ceilz1, globalwallu1, globalwallv1)
    WALL_VERTEX(x1, y1, floorz1, globalwallu1, globalwallv2)
    WALL_VERTEX(x2, y2, floorz2, globalwallu2, globalwallv3)
    WALL_VERTEX(x2, y2, ceilz2, globalwallu2, globalwallv4)

    ret |= 4;
    wallpolycount++;
    return ret;
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend3/wallcalcnoslope.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend3/func_8006A310.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend3/func_8006A62C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend3/func_8006ADA4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend3/func_8006C2FC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend3/func_8006CB30.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend3/rendertileworld.s")

void setfxcolor(u8 r1, u8 g1, u8 b1, u8 r2, u8 g2, u8 b2)
{
    gDPSetAlphaCompare(pkt++, G_AC_NONE);
    gDPSetTextureLUT(pkt++, G_TT_NONE);
    gDPSetRenderMode(pkt++, G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2);
    gDPSetCombineLERP(pkt++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0,
                      PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, TEXEL0, 0, PRIMITIVE, 0);
    D_8026EC31 = 1;
    gDPSetPrimColor(pkt++, 0, 0, r1, g1, b1, 0xFF);
    gDPSetEnvColor(pkt++, r2, g2, b2, 0xFF);
}

void unsetfxsprite(void)
{
    gDPSetAlphaCompare(pkt++, G_AC_THRESHOLD);
    gDPSetTextureLUT(pkt++, G_TT_RGBA16);
    gDPSetCombineMode(pkt++, G_CC_MODULATEIA, G_CC_MODULATEIA);
    D_8026EC31 = 0;
}
