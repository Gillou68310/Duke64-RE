#include "common.h"
#include "duke3d.h"

typedef struct
{
    /*0x00*/ u32 rom_start;
    /*0x04*/ u32 rom_end;
    /*0x08*/ void *sector_addr;
    /*0x0C*/ void *wall_addr;
    /*0x10*/ void *sprite_addr;
    /*0x14*/ s32 numsectors;
    /*0x18*/ s32 numsprites;
    /*0x1C*/ s32 numwalls;
    /*0x20*/ s32 xpos;
    /*0x24*/ s32 ypos;
    /*0x28*/ s32 zpos;
    /*0x2C*/ s32 ang;
    /*0x30*/ f32 sky_re;
    /*0x34*/ f32 sky_ge;
    /*0x38*/ f32 sky_be;
    /*0x3C*/ f32 sky_r;
    /*0x40*/ f32 sky_g;
    /*0x44*/ f32 sky_b;
} boardinfo_t;

typedef struct
{
    /*0x00*/ s8 status;
    /*0x01*/ s8 type;
    /*0x02*/ s16 x;
    /*0x04*/ s16 y;
    /*0x06*/ s16 z;
    /*0x08*/ u16 orientation;
    /*0x0C*/ s32 unkC;
    /*0x10*/ f32 scale;
    /*0x14*/ f32 phase;
    /*0x18*/ f32 phase_step;
} smokeinstance_t;

typedef struct
{
    u8 enable;
    u16 time;
    s16 startsize, step, stepdec;
    u8 color1[3], color2[3];
} explosioninfo_t;

typedef struct
{
    /*0x00*/ s8 status;
    /*0x01*/ u8 type;
    /*0x02*/ u8 c_enable, r_enable;
    /*0x04*/ s16 x, y, z;
    /*0x0A*/ s16 c_size, r_size;
    /*0x0E*/ u16 c_time, r_time;
    /*0x12*/ s16 c_step, r_step;
} explosioninstance_t;

#define MAXEXPLOSIONS 16

extern u16 D_8026EC00;
extern s16 tilemap[MAXTILES];
extern s32 D_800D9AEC;
extern u8 D_8025F398[];
extern char D_8025FEC0[];
extern s32 globalboardnum;
extern u16 *D_8026EB94;
extern u16 D_8026EB98;
extern vertex_t wallvertex[12];
extern s32 D_8021E4CC;
extern u16 D_8021D67E;
extern s32 D_8021D700;
extern s32 wallpolycount;
extern f32 viewportscalex;
extern f32 viewportscaley;
extern f32 viewporttransx;
extern f32 viewporttransy;
extern f32 D_8025C380;
extern u16 globalpicnum;
extern smokeinstance_t smoke[MAXEXPLOSIONS];
extern explosioninstance_t explosions[MAXEXPLOSIONS];
extern s16 D_8021DCAC;
extern s16 D_8021DCAE;
extern s16 D_8021DCB0;

extern u16 explosiontris[20][3];
extern explosioninfo_t explosioninfo[9][2];
extern boardinfo_t boardinfo[];
extern f32 globalxpos;
extern f32 globalypos;
extern f32 globalzpos;
extern s32 D_800DA730;

u8 *loadtile(u16 tileid);
u8 tilemasks(u16 tileid);
u8 tilemaskt(u16 tileid);
void resetexplosions(void);
void func_80066DF4(s32 x1, s32 y1, s32 x2, s32 y2);
void func_800637C8(void);

void loadboard(int boardnum)
{
    s32 size;
    resetmovesectorinfo();
    resetboardtimer();
    globalboardnum = boardnum;
    if (boardinfo[boardnum].numwalls)
    {
    }
    size = boardinfo[boardnum].rom_end - boardinfo[boardnum].rom_start;
    copyfromrom((char *)TEMP_AREA_ADDR, boardinfo[boardnum].rom_start, size);
    numsectors = boardinfo[boardnum].numsectors;
    numsprites = boardinfo[boardnum].numsprites;
    numwalls = boardinfo[boardnum].numwalls;
    if (numsprites)
    {
        printf("SPRITES %d\n", numsprites);
    }
    if (numsectors)
    {
        printf("SECTORS %d\n", numsectors);
    }
    if (numwalls)
    {
        printf("WALLS %d\n", numwalls);
    }
    cameradist = 0;
    cameraclock = totalclock;
    sky_re = boardinfo[boardnum].sky_re;
    sky_r = boardinfo[boardnum].sky_r;
    sky_ge = boardinfo[boardnum].sky_ge;
    sky_g = boardinfo[boardnum].sky_g;
    sky_be = boardinfo[boardnum].sky_be;
    sky_b = boardinfo[boardnum].sky_b;
    func_800B4C80();
    clearbufbyte(Sound, 0, sizeof(Sound));
    clearbufbyte(SoundOwner, 0, sizeof(SoundOwner));
    resetexplosions();
}

/*TODO*/
static const char D_801048F4[] = "Set MAXSPRITES to %d\n";
static const char D_8010490C[] = "Set MAXSECTORS to %d\n";
static const char D_80104924[] = "Set MAXWALLS to %d\n";
static const char D_80104938[] = "%d %d %d\n";
static const char D_80104944[] = "Starting decrunch\n";
static const char D_80104958[] = "From %x\n To %x\n";
static const char D_80104968[] = "Failed to decrunch sectors\n";
static const char D_80104984[] = "Decruned sectors\n";
static const char D_80104998[] = "Failed to decrunch walls\n";
static const char D_801049B4[] = "Decruned walls\n";
static const char D_801049C4[] = "Failed to decrunch sprites\n";
static const char D_801049E0[] = "Decruned sprites\n";
static const char D_801049F4[] = "Failed to decrunch vtxs\n";
static const char D_80104A10[] = "Decruned vtxs\n";
static const char D_80104A20[] = "%d %d %d\n";
static const char D_80104A2C[] = "Vert memory = %dk\n";
static const char D_80104A40[] = "Vertex data size = %dk\n";
static const char D_80104A58[] = "Gfx memory = %dk\n";
static const char D_80104A6C[] = "Done this bit!\n";
static const char D_80104A7C[] = "Draw Boss\n";
static const char D_80104A88[] = "Render mode %d\n";
static const char D_80104A98[] = "Combine mode %d\n";
static const char D_80104AAC[] = "Maximum number of displayed sprites reached (256)\n";

void func_8005B720(void)
{
}

void func_8005B728(void)
{
    s32 count, i;
    void *sector_addr;

    globalxpos = boardinfo[globalboardnum].xpos;
    globalypos = boardinfo[globalboardnum].ypos;
    globalzpos = boardinfo[globalboardnum].zpos;
    ps->posx = (globalxpos * 2);
    ps->posy = (globalypos * 2);
    ps->posz = (globalzpos * 32);
    ps->ang = boardinfo[globalboardnum].ang;
    ps->cursectnum = 4;
    ps->over_shoulder_on = 0;
    sector_addr = boardinfo[globalboardnum].sector_addr;
    RNCDecompress(sector_addr, (char *)sector);
    RNCDecompress(boardinfo[globalboardnum].wall_addr, (char *)wall);
    RNCDecompress(boardinfo[globalboardnum].sprite_addr, (char *)sprite);
    RNCDecompress((char *)TEMP_AREA_ADDR, (char *)VERTEXBASE);
    count = 0;
    for (i = 0; i < numsectors; i++)
    {
        count += sector[i].floorvtxcount;
        count += sector[i].ceilingvtxcount;
    }

    count *= 3;
    count = ALIGN((count * sizeof(vertex_t)), 64);
    D_8021D700 = count + (char *)VERTEXBASE;
    initcache((char *)D_8021D700, ZBUFFER_ADDR - D_8021D700);
    func_800637C8();
    func_8005B720();
}

void func_8005B92C(void)
{
}

void func_8005B934(void)
{
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/display_game.s")

void func_8005BB48(void)
{
    viewport[0].vp.vtrans[0] = SCREEN_WIDTH * 2;
    viewport[0].vp.vtrans[1] = SCREEN_HEIGHT * 2;
}

void func_8005BB64(Vp *vp)
{
    viewportscalex = (vp->vp.vscale[0] / 4);
    viewportscaley = (vp->vp.vscale[1] / 4);
    viewporttransx = (vp->vp.vtrans[0] / 4);
    viewporttransy = (vp->vp.vtrans[1] / 4);
    if (numplayers < 3)
        D_8025C380 = 1.0f;
    else
        D_8025C380 = 0.75f;
}

void func_8005BC30(u8 arg0)
{
    switch (numplayers)
    {
    case 1:
        gSPViewport(pkt++, &viewport[7]);
        gDPSetScissor(pkt++, G_SC_NON_INTERLACE, 22, 16, 298, 224);
        func_8005BB64(&viewport[7]);
        break;
    case 2:
        switch (arg0)
        {
        case 0:
            gSPViewport(pkt++, &viewport[5]);
            gDPSetScissor(pkt++, G_SC_NON_INTERLACE, 22, 16, 298, 119);
            func_8005BB64(&viewport[5]);
            break;
        case 1:
            gSPViewport(pkt++, &viewport[6]);
            gDPSetScissor(pkt++, G_SC_NON_INTERLACE, 22, 121, 298, 224);
            func_8005BB64(&viewport[6]);
            break;
        }
        break;
    case 3:
    case 4:
        switch (arg0)
        {
        case 0:
            gSPViewport(pkt++, &viewport[1]);
            gDPSetScissor(pkt++, G_SC_NON_INTERLACE, 22, 16, 159, 119);
            func_8005BB64(&viewport[1]);
            break;
        case 1:
            gSPViewport(pkt++, &viewport[2]);
            gDPSetScissor(pkt++, G_SC_NON_INTERLACE, 161, 16, 298, 119);
            func_8005BB64(&viewport[2]);
            break;
        case 2:
            gSPViewport(pkt++, &viewport[3]);
            gDPSetScissor(pkt++, G_SC_NON_INTERLACE, 22, 121, 159, 224);
            func_8005BB64(&viewport[3]);
            break;
        case 3:
            gSPViewport(pkt++, &viewport[4]);
            gDPSetScissor(pkt++, G_SC_NON_INTERLACE, 161, 121, 298, 224);
            func_8005BB64(&viewport[4]);
            break;
        }
        break;
    }
}

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

void func_8005C390(s32 arg0)
{
    for (; arg0 >= 11; arg0 -= 11)
        ;
    for (; arg0 < 0; arg0 += 11)
        ;
    switch (arg0)
    {
    case 0:
        gDPSetRenderMode(pkt++, G_RM_ZB_OPA_SURF, G_RM_ZB_OPA_SURF2);
        break;
    case 1:
        gDPSetRenderMode(pkt++, G_RM_ZB_XLU_SURF, G_RM_ZB_XLU_SURF2);
        break;
    case 2:
        gDPSetRenderMode(pkt++, G_RM_ZB_OPA_DECAL, G_RM_ZB_OPA_DECAL2);
        break;
    case 3:
        gDPSetRenderMode(pkt++, G_RM_ZB_XLU_DECAL, G_RM_ZB_XLU_DECAL2);
        break;
    case 8:
        gDPSetRenderMode(pkt++, G_RM_AA_ZB_TEX_EDGE, G_RM_AA_ZB_TEX_EDGE2);
    default:
        break;
    }
}

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

void settexture(s16 tilenum)
{
    u16 _tilenum = tilenum;

    if (D_8021D67E == tilenum)
        return;

    tilenum = gettileid(tilenum);
    if (tileinfo[tilenum].picanm & 192)
        tilenum += animateoffs(_tilenum, 0);

    gDPLoadTextureBlock_4b(pkt++, loadtile(tilenum) + 32, G_IM_FMT_I,
                           tileinfo[tilenum].dimx, tileinfo[tilenum].dimy, 0, 0, 0,
                           tilemasks(tilenum), tilemaskt(tilenum), 0, 0);

    D_8021D67E = _tilenum;
    D_8021E4CC++;
}

void func_80061FC8(s16 tileid)
{
    if (tileid != D_8026EC00)
    {
        gDPLoadTLUT_pal16(pkt++, 0, loadtile(tileid));
        gDPLoadTextureBlock_4b(pkt++, loadtile(tileid) + 32, G_IM_FMT_CI,
                               tileinfo[tileid].dimx, tileinfo[tileid].dimy, 0,
                               G_TX_NOMIRROR | G_TX_CLAMP, G_TX_NOMIRROR | G_TX_CLAMP,
                               0, 0, 0, 0);
        D_8021E4CC++;
        D_8026EC00 = tileid;
    }
}

void func_800622E0(void)
{
    D_8021E4CC = 0;
    D_8021DCAC = 0;
    D_8021DCAE = 0;
    D_8021DCB0 = 0;
}

void func_80062304(void)
{
}

void func_8006230C(s32 arg0)
{
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062314.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800624B4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062680.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062804.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062AD0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80062D98.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063168.s")

void func_80063270(void)
{
    drawrooms_(globalposx, globalposy, globalposz, -globalang, globalsectnum);
}

void func_800632BC(s32, s32);
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800632BC.s")

void func_80063470(s32, s32);
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063470.s")

void func_80063594(s32, s32);
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063594.s")

void func_800636B8(void)
{
    func_800632BC(0, drawwallcnt - 1);
    func_80063470(0, drawfloorcnt - 1);
    func_80063594(0, drawceilcnt - 1);
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

void func_80063750(void);
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063750.s")

void func_800637C8(void)
{
    s32 i;

    func_80063750();
    for (i = 0; i < 2851; i++)
    {
        tileinfo[i].ramaddr = NULL;
        D_8025F398[i] = 0;
    }

    D_800DA730 = 0;
    if (globalboardnum == 27)
        func_80056150();
}

u8 *loadtile(u16 tileid)
{
    u8 i;
    u16 size;
    u8 *ptr;
    s32 romAddr;
    u8 val;

    if (tileinfo[tileid].ramaddr != NULL)
    {
        D_8025F398[tileid] = 0x82;
        return tileinfo[tileid].ramaddr;
    }
    if ((tileinfo[tileid].flags & 0x80) == 0x80)
    {
        size = (tileinfo[tileid].dimx * tileinfo[tileid].dimy);
    }
    else
    {
        size = (((tileinfo[tileid].dimx * tileinfo[tileid].dimy) / 2) + 0x20);
    }

    if (size < 0)
    {
    }

    D_8025F398[tileid] = 0x82;
    allocache2((long *)&tileinfo[tileid].ramaddr, size, &D_8025F398[tileid]);
    romAddr = D_800D9AEC;
    romAddr += tileinfo[tileid].romoff;
    copyfromrom(D_8025FEC0, romAddr, (tileinfo[tileid].memsiz + 1) & ~1);
    if (RNCDecompress(D_8025FEC0, tileinfo[tileid].ramaddr) == -1)
    {
        fastmemcpy(tileinfo[tileid].ramaddr, D_8025FEC0, size);
    }
    if ((tileinfo[tileid].flags & 0x80) != 0x80)
    {
        ptr = tileinfo[tileid].ramaddr;
        for (i = 0; i < 16; i++)
        {
            val = *ptr;
            *ptr = *(ptr + 1);
            *(ptr + 1) = val;
            ptr += 2;
        }
    }
    return tileinfo[tileid].ramaddr;
}

u8 tilemasks(u16 tileid)
{
    switch (tileinfo[tileid].dimx)
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

u8 tilemaskt(u16 tileid)
{
    switch (tileinfo[tileid].dimy)
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
    s32 x_;
    char *ptr;

    x_ = x;

    if (x_ == -1)
    {
        for (ptr = string, x = 0; ((*ptr != 0) && (*ptr != '\n') && (*ptr != '\r')); ptr++)
        {
            if (*ptr == ' ')
                x += 7;
            else
                x += tileinfo[gettileid((*ptr + 2789))].tilesizx;
        }
        x_ = (SCREEN_WIDTH / 2) - x / 2;
    }

    for (; ((*string != 0) && (*string != '\n') && (*string != '\r')); string++)
    {
        rotatesprite_simple(x_, y, *string + (2822 - '!'));
        if (*string == ' ')
            x_ += 7;
        else
            x_ += tileinfo[gettileid((*string + 2789))].tilesizx;
    }
}

void menutext(s16 x, s16 y, char *string)
{
    s32 x_;
    char *ptr;
    s16 tilenum;

    x_ = x;

    if (x_ == -1)
    {
        for (ptr = string, x = 0; ((*ptr != 0) && (*ptr != '\n') && (*ptr != '\r')); ptr++)
        {
            tilenum = *ptr;
            if (tilenum >= '0' && tilenum <= '9')
                tilenum += 2882;
            if (tilenum >= 'A' && tilenum <= 'Z')
                tilenum += 2875;
            if (tilenum == '-')
                tilenum = 2929;
            if (tilenum == '.')
                tilenum = 3002;
            if (tilenum == ',')
                tilenum = 3003;
            if (tilenum == '\'')
                tilenum = 3022;
            if (tilenum == ' ')
                x += 12;
            else
                x += tileinfo[gettileid(tilenum)].tilesizx;
        }
        x_ = (SCREEN_WIDTH / 2) - x / 2;
    }

    for (; ((*string != 0) && (*string != '\n') && (*string != '\r')); string++)
    {
        tilenum = *string;
        if (tilenum >= '0' && tilenum <= '9')
            tilenum += 2882;
        if (tilenum >= 'A' && tilenum <= 'Z')
            tilenum += 2875;
        if (tilenum == '-')
            tilenum = 2929;
        if (tilenum == '.')
            tilenum = 3002;
        if (tilenum == ',')
            tilenum = 3003;
        if (tilenum == '\'')
            tilenum = 3022;
        if (tilenum == ' ')
            x_ += 12;
        else
        {
            rotatesprite_simple(x_, y, tilenum);
            x_ += tileinfo[gettileid(tilenum)].tilesizx;
        }
    }
}

void func_80064418(s16 x, s16 y, char *string)
{
    s32 x_;
    char *ptr;
    s16 tilenum;

    x_ = x;

    if (x_ == -1)
    {
        for (ptr = string, x = 0; ((*ptr != 0) && (*ptr != '\n') && (*ptr != '\r')); ptr++)
        {
            tilenum = *ptr;
            if (tilenum >= '0' && tilenum <= '9')
                tilenum += 2944;
            if (tilenum >= 'A' && tilenum <= 'Z')
                tilenum += 2901;

            x += tileinfo[gettileid(tilenum)].tilesizx;
        }
        x_ = (SCREEN_WIDTH / 2) - x / 2;
    }

    for (; ((*string != 0) && (*string != '\n') && (*string != '\r')); string++)
    {
        tilenum = *string;
        if (tilenum >= '0' && tilenum <= '9')
            tilenum += 2944;
        if (tilenum >= 'A' && tilenum <= 'Z')
            tilenum += 2901;

        rotatesprite_simple(x_, y, tilenum);
        x_ += tileinfo[gettileid(tilenum)].tilesizx;
    }
}

void func_80064644(void)
{
    s32 i, j;

    for (i = 0; i != SCREEN_HEIGHT; i++)
    {
        for (j = 0; j != SCREEN_WIDTH; j++)
            ;
    }
}

void func_80064674(void)
{
    initcache((s32)VERTEXBASE, (s32)ZBUFFER_ADDR - (s32)VERTEXBASE);
    func_800637C8();
}

void display_tileviewer(void)
{
    char buffer[260];

    gDPPipeSync(pkt++);
    guTranslate(&viewinfo->_f_300, 0.0f, 0.0f, 0.0f);
    guScale(&viewinfo->_f_380, 0.5f, 0.5f, 0.5f);

    gSPMatrix(pkt++, OS_K0_TO_PHYSICAL(&viewinfo->_f_300), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
    gSPMatrix(pkt++, OS_K0_TO_PHYSICAL(&viewinfo->_f_380), G_MTX_NOPUSH | G_MTX_MUL | G_MTX_MODELVIEW);

    gSPDisplayList(pkt++, &dl_2);
    clearscreen();

    gDPSetScissor(pkt++, G_SC_NON_INTERLACE, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    sprintf(buffer, "picnum %d\n", globalpicnum);
    gametext(20, 20, buffer);
    rotatesprite_simplest(160, 220, globalpicnum);
    if (control_button[0] & CONT_LEFT)
        globalpicnum--;

    if (control_button[0] & CONT_RIGHT)
        globalpicnum++;
}

void cacheandtileinit(void)
{
    initcache((s32)VERTEXBASE, (s32)ZBUFFER_ADDR - (s32)VERTEXBASE);
    func_800637C8();
}

void addsmoke(short x, short y, short z, short type)
{
    s16 i;

    if (numplayers != 1)
        return;

    for (i = 0; i < MAXEXPLOSIONS; i++)
    {
        if (smoke[i].status == 0)
            goto label;
    }
    return;

label:
    smoke[i].type = type;
    smoke[i].z = z;
    smoke[i].scale = 40.0f;
    smoke[i].x = x;
    smoke[i].y = y;
    smoke[i].phase = 10.0f;
    smoke[i].phase_step = 0.1f;
    smoke[i].orientation = krand2();
    smoke[i].status = 1;
}

void addexplosionmodel(short x, short y, short z, char type)
{
    s16 i;

    if (numplayers != 1)
        return;

    for (i = 0; i < MAXEXPLOSIONS; i++)
    {
        if (explosions[i].status == 0)
            goto label;
    }
    return;

label:
    explosions[i].type = type;
    explosions[i].x = x;
    explosions[i].y = y;
    explosions[i].z = z;
    explosions[i].status = 1;
    explosions[i].c_size = explosioninfo[type][0].startsize;
    explosions[i].r_size = explosioninfo[type][1].startsize;
    explosions[i].c_time = explosioninfo[type][0].time;
    explosions[i].r_time = explosioninfo[type][1].time;
    explosions[i].c_step = explosioninfo[type][0].step;
    explosions[i].r_step = explosioninfo[type][1].step;
    explosions[i].c_enable = explosioninfo[type][0].enable;
    explosions[i].r_enable = explosioninfo[type][1].enable;
}

void resetexplosions(void)
{
    s32 i;
    for (i = 0; i < 16; i++)
        smoke[i].status = 0;

    for (i = 0; i < 16; i++)
        explosions[i].status = 0;
}

void displayexplosion(s16 index, u8 bottom)
{
    u16 i, type;

    type = explosions[index].type;
    if (explosions[index].c_enable == 1)
    {
        settexture(3921);
        gDPSetPrimColor(pkt++, 0, 0, explosioninfo[type][0].color1[0],
                        explosioninfo[type][0].color1[1], explosioninfo[type][0].color1[2], 0xFF);
        gDPSetEnvColor(pkt++, explosioninfo[type][0].color2[0],
                       explosioninfo[type][0].color2[1], explosioninfo[type][0].color2[2], 0xFF);

        for (i = 0; i < 20; i++)
            gSP1Triangle(pkt++, explosiontris[i][0], explosiontris[i][1], explosiontris[i][2], 0);
    }
    if ((explosions[index].r_enable == 1) && (bottom == 0))
    {
        settexture(3674);
        gDPSetPrimColor(pkt++, 0, 0, explosioninfo[type][1].color1[0],
                        explosioninfo[type][1].color1[1], explosioninfo[type][1].color1[2], 0xFF);
        gDPSetEnvColor(pkt++, explosioninfo[type][1].color2[0],
                       explosioninfo[type][1].color2[1], explosioninfo[type][1].color2[2], 0xFF);
        gSP1Triangle(pkt++, 12, 13, 14, 12);
        gSP1Triangle(pkt++, 12, 14, 15, 12);
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064D28.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800651C0.s")

void displayexplosions(void)
{
    f32 x, y, z;
    f32 sx, sy, sz, sw;
    f32 tempx, tempy;
    f32 scale;
    s32 alpha;
    s16 i;

    gSPClearGeometryMode(pkt++, G_SHADE | G_CULL_BOTH | G_FOG | G_LIGHTING | G_TEXTURE_GEN |
                                    G_TEXTURE_GEN_LINEAR | G_LOD | G_SHADING_SMOOTH);
    gSPSetGeometryMode(pkt++, G_ZBUFFER | G_SHADE | G_SHADING_SMOOTH);
    gDPSetRenderMode(pkt++, G_RM_AA_ZB_XLU_SURF, G_RM_AA_ZB_XLU_SURF2);
    gDPSetColorDither(pkt++, G_CD_NOISE);
    gDPSetAlphaCompare(pkt++, G_AC_NONE);
    gDPSetTextureLUT(pkt++, G_TT_NONE);
    gDPSetCombineMode(pkt++, G_CC_BLENDPE, G_CC_BLENDPE);
    gDPSetDepthSource(pkt++, G_ZS_PRIM);
    gDPSetTexturePersp(pkt++, G_TP_NONE);
    D_8026EC31 = 0;
    gDPSetCombineMode(pkt++, G_CC_MODULATEIA_PRIM, G_CC_MODULATEIA_PRIM);
    gDPSetPrimColor(pkt++, 0, 0, 0xFF, 0xFF, 0xFF, 0xFF);
    setfxcolor(0xFF, 0xFF, 0xFF, 0, 0, 0);
    gDPSetColorDither(pkt++, G_CD_NOISE);
    gDPSetAlphaDither(pkt++, G_AD_NOISE);

    for (i = 0; i < 16; i++)
    {
        if (smoke[i].status == 0)
            continue;

        x = (smoke[i].x / 2);
        y = (smoke[i].y / 2);
        z = (smoke[i].z / 2);
        sx = ((projmatrix[0][0] * x) + (projmatrix[1][0] * y) + (projmatrix[2][0] * z) + projmatrix[3][0]);
        tempy = sy / sw; // FAKE
        sy = ((projmatrix[0][1] * x) + (projmatrix[1][1] * y) + (projmatrix[2][1] * z) + projmatrix[3][1]);
        sz = ((projmatrix[0][2] * x) + (projmatrix[1][2] * y) + (projmatrix[2][2] * z) + projmatrix[3][2]);
        sw = ((projmatrix[0][3] * x) + (projmatrix[1][3] * y) + (projmatrix[2][3] * z) + projmatrix[3][3]);
        if (sw == 0.0f)
            continue;

        tempx = sx / sw;
        if (tempx < -2.0f)
            continue;

        tempy = sy / sw;
        if ((tempy < -2.0f) || (tempx > 2.0f) || (tempy > 2.0f))
            continue;

        if (((sz / sw) < 0.0f) || ((sz / sw) > 1.0f))
            continue;

        gDPSetPrimDepth(pkt++, ((((sz / sw) * 511.0f) + 511.0f) * 32.0f), 0);

        sz = 1.0f - (sz / sw);
        scale = smoke[i].scale;
        alpha = (smoke[i].phase * 25.0f);
        if (smoke[i].type == 0)
        {
            gDPSetPrimColor(pkt++, 0, 0, 200, 200, 200, alpha);
            gDPSetEnvColor(pkt++, 0, 0, 0, alpha);
        }
        else
        {
            gDPSetPrimColor(pkt++, 0, 0, 0, 0, 0, alpha);
            gDPSetEnvColor(pkt++, 0, 0, 0, alpha);
        }

        rendertileworld((tempx * viewportscalex) + viewporttransx,
                        (-tempy * viewportscaley) + viewporttransy,
                        scale * sz * 4.0f, scale * sz * 4.0f,
                        gettileid(3658), smoke[i].orientation);
    }
    clearscreen();
    gDPSetDepthSource(pkt++, G_ZS_PIXEL);
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/rotatesprite_simple.s")

void animateexplosions(void)
{
    s16 i;

    for (i = 0; i < MAXEXPLOSIONS; i++)
    {
        if (explosions[i].status != 0)
        {
            explosions[i].r_size += explosions[i].r_step;
            explosions[i].c_size += explosions[i].c_step;
            explosions[i].r_step -= explosioninfo[explosions[i].type][1].stepdec;
            explosions[i].c_step -= explosioninfo[explosions[i].type][0].stepdec;
            explosions[i].c_time--;
            explosions[i].r_time--;
            if (explosions[i].c_time == 0)
                explosions[i].c_enable = 0;

            if (explosions[i].r_time == 0)
                explosions[i].r_enable = 0;

            if ((explosions[i].c_enable + explosions[i].r_enable) == 0)
                explosions[i].status = 0;
        }
    }
}

void animatesmoke(void)
{
    s16 i;

    for (i = 0; i < MAXEXPLOSIONS; i++)
    {
        if (smoke[i].status != 0)
        {
            smoke[i].scale += smoke[i].phase;
            smoke[i].phase -= smoke[i].phase_step;
            smoke[i].z -= smoke[i].phase;
            if (smoke[i].phase <= 0.0f)
                smoke[i].status = 0;
        }
    }
}

void animatemodels(void)
{
    if (globalboardnum == 27)
        animateboss2();

    animateexplosions();
    animatesmoke();
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066324.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066718.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006693C.s")

void func_80066BE0(s32 arg0)
{
    s32 i;
    u16 index;
    s32 x1, y1, x2, y2;

    if (arg0 != 0)
        D_8026EB94 = (*FRAMEBUFFER_ADDR)[0];
    else
        D_8026EB94 = (*FRAMEBUFFER_ADDR)[1];

    for (i = 0; i < drawwallcnt; i++)
    {
        index = drawwalllist[i];

        x1 = wall[index].x - globalposx;
        y1 = wall[index].y - globalposy;
        x2 = wall[wall[index].point2].x - globalposx;
        y2 = wall[wall[index].point2].y - globalposy;

        x1 >>= 6;
        y1 >>= 6;
        x2 >>= 6;
        y2 >>= 6;

        rotatepoint(0, 0, x1, y1, (-(globalang + 0x200) & 0x7FF), &x1, &y1);
        rotatepoint(0, 0, x2, y2, (-(globalang + 0x200) & 0x7FF), &x2, &y2);
        x1 += SCREEN_WIDTH / 2;
        y1 += SCREEN_HEIGHT / 2;
        x2 += SCREEN_WIDTH / 2;
        y2 += SCREEN_HEIGHT / 2;
        if (wall[index].nextwall == -1)
            D_8026EB98 = 0xFFFF;
        else
            D_8026EB98 = 0x8421;
        func_80066DF4(x1, y1, x2, y2);
    }
}

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066DF4.s")

void func_800674EC(s32 x, s32 y, s32 arg2)
{
    if (arg2 != 0)
    {
        if ((y > 0) && (y < SCREEN_WIDTH - 1) && (x > 0) && (x < SCREEN_HEIGHT - 1))
            D_8026EB94[y + x * SCREEN_WIDTH] = D_8026EB98;
    }
    else if ((x > 0) && (x < SCREEN_WIDTH - 1) && (y > 0) && (y < SCREEN_HEIGHT - 1))
    {
        D_8026EB94[x + y * SCREEN_WIDTH] = D_8026EB98;
    }
}
