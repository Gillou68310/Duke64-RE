#include "common.h"
#include "duke3d.h"

void func_8004D664(s16 playernum);
void func_8004D888(s16 playernum);

s8 D_800D3740[4] = {0};
s8 D_800D3744[4] = {0};
s8 D_800D3748 = -1;
s8 D_800D374C = -1;

OSMesgQueue D_801C92F0;
OSMesg D_801C9308[4];
OSContStatus D_801C9318[4];
OSContPad D_801C9328[4];
s32 D_801C9340;
s32 D_801C9344;
s32 D_801C9348;
OSPfs D_801C9350;
volatile unsigned short control_button[4];
volatile short control_stickx[4];
volatile short control_sticky[4];
controlmap_t controlmap[4];

void func_8004CF80(void)
{
}

s32 initControllers(void)
{
    s32 i;
    s32 one = 1;
    u8 bitpattern;
    s32 unused1;
    s32 ret;
    s32 unused2;
    s32 max_files;
    s32 unused3;
    OSMesgQueue mq;
    OSMesg msg;
    OSPfs pfs;
    u8 pad[24];
    OSContStatus *status = D_801C9318;

    printf("READ PADS\n");
    osCreateMesgQueue(&mq, &msg, 1);
    osSetEventMesg(OS_EVENT_SI, &mq, (OSMesg)1);
    osContInit(&mq, &bitpattern, status);
    osCreateMesgQueue(&D_801C92F0, D_801C9308, 1);
    osSetEventMesg(OS_EVENT_SI, &D_801C92F0, NULL);

    for (i = 0; i < 4; i++)
    {
        printf("Checking pad %d\n", i);
        if ((bitpattern >> i) & 1)
        {
            if ((status[i].type & CONT_JOYPORT) && (status[i].status & CONT_CARD_ON))
            {
                printf("osPfsInitPak %d\n", i);
                ret = osPfsInitPak(&D_801C92F0, &pfs, i);
                switch (ret)
                {
                case 0:
                    printf("Found memory in pad %d\n", i);
                    D_801C9340 = osPfsInitPak(&D_801C92F0, &D_801C9350, i);
                    D_800D3748 = i;
                    osPfsNumFiles(&D_801C9350, &max_files, &D_801C9344);
                    osPfsFreeBlocks(&D_801C9350, &D_801C9348);
                    break;
                case PFS_ERR_ID_FATAL:
                    printf("ID FATAL\n");
                case PFS_ERR_DEVICE:
                    if (osMotorInit(&D_801C92F0, &D_801CB728[i], i) == 0)
                    {
                        printf("Found jolt in pad %d\n", i);
                        D_801CB8F8[i].unk0 = one;
                        func_80051A80(i);
                        break;
                    }
                    if (ret == PFS_ERR_ID_FATAL)
                    {
                        printf("Damaged memory\n");
                        D_800D3748 = -2;
                        D_800D374C = i;
                        break;
                    }
                    printf("Damaged memory\n");
                    D_800D3748 = -2;
                    D_800D374C = i;
                    break;
                }
            }
        }
    }
    if ((bitpattern & 1) && !(status[0].errno & 8))
    {
        D_800D3744[0] = one;
    }
    if ((bitpattern & 2) && !(status[1].errno & 8))
    {
        D_800D3744[1] = one;
    }
    if ((bitpattern & 4) && !(status[2].errno & 8))
    {
        D_800D3744[2] = one;
    }
    if ((bitpattern & 8) && !(status[3].errno & 8))
    {
        D_800D3744[3] = one;
    }
    return -1;
}

void func_8004D248(u8 playernum)
{
    control_button[playernum] = D_801C9328[playernum].button;
    control_stickx[playernum] = D_801C9328[playernum].stick_x;
    control_sticky[playernum] = D_801C9328[playernum].stick_y;
}

void func_8004D2A0(s16 playernum)
{
    controlmap[playernum].key.jump = U_CBUTTONS;
    controlmap[playernum].key.crouch = D_CBUTTONS;
    controlmap[playernum].key.fire = Z_TRIG;
    controlmap[playernum].key.aim_up = U_JPAD;
    controlmap[playernum].key.aim_down = D_JPAD;
    controlmap[playernum].key.prev_weapon = B_BUTTON;
    controlmap[playernum].key.next_weapon = A_BUTTON;
    controlmap[playernum].key.inv_left = L_JPAD;
    controlmap[playernum].key.inv_right = R_JPAD;
    controlmap[playernum].key.open = R_TRIG;
    controlmap[playernum].key.inventory = L_TRIG;
    controlmap[playernum].key.forward = 0;
    controlmap[playernum].key.backward = 0;
    controlmap[playernum].key.strafe_left = L_CBUTTONS;
    controlmap[playernum].key.strafe_right = R_CBUTTONS;
    controlmap[playernum].key.buttonmode = 0;
    if (ps[playernum].left_right_handed != 0)
    {
        func_8004D664(playernum);
    }
}

void func_8004D380(s16 playernum)
{
    controlmap[playernum].key.jump = R_TRIG;
    controlmap[playernum].key.crouch = B_BUTTON;
    controlmap[playernum].key.fire = Z_TRIG;
    controlmap[playernum].key.prev_weapon = L_JPAD;
    controlmap[playernum].key.next_weapon = R_JPAD;
    controlmap[playernum].key.aim_up = 0;
    controlmap[playernum].key.aim_down = 0;
    controlmap[playernum].key.inv_left = U_JPAD;
    controlmap[playernum].key.inv_right = D_JPAD;
    controlmap[playernum].key.open = A_BUTTON;
    controlmap[playernum].key.inventory = L_TRIG;
    controlmap[playernum].key.forward = U_CBUTTONS;
    controlmap[playernum].key.backward = D_CBUTTONS;
    controlmap[playernum].key.strafe_left = L_CBUTTONS;
    controlmap[playernum].key.strafe_right = R_CBUTTONS;
    controlmap[playernum].key.buttonmode = 0;
    if (ps[playernum].left_right_handed != 0)
    {
        func_8004D664(playernum);
    }
}

void func_8004D460(s16 playernum)
{
    controlmap[playernum].key.jump = B_BUTTON;
    controlmap[playernum].key.crouch = A_BUTTON;
    controlmap[playernum].key.fire = 0x20000 | Z_TRIG;
    controlmap[playernum].key.prev_weapon = 0x10000 | L_CBUTTONS;
    controlmap[playernum].key.aim_up = 0;
    controlmap[playernum].key.aim_down = 0;
    controlmap[playernum].key.next_weapon = 0x10000 | R_CBUTTONS;
    controlmap[playernum].key.inv_left = 0x10000 | U_CBUTTONS;
    controlmap[playernum].key.inv_right = 0x10000 | D_CBUTTONS;
    controlmap[playernum].key.open = R_TRIG;
    controlmap[playernum].key.inventory = 0x10000 | Z_TRIG;
    controlmap[playernum].key.forward = 0x20000 | U_CBUTTONS;
    controlmap[playernum].key.backward = 0x20000 | D_CBUTTONS;
    controlmap[playernum].key.strafe_left = 0x20000 | L_CBUTTONS;
    controlmap[playernum].key.strafe_right = 0x20000 | R_CBUTTONS;
    controlmap[playernum].key.buttonmode = 0x10;
    if (ps[playernum].left_right_handed != 0)
    {
        func_8004D664(playernum);
    }
}

void func_8004D568(s16 playernum)
{
    controlmap[playernum].key.jump = 0x20000 | B_BUTTON;
    controlmap[playernum].key.crouch = 0x10000 | B_BUTTON;
    controlmap[playernum].key.fire = Z_TRIG;
    controlmap[playernum].key.next_weapon = 0x10000 | L_CBUTTONS;
    controlmap[playernum].key.aim_up = 0;
    controlmap[playernum].key.aim_down = 0;
    controlmap[playernum].key.prev_weapon = 0;
    controlmap[playernum].key.inv_left = 0;
    controlmap[playernum].key.inv_right = 0x10000 | R_CBUTTONS;
    controlmap[playernum].key.open = 0x20000 | A_BUTTON;
    controlmap[playernum].key.inventory = 0x10000 | A_BUTTON;
    controlmap[playernum].key.forward = U_CBUTTONS;
    controlmap[playernum].key.backward = D_CBUTTONS;
    controlmap[playernum].key.strafe_left = 0x20000 | L_CBUTTONS;
    controlmap[playernum].key.strafe_right = 0x20000 | R_CBUTTONS;
    controlmap[playernum].key.buttonmode = 0x10;
    if (ps[playernum].left_right_handed != 0)
    {
        func_8004D664(playernum);
    }
}

void func_8004D664(s16 playernum)
{
    s16 i;
    s32 previous;

    for (i = 0; i < 16; i++)
    {
        previous = controlmap[playernum].map[i];
        switch (controlmap[playernum].map[i] & 0xFFFF)
        {
        case L_TRIG:
            controlmap[playernum].map[i] = R_TRIG;
            break;
        case R_TRIG:
            controlmap[playernum].map[i] = L_TRIG;
            break;
        case U_JPAD:
            controlmap[playernum].map[i] = U_CBUTTONS;
            break;
        case D_JPAD:
            controlmap[playernum].map[i] = D_CBUTTONS;
            break;
        case L_JPAD:
            controlmap[playernum].map[i] = L_CBUTTONS;
            break;
        case R_JPAD:
            controlmap[playernum].map[i] = R_CBUTTONS;
            break;
        case U_CBUTTONS:
            controlmap[playernum].map[i] = U_JPAD;
            break;
        case D_CBUTTONS:
            controlmap[playernum].map[i] = D_JPAD;
            break;
        case L_CBUTTONS:
            controlmap[playernum].map[i] = L_JPAD;
            break;
        case R_CBUTTONS:
            controlmap[playernum].map[i] = R_JPAD;
            break;
        }
        controlmap[playernum].map[i] |= (previous & 0xFFFF0000);
    }
}

void setdefaultcontrollermode(void)
{
    s16 i;

    for (i = 0; i < 4; i++)
    {
        ps[i].aim_mode = 1;
        ps[i].left_right_handed = 0;
        func_8004D888(i);
    }
}

void func_8004D888(s16 playernum)
{
    switch (ps[playernum].aim_mode)
    {
    case 0:
        func_8004D2A0(playernum);
        break;
    case 1:
        func_8004D380(playernum);
        break;
    case 2:
        func_8004D460(playernum);
        break;
    case 3:
        func_8004D568(playernum);
        break;
    default:
        func_8004D380(playernum);
        break;
    }
}