#include "common.h"
#include "duke3d.h"

void func_8004D248(u8 arg0);

s32 D_800D38B0 = 0;
signed char doinitrumble = 0;
s8 D_800D38B8 = 0;
s32 D_800D38BC = 0;
s8 D_800D38C0 = 0;
s32 D_800D38C4 = 0;

OSMesgQueue D_801CB8E0;
unkstruct1_t D_801CB8F8[4];

void func_80051850(void)
{
    D_800D38B0 = 1;
    osCreateMesgQueue(&D_801CB8E0, &D_801CB8D8, 1);
    osSendMesg(&D_801CB8E0, NULL, OS_MESG_NOBLOCK);
}

void func_800518A0(void)
{
    OSMesg msg;

    if (D_800D38B0 == 0)
    {
        func_80051850();
    }
    osRecvMesg(&D_801CB8E0, &msg, OS_MESG_BLOCK);
}

void func_800518E4(void)
{
    osSendMesg(&D_801CB8E0, NULL, OS_MESG_BLOCK);
}

void controllervibdisable(void)
{
    u16 i;

    for (i = 0; i < 4; i++)
    {
        D_801CB8F8[i].unk0 = 0;
        D_801CB8F8[i].unk1 = 0;
        D_801CB8F8[i].unk2 = 0;
    }
}

void controllersetvib(u8 arg0, u16 arg1, u8 arg2)
{
    if ((arg0 < numplayers) && (game_inactive == 0) && (gamestate == 2))
    {
        if (D_801CB8F8[arg0].unk0 != 0)
        {
            D_801CB8F8[arg0].unk2 = arg1 * 2;
            D_801CB8F8[arg0].unk4 = arg2;
        }
    }
}

void func_800519C4(u8 arg0)
{
    D_801CB8F8[arg0].unk2 = 0;
}

void controllervibstopall(void)
{
    u16 i;

    for (i = 0; i < 4; i++)
    {
        func_800519C4(i);
    }
}

void func_80051A20(s32 arg0)
{
    if (osMotorStart(&D_801CB728[arg0]) != PFS_ERR_CONTRFAIL)
    {
        D_801CB8F8[arg0].unk1 = 5;
    }
}

void func_80051A80(s32 arg0)
{
    s32 ret;
    ret = osMotorStop(&D_801CB728[arg0]);
    printf("osMotorStop ret %d\n", ret);
    if (ret != PFS_ERR_CONTRFAIL)
    {
        D_801CB8F8[arg0].unk1--;
        if (D_801CB8F8[arg0].unk1 < 0)
        {
            D_801CB8F8[arg0].unk1 = 0;
        }
    }
}

void updatecontrollers(void)
{
    u16 i;

    func_800518A0();
    if (D_801C92F0.validCount != 0)
    {
        osRecvMesg(&D_801C92F0, NULL, OS_MESG_BLOCK);
        osContGetReadData(D_801C9328);
        func_8004D248(0);
        func_8004D248(1);
        func_8004D248(2);
        func_8004D248(3);
    }

    for (i = 0; i < 4; i++)
    {
        if (D_801CB8F8[i].unk0 != 0)
        {
            if (D_801CB8F8[i].unk2 <= 0)
            {
                if (D_801CB8F8[i].unk1 != 0)
                {
                    func_80051A80(i);
                }
            }
            else
            {
                D_801CB8F8[i].unk6 -= D_801CB8F8[i].unk4;
                if (D_801CB8F8[i].unk6 < 0)
                {
                    if (D_801CB8F8[i].unk1 != 5)
                    {
                        func_80051A20(i);
                    }
                    D_801CB8F8[i].unk6 += 256;
                }
                else if (D_801CB8F8[i].unk1 != 0)
                {
                    func_80051A80(i);
                }
                D_801CB8F8[i].unk2--;
            }
        }
    }
    if (D_800D38B8 != 0)
    {
        D_800D38BC = osPfsInitPak(&D_801C92F0, &D_801C9350, D_800D3748);
        D_800D38B8 = 0;
    }
    if (D_800D38C0 != 0)
    {
        D_800D38C4 = osMotorInit(&D_801C92F0, &D_801C9350, D_800D3748);
        D_800D38C0 = 0;
    }

    if (doinitrumble != 0)
    {
        for (i = 0; i < 4; i++)
        {
            if ((D_801CB8F8[i].unk0 != 0) && (osMotorInit(&D_801C92F0, &D_801CB728[i], i) == 0))
            {
                D_801CB8F8[i].unk2 = 0;
                D_801CB8F8[i].unk1 = 0;
                func_80051A80(i);
            }
        }
        doinitrumble = 0;
    }
    osContStartReadData(&D_801C92F0);
    func_800518E4();
}
