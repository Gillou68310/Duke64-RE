
#include "common.h"
#include "duke3d.h"

s8 D_800D3930 = 0;
s8 D_800D3934 = 0;
s32 D_800D3938 = 120;
s16 D_800D393C = 0;
u8 D_800D3940 = 0;

char *D_800D3944[] = {
    "DUKE NUKEM (TM) 64 (C) 1996, 1997",
    "3D REALMS ENTERTAINMENT.",
    "ALL RIGHTS RESERVED.",
    "CREATED BY",
    "3D REALMS ENTERTAINMENT.",
    "PUBLISHED AND DISTRIBUTED BY",
    "GT INTERACTIVE SOFTWARE CORP.",
    "GT IS A TRADEMARK AND THE GT LOGO",
    "IS A REGISTERED TRADEMARK OF",
    "GT INTERACTIVE SOFTWARE CORP.",
    "NINTENDO, N64, THE 3-D \"N\" LOGO",
    "AND THE OFFICIAL SEAL ARE",
    "TRADEMARKS OF",
    "NINTENDO OF AMERICA, INC.",
    "DUKE NUKEM FOR NINTENDO 64",
    "DEVELOPED BY",
    "EUROCOM DEVELOPMENTS LTD.",
    "FOR GT INTERACTIVE SOFTWARE CORP.",
    "AND 3D REALMS ENTERTAINMENT.",
    "ALL OTHER TRADEMARKS ARE THE",
    "PROPERTY OF THEIR RESPECTIVE",
    "COMPANIES.",
    "LICENSED BY NINTENDO",
};

s8 D_800D39A0 = 1;

void display_logos(void)
{
    s32 i, j;
    char buf[256];
    s32 max_files;
    s32 ret;

    func_800B76A0();
    if ((D_800D39A0 == 1) && (D_8027A3BE == 0x1000) && (D_800D3748 >= 0))
    {
        gamestate = 12;
        menu_inputfreeze = 0x32;
        D_800D39A0 = 0;
        return;
    }
    D_800D39A0 = 0;
    func_800524F0();
    switch (D_800D3940)
    {
    case 0:
        D_800D393C += 0x10;
        if (D_800D393C >= 0xFF)
        {
            D_800D393C = 0xFF;
            D_800D3940 = 2;
        }
        break;
    case 1:
        D_800D393C -= 0x10;
        if (D_800D393C <= 0)
        {
            D_800D393C = 0;
            D_800D3940 = 0;
            D_800D3930 = D_800D3934;
        }
        break;
    case 2:
        if (D_800D3938 == 0)
        {
            D_800D3940 = 1;
            D_800D3938 = 120;
        }
        else
        {
            D_800D3938 -= 1;
        }
        break;
    }
    if ((D_8027A3BE != 0) && (D_800D3930 != 0) && (D_800D3938 >= 0x10))
    {
        D_800D3938 = 0xF;
    }
    setrotatespriteflags(0xFF, 0xFF, 0xFF, D_800D393C);
    switch (D_800D3930)
    {
    case 2:
        rotatesprite(160.0f, 60.0f, 100.0f, 100.0f, 3593, 0);
        rotatesprite(100.0f, 160.0f, 100.0f, 100.0f, 3677, 0);
        rotatesprite(220.0f, 160.0f, 100.0f, 100.0f, 3678, 0);
        D_800D3934 = 3;
        break;
    case 1:
        if (D_800D3744[0] == 0)
        {
            menutext(-1, 0x32, "NO CONTROLLER");
            D_800D3938 = 1;
            return;
        }
        if (D_800D3748 == -1)
        {
            menutext(-1, 0x32, "NO CONTROLLER PAK");
            menutext(-1, 0x46, "FOUND");
            menutext(-1, 0x5A, "GAME CANNOT BE SAVED");
            D_800D3934 = 2;
            return;
        }
        if (D_800D3748 == -2)
        {
            D_800D3934 = 4;
            D_800D3930 = D_800D3934;
            return;
        }
        if (D_800D3748 == -3)
        {
            menutext(-1, 0x32, "CONTROLLER PAK");
            menutext(-1, 0x46, "DAMAGED");
            menutext(-1, 0x5A, "POWER OFF");
            menutext(-1, 0x6E, "AND REMOVE");
            D_800D3938 = 1;
            return;
        }
        if (D_801C9340 == 0)
        {
            if ((D_801C9344 >= 0x10) || (D_801C9348 < 0x200))
            {
                menutext(-1, 0x32, "CONTROLLER PAK");
                menutext(-1, 0x46, "FULL");
                menutext(-1, 0x5A, "GAME CANNOT BE SAVED");
            }
            else
            {
                sprintf(buf, "FOUND IN CONTROLLER %d", D_800D3748 + 1);
                menutext(-1, 0x32, "CONTROLLER PAK");
                menutext(-1, 0x46, buf);
            }
        }
        D_800D3934 = 2;
        break;
    case 0:
        for (i = 0; i < 23; i++)
        {
            gametext(-1, 0x12 + i * 9, D_800D3944[i]);
        }
        D_800D3934 = 1;
        break;
    case 3:
        D_801012A8 = 0;
        gamestate = 1;
        break;
    case 4:
        menutext(-1, 0x1E, "CONTROLLER PAK APPEARS");
        menutext(-1, 0x32, "DAMAGED");
        menutext(-1, 0x46, "PLEASE ENSURE THAT PAK");
        menutext(-1, 0x5A, "IS CLEAN, AND");
        menutext(-1, 0x6E, "PROPERLY INSERTED");
        rotatesprite_simplest(0x32, 0xC3, 0xC80);
        rotatesprite_simplest(0x10E, 0xC3, 0xC80);
        setrotatespriteflags(0x96, 0xFF, 0x96, D_800D393C);
        menutext(-1, 0xBE, "OK");
        if (D_8027A3BE & 0xE000)
        {
            menu_inputfreeze = 0x32;
            j = 5;
            D_800D3748 = D_800D374C;
            printf("RequestControllerInit %d\n", D_800D3748);
        label1:
            D_800D38B8 = 1;
            if (D_800D38B8 == 1)
            {
                do
                {
                    osYieldThread();
                } while (D_800D38B8 == 1);
            }
            updatetime();
            switch (D_800D38BC)
            {
            case 0:
                printf("Found memory in pad %d\n", D_800D3748);
                func_80051D40();
                osPfsNumFiles(&D_801C9350, &max_files, &D_801C9344);
                osPfsFreeBlocks(&D_801C9350, &D_801C9348);
                func_80051D6C();
                D_800D3934 = 1;
                return;
            case PFS_ERR_NOPACK:
                j -= 1;
                if (j == 0)
                {
                    D_800D3934 = 1;
                    D_800D3748 = -1;
                    return;
                }
            case PFS_ERR_NEW_PACK:
                goto label1;
            case PFS_ERR_ID_FATAL:
                printf("ID FATAL\n");
            case PFS_ERR_DEVICE:
                D_800D38C0 = 1;
                if (D_800D38C0 == 1)
                {
                    do
                    {
                        osYieldThread();
                    } while (D_800D38C0 == 1);
                }
                updatetime();
                if (D_800D38C4 == 0)
                {
                    printf("Found jolt in pad %d\n", D_800D3748);
                    D_801CB8F8[D_800D3748].unk0 = 1;
                    func_80051A80(D_800D3748);
                    D_800D3934 = 1;
                    D_800D3748 = -1;
                }
                else
                {
                    D_801012A8 = 0;
                    D_800D3934 = 5;
                }
                return;
            }
        }
        if (D_800D3930 == D_800D3934)
        {
            D_800D3938 = 1;
        }
        break;
    case 5:
        menutext(-1, 0x1E, "CONTROLLER PAK DAMAGED");
        menutext(-1, 0x32, "REPAIR OR CANCEL?");
        menutext(-1, 0x46, "CONTENTS OF PAK WILL");
        menutext(-1, 0x5A, "BE LOST");
        rotatesprite_simplest(0x32, (D_801012A8 * 0x10) + 0xB3, 0xC80);
        rotatesprite_simplest(0x10E, (D_801012A8 * 0x10) + 0xB3, 0xC80);
        if (D_801012A8 == 0)
        {
            setrotatespriteflags(0x96, 0xFF, 0x96, D_800D393C);
        }
        else
        {
            setrotatespriteflags(0xFF, 0xFF, 0xFF, D_800D393C);
        }
        menutext(-1, 0xAE, "CANCEL");
        if (D_801012A8 == 1)
        {
            setrotatespriteflags(0x96, 0xFF, 0x96, D_800D393C);
        }
        else
        {
            setrotatespriteflags(0xFF, 0xFF, 0xFF, D_800D393C);
        }
        menutext(-1, 0xBE, "REPAIR");
        if (D_800D3930 != D_800D3934)
        {
            D_8027A3BE = myy = 0;
        }
        if ((D_8027A3BE & 0x404) || (myy < 0))
        {
            D_801012A8 += 1;
            menu_inputfreeze = 0xA;
        }
        if ((D_8027A3BE & 0x808) || (myy > 0))
        {
            D_801012A8 -= 1;
            menu_inputfreeze = 0xA;
        }
        if (D_801012A8 >= 2)
        {
            D_801012A8 = 0;
        }
        if (D_801012A8 < 0)
        {
            D_801012A8 = 1;
        }
        if (D_8027A3BE & 0xE000)
        {
            menu_inputfreeze = 0x32;
            if (D_801012A8 == 0)
            {
                D_800D3934 = 1;
                D_800D3748 = -3;
                return;
            }
            func_80051D40();
            ret = osPfsRepairId(&D_801C9350);
            printf("osPfsRepairId ret %d\n", ret);
            func_80051D6C();
            j = 5;
        label2:
            D_800D38B8 = 1;
            printf("RequestControllerInit %d\n", D_800D3748);
            if (D_800D38B8 == 1)
            {
                do
                {
                    osYieldThread();
                } while (D_800D38B8 == 1);
            }
            updatetime();
            switch (D_800D38BC)
            {
            case 0:
                printf("Found memory in pad %d\n", D_800D3748);
                func_80051D40();
                osPfsNumFiles(&D_801C9350, &max_files, &D_801C9344);
                osPfsFreeBlocks(&D_801C9350, &D_801C9348);
                func_80051D6C();
                D_800D3934 = 1;
                return;
            case PFS_ERR_NOPACK:
                j -= 1;
                if (j == 0)
                {
                    D_800D3934 = 1;
                    D_800D3748 = -1;
                    return;
                }
            case PFS_ERR_NEW_PACK:
                goto label2;
            case PFS_ERR_ID_FATAL:
                printf("ID FATAL\n");
            case PFS_ERR_DEVICE:
                D_800D38C0 = 1;
                if (D_800D38C0 == 1)
                {
                    do
                    {
                        osYieldThread();
                    } while (D_800D38C0 == 1);
                }
                updatetime();
                if (D_800D38C4 == 0)
                {
                    printf("Found jolt in pad %d\n", D_800D3748);
                    D_801CB8F8[D_800D3748].unk0 = 1;
                    func_80051A80(D_800D3748);
                    D_800D3934 = 1;
                    D_800D3748 = -1;
                }
                else
                {
                    D_800D3934 = 1;
                    D_800D3748 = -3;
                }
                return;
            }
        }
        if (D_800D3930 == D_800D3934)
        {
            D_800D3938 = 1;
        }
        break;
    }
}
