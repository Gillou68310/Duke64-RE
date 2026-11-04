#include "common.h"
#include "duke3d.h"

s32 D_800D4D80 = 0;
u8 D_800D4D84[4] = {0};

OSMesgQueue D_8021A060;
OSMesg D_8021A078[32];
unk8021A0F8 D_8021A0F8[32];
u8 D_8021A3F8;

void soundSetupMsgQueue(void)
{
    s32 i;

    for (i = 0; i < 32; i++)
    {
        D_8021A0F8[i].unk0 = 0;
    }
    osCreateMesgQueue(&D_8021A060, D_8021A078, 0x20);
}

s32 func_80058348(void)
{
    s32 i;

    for (i = 0; i < 32; i++)
    {
        if (D_8021A0F8[i].unk0 == 0)
            return i;
    }
    return -1;
}

s32 func_800583C0(void)
{
    s32 i, j;

    j = 0;
    for (i = 0; i < 32; i++)
    {
        if (D_8021A0F8[i].unk0 == 0)
            j++;
    }
    return j;
}

s32 func_80058428(s32 arg0, s32 arg1, s16 arg2, s32 arg3, s32 arg4, s16 arg5, s8 arg6)
{
    s32 i;

    if (game_inactive != 0)
    {
        return -1;
    }
    i = func_80058348();
    if (i == -1)
    {
        return -1;
    }

    D_8021A0F8[i].unk0 = 1;
    D_8021A0F8[i].unk4 = arg0;
    D_8021A0F8[i].unk8 = arg1;
    D_8021A0F8[i].unkC = arg2;
    D_8021A0F8[i].unk10 = arg3;
    if (arg6 == -1)
    {
        D_8021A0F8[i].unk1 = 0;
    }
    else
    {
        D_8021A0F8[i].unk1 = D_800D4D84[arg6];
    }
    D_8021A0F8[i].unk2 = arg5;
    D_8021A0F8[i].unk14 = arg4;
    return osSendMesg(&D_8021A060, &D_8021A0F8[i], OS_MESG_BLOCK);
}

void func_8005850C(s32 arg0)
{
    s32 i;

    if (game_inactive == 0)
    {
        i = func_80058348();
        if (i != -1)
        {
            D_8021A0F8[i].unk0 = 2;
            D_8021A0F8[i].unk14 = arg0;
            osSendMesg(&D_8021A060, &D_8021A0F8[i], OS_MESG_BLOCK);
        }
    }
}

void func_8005857C(void)
{
    s32 i;

    i = func_80058348();
    if (i != -1)
    {
        D_8021A0F8[i].unk0 = 3;
        osSendMesg(&D_8021A060, &D_8021A0F8[i], OS_MESG_BLOCK);
    }
}

void stopsong(void)
{
    s32 i;

    i = func_80058348();
    if (i != -1)
    {
        D_8021A0F8[i].unk0 = 4;
        osSendMesg(&D_8021A060, &D_8021A0F8[i], OS_MESG_BLOCK);
    }
}

void func_8005862C(void)
{
    s32 i;

    i = func_80058348();
    if (i != -1)
    {
        D_8021A0F8[i].unk0 = 6;
        osSendMesg(&D_8021A060, &D_8021A0F8[i], OS_MESG_BLOCK);
    }
}

s32 func_80058684(s16 arg0)
{
    s32 sndang;

    if (D_801012A0 == 0)
    {
        return 0x40;
    }

    sndang = (((ps[D_8021A3F8].ang + ps[D_8021A3F8].look_ang) -
               getangle(ps[D_8021A3F8].oposx - sprite[arg0].x,
                        ps[D_8021A3F8].oposy - sprite[arg0].y)) +
              0xA00);
    sndang &= 0x7ff;
    if (sndang >= 0x400)
    {
        sndang = 0x800 - sndang;
    }
    return sndang >> 3;
}

u8 func_8005878C(s16 arg0, s16 arg1, s32 *arg2)
{
    s32 d1, d2;
    s32 i;

    d1 = 0x4001;
    for (i = 0; i < numplayers; i++)
    {
        d2 = FindDistance3D((ps[i].oposx - sprite[arg0].x),
                            (ps[i].oposy - sprite[arg0].y),
                            (ps[i].oposz - sprite[arg0].z) >> 4);
        if (d2 < d1)
        {
            D_8021A3F8 = i;
            d1 = d2;
        }
    }

    if (1) // FAKE
        ;
    if ((sprite[arg0].picnum == APLAYER) && (D_8021A3F8 == sprite[arg0].yvel))
        *arg2 = 0x40;
    else
        *arg2 = func_80058684(arg0);

    i = d1;
    if ((&sprite[arg0] >= &sprite[0]) && !(soundm[arg1] & 0x10) &&
        (sprite[arg0].picnum == 5) && (sprite[arg0].lotag < 999) &&
        ((sector[sprite[arg0].sectnum].lotag & 0xFF) < 9))
    {
        i = divscale14(d1, sprite[arg0].hitag + 1);
    }

    i += soundvo[arg1];
    if (i < 0)
        i = 0;

    if (soundm[arg1] & 0x10)
        i = 0;

    if ((soundm[arg1] & 4) && (sprite[arg0].yvel < numplayers))
        i = 0;

    switch (arg1)
    {
    case 8:
    case 12:
    case 15:
    case 271:
        if (i > 0x1800)
            i = 0x1800;
        break;
    }

    if (i > 0x4000)
        i = 0x4000;

    i = (0x4000 - i) >> 6;
    if (i > 0xFF)
        i = 0xFF;

    if (i < 0)
        i = 0;

    return i;
}

int func_80058A30(void)
{
    return ++D_800D4D80;
}

int xyzsound(short num, short i, long x, long y, long z)
{
    short pitch;
    short pitche;
    short pitchs;
    short jj;
    u8 sp57;
    s32 sp50;
    long voice;
    s32 unused;
    long j, k;

    if (num > NUM_SOUNDS)
        return -1;

    if (Sound[num].num >= 6)
        return -1;

    if ((soundm[num] & 1) && (Sound[num].num > 0))
        return -1;

    if (soundm[num] & 0x80)
    {
        sound(num);
        return 0;
    }

    if (soundm[num] & 4)
    {
        for (j = 0; j < NUM_SOUNDS; j++)
            for (k = 0; k < Sound[j].num; k++)
                if ((Sound[j].num > 0) && (soundm[j] & 4))
                    return -1;
    }

    sp57 = func_8005878C(i, num, &sp50);
    if (sp57 == 0)
        return -1;

    pitchs = soundps[num];
    pitche = soundpe[num];
    jj = klabs(pitche - pitchs);
    if (jj)
    {
        if (pitchs < pitche)
            pitch = pitchs + (krand2() % jj);
        else
            pitch = pitche + (krand2() % jj);
    }
    else
        pitch = pitchs;

    switch (num)
    {
    case 8:
    case 12:
    case 15:
    case 271:
        if (sector[ps[D_8021A3F8].cursectnum].lotag == 2)
            pitch -= 1024;
        break;
    default:
        if (sector[ps[D_8021A3F8].cursectnum].lotag == 2 && (soundm[num] & 4) == 0)
            pitch = -768;
        break;
    }

    voice = func_80058A30();
    if (func_80058428(num, sp57, i, sp50, voice, pitch, D_8021A3F8) == -1)
    {
        return -1;
    }

    SoundOwner[num][Sound[num].num].i = i;
    SoundOwner[num][Sound[num].num].voice = voice;
    Sound[num].num++;
    return voice;
}

void sound(s16 num)
{
    s16 pitch;
    s16 pitche;
    s16 pitchs;
    s16 jj;
    long j, k;

    if (soundm[num] & 4)
    {
        for (j = 0; j < NUM_SOUNDS; j++)
            for (k = 0; k < Sound[j].num; k++)
                if ((Sound[j].num > 0) && (soundm[j] & 4))
                    return;
    }

    pitchs = soundps[num];
    pitche = soundpe[num];
    jj = klabs(pitche - pitchs);
    if (jj)
    {
        if (pitchs < pitche)
            pitch = pitchs + (krand2() % jj);
        else
            pitch = pitche + (krand2() % jj);
    }
    else
        pitch = pitchs;

    func_80058428(num, 0xFF, -1, 0x40, func_80058A30(), pitch, -1);
}

void func_80058FEC(s16 arg0, u8 arg1)
{
    func_80058428(arg0, arg1, -1, 0x40, func_80058A30(), 0, -1);
}

int spritesound(unsigned short num, short i)
{
    if (num >= NUM_SOUNDS)
        return -1;

    return xyzsound(num, i, sprite[i].x, sprite[i].y, sprite[i].z);
}

void testcallback(long num)
{
    short tempi, tempj, tempk;

    tempk = Sound[num].num;

    if (tempk > 0)
    {
        if ((soundm[num] & 16) == 0)
            for (tempj = 0; tempj < tempk; tempj++)
            {
                tempi = SoundOwner[num][tempj].i;
                if (sprite[tempi].picnum == MUSICANDSFX && sector[sprite[tempi].sectnum].lotag < 3 && sprite[tempi].lotag < 999)
                {
                    hittype[tempi].temp_data[0] = 0;
                    if ((tempj + 1) < tempk)
                    {
                        SoundOwner[num][tempj].voice = SoundOwner[num][tempk - 1].voice;
                        SoundOwner[num][tempj].i = SoundOwner[num][tempk - 1].i;
                    }
                    break;
                }
            }

        Sound[num].num--;
        SoundOwner[num][tempk - 1].i = -1;
    }
}

void stopsound(s16 num)
{

    if (Sound[num].num > 0)
    {
        func_8005850C(SoundOwner[num][Sound[num].num - 1].voice);
        testcallback(num);
    }
}

void stopenvsound(s16 num, s16 arg1)
{
    s16 i;
    s16 num2;

    if (Sound[num].num <= 0)
        return;

    num2 = Sound[num].num;
    for (i = 0; i < num2; i++)
    {
        if (arg1 == SoundOwner[num][i].i)
        {
            func_8005850C(SoundOwner[num][i].voice);
            return;
        }
    }
}

void stopallsounds(void)
{
    s32 i;

    i = func_80058348();
    if (i != -1)
    {
        D_8021A0F8[i].unk0 = 5;
        osSendMesg(&D_8021A060, &D_8021A0F8[i], OS_MESG_BLOCK);
    }
}

s32 func_80059384(s32 arg0)
{
    return 0;
}

void func_80059390(s32 arg0)
{
}

void func_80059398(int arg0, signed char arg1)
{
    arg0 /= 2;
    if (arg1 == -1)
    {
        D_800D4D84[0] = arg0;
        D_800D4D84[1] = arg0;
        D_800D4D84[2] = arg0;
        D_800D4D84[3] = arg0;
    }
    else
        D_800D4D84[arg1] = arg0;
}

void FX_StopSound(int arg0)
{
    func_8005850C(arg0);
}
