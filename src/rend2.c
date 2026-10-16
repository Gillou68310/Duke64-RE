#include "common.h"
#include "duke3d.h"

extern f32 _globalXrepeat;
extern f32 _globalYrepeat;
extern f32 _globalXpanning;
extern f32 _globalYpanning;
extern f32 _globalWallVoffset;
extern f32 _globalTileDimY;
extern f32 _globalTileScaleX;
extern f32 _globalTileScaleY;
extern f32 _globalWallU1;
extern f32 _globalWallU2;
extern f32 _globalWallV1;
extern f32 _globalWallV2;
extern f32 _globalWallV3;
extern f32 _globalWallV4;
extern s32 gDrawCeilCnt;
extern s32 gDrawFloorCnt;
extern s32 gDrawWallCnt;
extern s32 D_8021E4CC;
extern u16 D_8026EC00;
extern s16 tilemap[MAXTILES];

u8 *loadtile(u16 tileid);

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8005B5F0.s")

void func_8005B720(void) {
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8005B728.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/display_game.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8005BB48.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8005BB64.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8005BC30.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8005BF2C.s")

void clearscreen(void)
{
    gDPPipeSync(pkt++);
    gSPClearGeometryMode(pkt++, G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING |
                                G_TEXTURE_GEN | G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH);
    gSPSetGeometryMode(pkt++, G_SHADE | G_SHADING_SMOOTH);
    gDPSetTextureLUT(pkt++, G_TT_RGBA16);
    gDPSetCombineMode(pkt++, G_CC_DECALRGBA, G_CC_DECALRGBA);
    gDPSetRenderMode(pkt++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
    gDPSetAlphaCompare(pkt++, G_AC_THRESHOLD);
    gDPSetTexturePersp(pkt++, G_TP_NONE);
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8005C390.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8005C4D4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/rotatesprite_simplest.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/rotatesprite.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/rotatespritesc.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80060B34.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80060C40.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80060EAC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006141C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800616A8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800617AC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80061904.s")

/*func_8000BDB0 in dukezh*/
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80061CB8.s")

void func_80061FC8(s16 tileid)
{
    if (tileid != D_8026EC00)
    {
        gDPLoadTLUT_pal16(pkt++, 0, loadtile(tileid));
        gDPLoadTextureBlock_4b(pkt++, loadtile(tileid)+32, G_IM_FMT_CI,
                               tileinfo[tileid].dim1, tileinfo[tileid].dim2, 0,
                               G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                               0, 0, 0, 0);
        D_8021E4CC++;
        D_8026EC00 = tileid;
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800622E0.s")

void func_80062304(void) {
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006230C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062314.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800624B4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062680.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062804.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062AD0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062D98.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063168.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063270.s")

void func_800632BC(s32, s32);
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800632BC.s")

void func_80063470(s32, s32);
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063470.s")

void func_80063594(s32, s32);
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063594.s")

void func_800636B8(void)
{
    func_800632BC(0, gDrawWallCnt - 1);
    func_80063470(0, gDrawFloorCnt - 1);
    func_80063594(0, gDrawCeilCnt - 1);
}

u16 gettileid(u16 tilenum)
{
    u16 tileid;

    if (tilenum == 0)
        return 0;
    tileid = tilemap[tilenum];

    if (tileid == MAXTILES)
        return 1;
    else
        return tileid;
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063750.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800637C8.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/loadtile.s")

s32 tilemasks(u16 tileid)
{
    switch (tileinfo[tileid].dim1)
    {
    case 1:
        return 0;
    case 2:
        return 1;
    case 4:
        return 2;
    case 8:
        return 3;
    case 16:
        return 4;
    case 32:
        return 5;
    case 64:
        return 6;
    case 128:
        return 7;
    case 256:
        return 8;
    }
    return 0;
}

s32 tilemaskt(u16 tileid)
{
    switch (tileinfo[tileid].dim2)
    {
    case 1:
        return 0;
    case 2:
        return 1;
    case 4:
        return 2;
    case 8:
        return 3;
    case 16:
        return 4;
    case 32:
        return 5;
    case 64:
        return 6;
    case 128:
        return 7;
    case 256:
        return 8;
    }
    return 0;
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063BB8.s")

void gametext(s16 x, s16 y, char *string)
{
    s32 x_, y_;
    char *ptr;

    x_ = x;
    y_ = y;

    if (x_ == -1)
    {
        for (ptr = string, x = 0; ((*ptr != 0) && (*ptr != '\n') && (*ptr != '\r')); ptr++)
        {
            if (*ptr == ' ')
                x += 7;
            else
                x += tileinfo[gettileid((*ptr + 2789))].tilesizx;
        }
        x_ = (320 / 2) - x / 2;
    }

    for (; ((*string != 0) && (*string != '\n') && (*string != '\r')); string++)
    {
        rotatesprite_simple(x_, y_, *string + (2822 - '!'));
        if (*string == ' ')
            x_ += 7;
        else
            x_ += tileinfo[gettileid((*string + 2789))].tilesizx;
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/menutext.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064418.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064644.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064674.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/D_801048D0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/display_tileviewer.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/cacheandtileinit.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/addexplosionmodel2.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/addexplosionmodel.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064A54.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064AA4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064D28.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800651C0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80065458.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/rotatesprite_simple.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066170.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066250.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/animatemodels.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066324.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066718.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006693C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066BE0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066DF4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800674EC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/settileglobals.s")

void setwallglobals(u16 wallnum, s16 cstat)
{
    _globalXrepeat = wall[wallnum].xrepeat / _globalTileScaleX;
    _globalXpanning = wall[wallnum].xpanning / _globalTileScaleX;
    _globalYrepeat = (wall[wallnum].yrepeat / _globalTileScaleY) / 4;

    if (cstat & 256)
    {
        _globalYpanning = (255 - wall[wallnum].ypanning);
        _globalYpanning = (_globalTileDimY / 256) * _globalYpanning;
    }
    else
    {
        _globalYpanning = (wall[wallnum].ypanning);
        _globalYpanning = (_globalTileDimY / 256) * _globalYpanning;
        _globalYrepeat = -_globalYrepeat;
    }

    _globalWallVoffset = _globalYpanning;

    if ((cstat&4)==4)
        _globalWallVoffset = (_globalTileDimY * 32.0f) + (_globalYpanning * 32.0f);
    else
        _globalWallVoffset = _globalYpanning * 32.0f;

    _globalWallU1 = _globalXpanning * 32.0f;
    _globalWallU2 = (_globalXrepeat * 8.0f * 32.0f) + _globalXpanning * 32.0f;
}

void setwallglobals2(u16 wallnum, s16 cstat)
{
    _globalXrepeat = wall[wallnum].xrepeat / _globalTileScaleX;
    _globalXpanning = wall[wall[wallnum].nextwall].xpanning / _globalTileScaleX;
    _globalYrepeat = (wall[wallnum].yrepeat / _globalTileScaleY) / 4;

    if ((cstat&0x100)==0x100)
    {
        _globalYpanning = (0xFF - wall[wall[wallnum].nextwall].ypanning);
        _globalYpanning = (_globalTileDimY / 256) * _globalYpanning;
    }
    else
    {
        _globalYpanning = wall[wall[wallnum].nextwall].ypanning;
        _globalYpanning = (_globalTileDimY / 256) * _globalYpanning;
        _globalYrepeat = -_globalYrepeat;
    }

    _globalWallVoffset = _globalYpanning;

    if ((cstat&4)==4)
        _globalWallVoffset = (_globalTileDimY * 32.0f) + (_globalYpanning * 32.0f);
    else
        _globalWallVoffset = _globalYpanning * 32.0f;

    _globalWallU1 = _globalXpanning * 32.0f;
    _globalWallU2 = (_globalXrepeat * 8.0f * 32.0f) + _globalXpanning * 32.0f;
}

void handlewallcstatslope(s16 cstat)
{
    f32 f1, prev;
    s32 adjust;

    if ((cstat&8)==8)
    {
        prev = _globalWallU1;
        _globalWallU1 = _globalWallU2;
        _globalWallU2 = prev;
    }
    f1 = MIN(_globalWallV1, MIN(_globalWallV2, MIN(_globalWallV3, _globalWallV4)));
    if (f1 < -32760.0f)
    {
        adjust = f1 - (-32760.0f);
        adjust = ((adjust + 0xFFF) & ~0xFFF);
        _globalWallV1 += adjust;
        _globalWallV2 += adjust;
        _globalWallV3 += adjust;
        _globalWallV4 += adjust;
    }
    f1 = MAX(_globalWallV1, MAX(_globalWallV2, MAX(_globalWallV3, _globalWallV4)));
    if (f1 > 32760.0f)
    {
        adjust = f1 - 32760.0f;
        adjust = ((adjust + 0xFFF) & ~0xFFF);
        _globalWallV1 -= adjust;
        _globalWallV2 -= adjust;
        _globalWallV3 -= adjust;
        _globalWallV4 -= adjust;
    }
}

void handlewallcstat(s16 cstat)
{
    f32 f1, prev;
    s32 adjust;

    if ((cstat&8) == 8)
    {
        prev = _globalWallU1;
        _globalWallU1 = _globalWallU2;
        _globalWallU2 = prev;
    }
    f1 = min(_globalWallV1, _globalWallV2);
    if (f1 < -32760.0f)
    {
        adjust = klabs2(f1) - 32760.0f;
        adjust = ((adjust + 0xFFF) & ~0xFFF);
        _globalWallV1 += adjust;
        _globalWallV2 += adjust;
        _globalWallV3 += adjust;
        _globalWallV4 += adjust;
    }
    f1 = max(_globalWallV1, _globalWallV2);
    if (f1 > 32760.0f)
    {
        adjust = f1 - 32760.0f;
        adjust = ((adjust + 0xFFF) & ~0xFFF);
        _globalWallV1 -= adjust;
        _globalWallV2 -= adjust;
        _globalWallV3 -= adjust;
        _globalWallV4 -= adjust;
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

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/wallcalcslope.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/drawmaskwall.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/wallcalcnoslope.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006A310.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006A62C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006ADA4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006C2FC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006CB30.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/rendertileworld.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/setfxcolor.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/unsetfxsprite.s")
