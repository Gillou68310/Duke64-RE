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
} mapinfo_t;

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
extern u16 D_8026EC00;
extern s16 tilemap[MAXTILES];
extern s32 D_800D9AEC;
extern u8 D_8025F398[];
extern char D_8025FEC0[];
extern s32 globalmapnum;
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
extern s8 D_8026EC31;
extern u16 globalpicnum;

extern mapinfo_t mapinfo[];
extern f32 D_800DA490;
extern f32 D_800DA494;
extern f32 D_800DA498;

u8 *loadtile(u16 tileid);
u8 tilemasks(u16 tileid);
u8 tilemaskt(u16 tileid);
void func_80064A54(void);
void func_80066DF4(s32 x1, s32 y1, s32 x2, s32 y2);
void func_800637C8(void);

void func_8005B5F0(int mapnum)
{
    s32 size;
    resetmovesectorinfo();
    resetboardtimer();
    globalmapnum = mapnum;
    if (mapinfo[mapnum].numwalls)
    {
    }
    size = mapinfo[mapnum].rom_end - mapinfo[mapnum].rom_start;
    copyfromrom((char *)TEMP_AREA_ADDR, mapinfo[mapnum].rom_start, size);
    numsectors = mapinfo[mapnum].numsectors;
    numsprites = mapinfo[mapnum].numsprites;
    numwalls = mapinfo[mapnum].numwalls;
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
    sky_re = mapinfo[mapnum].sky_re;
    sky_r = mapinfo[mapnum].sky_r;
    sky_ge = mapinfo[mapnum].sky_ge;
    sky_g = mapinfo[mapnum].sky_g;
    sky_be = mapinfo[mapnum].sky_be;
    sky_b = mapinfo[mapnum].sky_b;
    func_800B4C80();
    clearbufbyte(Sound, 0, sizeof(Sound));
    clearbufbyte(SoundOwner, 0, sizeof(SoundOwner));
    func_80064A54();
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

    D_800DA490 = mapinfo[globalmapnum].xpos;
    D_800DA494 = mapinfo[globalmapnum].ypos;
    D_800DA498 = mapinfo[globalmapnum].zpos;
    ps->posx = (D_800DA490 * 2);
    ps->posy = (D_800DA494 * 2);
    ps->posz = (D_800DA498 * 32);
    ps->ang = mapinfo[globalmapnum].ang;
    ps->cursectnum = 4;
    ps->over_shoulder_on = 0;
    sector_addr = mapinfo[globalmapnum].sector_addr;
    RNCDecompress(sector_addr, (char *)sector);
    RNCDecompress(mapinfo[globalmapnum].wall_addr, (char *)wall);
    RNCDecompress(mapinfo[globalmapnum].sprite_addr, (char *)sprite);
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

void func_80061CB8(s16 tilenum)
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

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800622E0.s")

void func_80062304(void)
{
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

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80063750.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800637C8.s")

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
        x_ = (SCREEN_WIDTH / 2) - x / 2;
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

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/addexplosionmodel2.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/addexplosionmodel.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064A54.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064AA4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80064D28.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_800651C0.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80065458.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/rotatesprite_simple.s")

void func_80066170(void);
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066170.s")

void func_80066250(void);
#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_80066250.s")

void animatemodels(void)
{
    if (globalmapnum == 27)
        func_80056400();

    func_80066170();
    func_80066250();
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

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/wallcalcslope.s")

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

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/wallcalcnoslope.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006A310.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006A62C.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006ADA4.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006C2FC.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/func_8006CB30.s")

#pragma GLOBAL_ASM("asm/nonmatchings/src/rend2/rendertileworld.s")

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
