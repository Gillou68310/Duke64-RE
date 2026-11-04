#include "common.h"
#include "duke3d.h"

#define EXTRA_SAMPLES 80
#define FRAME_LAG 2
#define NUM_DMA_BUFFERS 32
#define DMA_BUFFER_LENGTH 1024
#define MAX_CLIST_SIZE 1024 // FAKE

extern u8 soundctl_ROM_START[];
extern u8 soundctl_ROM_END[];
extern u8 soundtbl_ROM_START[];
extern u8 soundtbl_ROM_END[];
extern u8 musicseq_ROM_START[];
extern u8 musicseq_ROM_END[];
extern u8 musicctl_ROM_START[];
extern u8 musicctl_ROM_END[];
extern u8 musictbl_ROM_START[];
extern u8 musictbl_ROM_END[];

typedef struct
{
    s8 unk0;
    s16 unk2;
    s32 unk4;
    s32 unk8;
    s32 unkC;
} unk801CE710;

typedef struct
{
    ALLink node;
    s32 startAddr;
    u32 lastFrame;
    char *ptr;
} AMDMABuffer;

typedef struct
{
    u8 initialized;
    AMDMABuffer *firstUsed;
    AMDMABuffer *firstFree;
} AMDMAState;

ALSndPlayer D_801CD840;
ALCSPlayer D_801CD898;
ALCSeq D_801CD918;
ALSndId D_801CDA10[6][0x115];
unk801CE710 D_801CE710[12];
u8 D_801CE7D0[0x115];
OSScTask *AudioTask[2];
OSScTask *AudioTaskPtr;
ALGlobals D_801CE8F8;
ALSynConfig D_801CE948;
ALSeqpConfig D_801CE970;
ALSndpConfig D_801CE990;
u8 D_801CE9A0[0x4B000];
OSMesgQueue audDMAMessageQ;
void *D_802199B8[8];
OSIoMesg audDMAIOMesgBuf[NUM_DMA_BUFFERS];
Acmd *AudioCmdList[2];
s16 *frameData[3];
ALHeap AudioHeap;
AMDMAState dmaState;
AMDMABuffer dmaBuffer[NUM_DMA_BUFFERS];
s32 audFrameCt;
ALSeqFile *D_80219F94;
u32 D_80219F98;
u32 D_80219F9C;
u32 D_80219FA0;
ALInstrument *D_80219FA4;
Acmd *AudioCmdListPtr;
s16 *audioPtr;
u8 *D_80219FB0[NUM_DMA_BUFFERS];
ALBankFile *D_8021A030;
ALBankFile *D_8021A034;
u8 *D_8021A038;
f32 D_8021A03C;
s32 D_8021A040;
u32 D_8021A044;
s32 frameSize;
s32 minFrameSize;
s32 frameIndex;
s32 cmdLen;
s32 D_8021A058;
s32 D_8021A05C;

s32 D_800D4890[6] = {0};
s16 D_800D48A8[] =
{
    11025, 5988, 5988, 5988, 5988, 10989, 11025, 8000, 5988, 8000, 8000, 11025, 5988, 5988, 5988, 5988,
    5988, 8000, 11025, 8000, 8000, 8000, 8000, 5988, 8000, 8000, 8000, 8000, 5988, 8000, 8000, 8000,
    5988, 11025, 8000, 8000, 8000, 5988, 8000, 8000, 8000, 8000, 8000, 10989, 8000, 5988, 8000, 8000,
    8000, 8000, 5988, 5988, 8000, 8000, 10869, 5988, 8000, 5988, 5988, 8000, 5988, 5988, 11025, 11025,
    10989, 11025, 8000, 11025, 8000, 10989, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 5988, 8000, 8000,
    8000, 8000, 8000, 8000, 8000, 8000, 8000, 10989, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 11025,
    8000, 8000, 8000, 8000, 11025, 8000, 8000, 8000, 11025, 11025, 11025, 8000, 8000, 8000, 8000, 5988,
    5988, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 11025, 8000, 8000, 8000, 8000, 10989, 5988, 8000,
    8000, 8000, 8000, 8000, 8000, 5988, 8000, 5988, 5988, 5988, 8000, 8000, 5988, 8000, 10989, 11025,
    8000, 8000, 8000, 8000, 11025, 8000, 8000, 8000, 10989, 8000, 8000, 8000, 10989, 8000, 8000, 8000,
    8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000,
    8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 5988, 5988, 5988, 5988, 8000, 8000, 11025,
    8000, 8000, 8000, 8000, 8000, 8000, 8000, 5988, 8000, 8000, 5988, 8000, 8000, 8000, 8000, 8000,
    8000, 5988, 8000, 8000, 8000, 8000, 11025, 8000, 8000, 8000, 8000, 8000, 8000, 5988, 8000, 8000,
    11025, 8000, 8000, 11025, 8000, 8000, 8000, 8000, 8000, 8000, 10989, 5988, 8000, 8000, 8000, 8000,
    8000, 8000, 8000, 8000, 8000, 10989, 8000, 5988, 10989, 8000, 8000, 10989, 8000, 11025, 22050, 5988,
    8000, 8000, 8000, 8000, 8000, 8000, 8000, 8000, 11025, 11025, 11025, 11025, 11025, 11025, 11025, 11025,
    11025, 11025, 11025, 11025, 11025, 0, 0, 0, 0, 0, 0, 0,
};

s16 D_800D4AE0[] = 
{
    3, 3, 6, 1, 1, 6, 6, 6, 6, 6, 3, 6, 6, 3, 3, 6,
    3, 6, 6, 4, 6, 1, 1, 1, 1, 1, 1, 3, 2, 1, 1, 1,
    2, 4, 1, 2, 3, 2, 2, 2, 2, 1, 2, 2, 3, 2, 2, 1,
    2, 2, 2, 2, 1, 1, 4, 2, 2, 2, 2, 2, 2, 2, 2, 3,
    3, 3, 2, 2, 2, 6, 3, 1, 2, 2, 4, 3, 1, 4, 1, 1,
    6, 3, 3, 6, 3, 1, 2, 6, 3, 3, 1, 3, 3, 6, 3, 3,
    2, 2, 6, 3, 3, 2, 3, 2, 1, 2, 1, 3, 3, 1, 3, 3,
    3, 3, 1, 2, 3, 2, 2, 3, 6, 2, 3, 4, 3, 4, 3, 4,
    3, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 2, 2, 1, 1, 1,
    2, 2, 2, 2, 3, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1,
    1, 2, 2, 1, 1, 1, 2, 4, 3, 3, 2, 2, 1, 1, 2, 1,
    3, 2, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 1, 1, 1,
    1, 1, 4, 2, 1, 3, 3, 6, 1, 4, 2, 2, 2, 2, 2, 2,
    2, 2, 1, 1, 1, 2, 2, 2, 2, 2, 2, 3, 2, 2, 4, 1,
    2, 2, 1, 4, 2, 1, 2, 2, 2, 2, 3, 2, 2, 2, 6, 4,
    1, 3, 1, 4, 2, 2, 4, 4, 2, 2, 3, 2, 1, 6, 1, 1,
    1, 1, 1, 1, 1, 1, 1, 1, 6, 2, 4, 6, 6, 3, 4, 6,
    3, 3, 3, 3, 3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

ALSndPlayer *D_800D4D20 = &D_801CD840;
s32 D_800D4D24[3] = {0};
s32 D_800D4D30 = 0;
s32 curList = 0;
s32 frameNumber = 1;
s16 frameSamples[14] = {0};
ALCSPlayer *D_800D4D58 = &D_801CD898;
ALCSeq *D_800D4D5C = &D_801CD918;
ALBank *D_800D4D60 = NULL;
ALBank *D_800D4D64 = NULL;
s32 D_800D4D68[2] = {0};
u32 samplesLeft = 0;

/*TODO*/
static const char D_80104600[] = "AUDIO DMA Buffer overflow, give me more AUDIO_DMA_QUEUE_SIZE\n";

void func_800579C4(s32 arg0, s32 arg1, s16 arg2, s32 arg3, s32 arg4, s16 arg5, s16 arg6);
void func_80057E28(s32 arg0);
void func_80057E90(void);
void func_80057EB4(s32);

s32 __amDMA(s32 addr, s32 len, void *state)
{
    u8 *buf;
    s32 delta;
    AMDMABuffer *dmaPtr;
    AMDMABuffer *lastDmaPtr;
    AMDMABuffer *p;
    s32 buffEnd;
    u32 sts;

    dmaPtr = dmaState.firstUsed;
    lastDmaPtr = NULL;
    while (dmaPtr != NULL)
    {
        buffEnd = dmaPtr->startAddr + DMA_BUFFER_LENGTH;
        if (dmaPtr->startAddr > addr)
            break;

        lastDmaPtr = dmaPtr;
        if (buffEnd >= (addr + len))
        {
            dmaPtr->lastFrame = audFrameCt;
            return osVirtualToPhysical(&dmaPtr->ptr[addr] - dmaPtr->startAddr);
        }
        dmaPtr = (AMDMABuffer *)dmaPtr->node.next;
    }

    dmaPtr = dmaState.firstFree;
    dmaState.firstFree = (AMDMABuffer *)dmaPtr->node.next;
    alUnlink((ALLink *)dmaPtr);
    if (&lastDmaPtr->node != NULL)
    {
        alLink((ALLink *)dmaPtr, (ALLink *)&lastDmaPtr->node);
    }
    else
    {
        p = dmaState.firstUsed;
        if (p != NULL)
        {
            dmaState.firstUsed = dmaPtr;
            dmaPtr->node.next = (ALLink *)p;
            dmaPtr->node.prev = NULL;
            p->node.prev = (ALLink *)dmaPtr;
        }
        else
        {
            dmaState.firstUsed = dmaPtr;
            dmaPtr->node.next = NULL;
            dmaPtr->node.prev = NULL;
        }
    }
    buf = dmaPtr->ptr;
    delta = addr & 1;
    addr -= delta;
    dmaPtr->startAddr = addr;
    dmaPtr->lastFrame = audFrameCt;
    osWritebackDCacheAll();
    while (1)
    {
        sts = osPiGetStatus();
        if (sts & 2)
            continue;

        if (sts & 1)
            continue;

        if (sts & 4)
            continue;

        break;
    };

    osPiStartDma(&audDMAIOMesgBuf[D_800D4D30++], 0, 0, addr, buf, DMA_BUFFER_LENGTH, &audDMAMessageQ);
    return osVirtualToPhysical(buf) + delta;
}

ALDMAproc __amDmaNew(AMDMAState **state)
{
    s32 i;

    if (dmaState.initialized == 0)
    {
        dmaState.firstFree = dmaBuffer;
        for (i = 0; i < NUM_DMA_BUFFERS - 1; i++)
        {
            alLink(&dmaBuffer[i + 1].node, &dmaBuffer[i].node);
            dmaBuffer[i].ptr = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, DMA_BUFFER_LENGTH);
        }
        dmaState.initialized = 1;
    }
    *state = &dmaState;
    return __amDMA;
}

void __clearAudioDMA(void)
{
    AMDMABuffer *dmaPtr, *nextPtr;

    dmaPtr = dmaState.firstUsed;
    while (dmaPtr != NULL)
    {
        nextPtr = (AMDMABuffer *)dmaPtr->node.next;
        if ((dmaPtr->lastFrame + FRAME_LAG) < audFrameCt)
        {
            if (dmaPtr == dmaState.firstUsed)
                dmaState.firstUsed = (AMDMABuffer *)dmaPtr->node.next;

            alUnlink((ALLink *)dmaPtr);
            if (dmaState.firstFree != NULL)
            {
                alLink((ALLink *)dmaPtr, (ALLink *)dmaState.firstFree);
            }
            else
            {
                dmaState.firstFree = dmaPtr;
                dmaPtr->node.next = NULL;
                dmaPtr->node.prev = NULL;
            }
        }
        dmaPtr = nextPtr;
    }
}

void func_800570E8(void)
{
    osCreateMesgQueue(&audDMAMessageQ, D_802199B8, 8);
    alHeapInit(&AudioHeap, D_801CE9A0, sizeof(D_801CE9A0));
    AudioCmdList[0] = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, sizeof(Acmd) * 2048);
    AudioCmdList[1] = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, sizeof(Acmd) * 2048);
    AudioTask[0] = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, sizeof(OSScTask));
    AudioTask[1] = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, sizeof(OSScTask));
    frameData[0] = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, sizeof(s16) * 2048);
    frameData[1] = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, sizeof(s16) * 2048);
    frameData[2] = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, sizeof(s16) * 2048);
    D_801CE948.outputRate = osAiSetFrequency(22050);
    D_8021A03C = (D_801CE948.outputRate * (2.0f - 1.0f)) / 60.0f;
    frameSize = D_8021A03C;

    if (frameSize < D_8021A03C)
        frameSize++;

    if (frameSize & 0xF)
        frameSize = (frameSize & ~0xF) + 16;

    minFrameSize = frameSize - 16;
    D_801CE948.maxVVoices = 16;
    D_801CE948.maxPVoices = 16;
    D_801CE948.maxUpdates = 128;
    D_801CE948.dmaproc = __amDmaNew;
    D_801CE948.heap = &AudioHeap;
    D_801CE948.fxType = 1;
    D_801CE948.params = NULL;
    alInit(&D_801CE8F8, &D_801CE948);
}

void func_8005731C(void)
{
    u32 new_var;
    ALSndId sndid;
    s32 i, j;

    D_801CE990.maxSounds = 0x67E;
    D_801CE990.maxEvents = 0x80;
    D_801CE990.heap = &AudioHeap;
    alSndpNew(D_800D4D20, &D_801CE990);
    D_8021A040 = ((s32)soundctl_ROM_END) - ((s32)soundctl_ROM_START);
    D_8021A030 = alHeapDBAlloc(0, 0, &AudioHeap, 1, D_8021A040);
    copyfromrom((char *)D_8021A030, (u32)soundctl_ROM_START, D_8021A040);
    alBnkfNew(D_8021A030, soundtbl_ROM_START);
    D_800D4D60 = D_8021A030->bankArray[0];
    D_8021A058 = 0;
    D_80219FA0 = 0;
    D_80219FA4 = D_800D4D60->instArray[0];

    for (i = 0; i < ARRAY_COUNT(D_801CE710); i++)
        D_801CE710[i].unk0 = 0;

    for (i = 0; i < ARRAY_COUNT(D_801CE7D0); i++)
        D_801CE7D0[i] = 0;

    for (i = 0; i < D_80219FA4->soundCount; i++)
    {
        D_80219F9C = -1;
        for (j = 0; j < D_80219FA4->soundCount; j++)
        {
            D_80219F98 = (u32)D_80219FA4->soundArray[j];
            if ((D_80219F98 < D_80219F9C))
            {
                new_var = D_80219F98; // FAKE
                if (D_80219FA0 < (new_var))
                    D_80219F9C = new_var;
            }
        }

        for (j = 0; j < ARRAY_COUNT(D_801CDA10); j++)
        {
            sndid = alSndpAllocate(D_800D4D20, (ALSound *)D_80219F9C);
            D_801CDA10[j][D_8021A058] = sndid;
            if (sndid == -1)
            {
                printf("Unable to allocate sound! Oh duffer\n");
                exit(0);
            }
        }

        D_8021A058 += 1;
        D_80219FA0 = D_80219F9C;
    }
}

/*TODO*/
static const char D_80104668[] = "Num Sounds %d\n";
static const char D_80104678[] = "Too many Midi tunes, give me more MAX_MIDI_TUNES\n";
static const char D_801046AC[] = "Num Midi tunes %d\n";
static const char D_801046C0[] = "alHeap Free 0x%X\n";
static const char D_801046D4[] = "Sound id out of range %d\n";

void func_8005757C(void)
{
    s32 i;

    D_8021A040 = (s32)musicctl_ROM_END - (s32)musicctl_ROM_START;
    D_8021A034 = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, D_8021A040);
    copyfromrom((char *)D_8021A034, (u32)musicctl_ROM_START, D_8021A040);
    D_80219F94 = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, 4);
    copyfromrom((char *)D_80219F94, (u32)musicseq_ROM_START, 4);
    D_8021A044 = (D_80219F94->seqCount * 8) + 4;
    D_80219F94 = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, (D_80219F94->seqCount * 8) + 4);
    copyfromrom((char *)D_80219F94, (u32)musicseq_ROM_START, D_8021A044);
    alSeqFileNew(D_80219F94, musicseq_ROM_START);
    D_801CE970.maxVoices = 0x10;
    D_801CE970.maxEvents = 0x80;
    D_801CE970.maxChannels = 0x10;
    D_801CE970.heap = &AudioHeap;
    D_801CE970.initOsc = NULL;
    D_801CE970.updateOsc = NULL;
    D_801CE970.stopOsc = NULL;
    alCSPNew(D_800D4D58, &D_801CE970);
    alBnkfNew(D_8021A034, musictbl_ROM_START);
    D_800D4D64 = D_8021A034->bankArray[0];
    alSeqpSetBank((ALSeqPlayer *)D_800D4D58, D_800D4D64);
    D_8021A05C = D_80219F94->seqCount;
    if (!D_8021A05C) // FAKE
        ;

    for (i = 0; i < D_8021A05C; i++)
    {
        D_8021A038 = D_80219F94->seqArray[i].offset;
        D_8021A044 = D_80219F94->seqArray[i].len;

        if (D_8021A044 & 1)
            D_8021A044++;

        D_80219FB0[i] = alHeapDBAlloc(NULL, 0, &AudioHeap, 1, D_8021A044);
        copyfromrom(D_80219FB0[i], (u32)D_8021A038, D_8021A044);
    }
}

void soundInit(void)
{
    func_800570E8();
    func_8005731C();
    func_8005757C();
}

void func_8005780C(void)
{
    unk8021A0F8 *msg;
    s32 i;
    s32 unk0;
    s32 unk1;
    s32 unk2;
    s32 pad[2];
    s32 unk14;

    while (D_8021A060.validCount != 0)
    {
        osRecvMesg(&D_8021A060, (OSMesg *)&msg, OS_MESG_BLOCK);
        unk0 = msg->unk0;
        switch (unk0)
        {
        case 1:
            unk1 = msg->unk1;
            unk2 = msg->unk2;
            unk14 = msg->unk14;
            func_800579C4(msg->unk4, msg->unk8, msg->unkC, msg->unk10, unk14, unk1, unk2);
            break;
        case 2:
            unk14 = msg->unk14;
            func_80057EB4(unk14);
            break;
        case 5:
            for (i = 0; i < ARRAY_COUNT(D_801CE710); i++)
            {
                if (D_801CE710[i].unk0 != 0)
                    func_80057EB4(D_801CE710[i].unkC);
            }
            break;
        case 3:
            func_80057E28(0);
            break;
        case 4:
            func_80057E90();
            break;
        case 6:
            alCSPSetVol(D_800D4D58, ((f32)(D_801012A4 * 0xFF) * 0.5));
            break;
        }
        msg->unk0 = 0;
    }
}

void func_800579C4(s32 arg0, s32 arg1, s16 arg2, s32 arg3, s32 arg4, s16 arg5, s16 arg6)
{
    s16 j;
    s16 k;
    s16 i;

    k = -1;
    j = -1;
    if ((arg0 < ARRAY_COUNT(D_801CE7D0)) && (arg0 >= 0))
    {
        for (i = 0; i < ARRAY_COUNT(D_801CE710); i++)
        {
            if (D_801CE710[i].unk0 == 0)
                k = i;
        }

        if (k == -1)
        {
            testcallback(arg0);
            return;
        }

        for (i = 0; i < D_800D4AE0[arg0]; i++)
        {
            if ((D_801CE7D0[arg0] & pow2char[i]) != pow2char[i])
                j = i;
        }

        if (j == -1)
        {
            testcallback(arg0);
            return;
        }

        alSndpSetSound(D_800D4D20, D_801CDA10[j][arg0]);
        if (alSndpGetState(D_800D4D20) != 0)
        {
            testcallback(arg0);
            return;
        }

        D_801CE7D0[arg0] |= pow2char[j];
        alSndpSetSound(D_800D4D20, D_801CDA10[j][arg0]);
        alSndpSetPitch(D_800D4D20, ((f32)D_800D48A8[arg0] + arg6) / 22050.0f);
        alSndpSetPan(D_800D4D20, arg3);
        alSndpSetVol(D_800D4D20, ((f32)(D_801012A4 * arg1) * 0.5));
        alSndpPlay(D_800D4D20);
        D_801CE710[k].unk0 = 1;
        D_801CE710[k].unk4 = arg0;
        D_801CE710[k].unk8 = j;
        D_801CE710[k].unk2 = arg2;
        D_801CE710[k].unkC = arg4;
    }
}

void func_80057C78(void)
{
    s16 new_var;
    s16 i;
    s32 sp50;

    for (i = 0; i < ARRAY_COUNT(D_801CE710); i++)
    {
        if (D_801CE710[i].unk0 == 0)
            continue;

        alSndpSetSound(D_800D4D20, D_801CDA10[D_801CE710[i].unk8][D_801CE710[i].unk4]);
        if (alSndpGetState(D_800D4D20) == 0)
        {
            D_801CE710[i].unk0 = 0;
            D_801CE7D0[D_801CE710[i].unk4] ^= pow2char[D_801CE710[i].unk8];
            testcallback(D_801CE710[i].unk4);
        }
        else
        {
            new_var = D_801CE710[i].unk2;
            if (new_var != -1)
            {
                alSndpSetVol(D_800D4D20, ((f32)(func_8005878C(new_var, D_801CE710[i].unk4, &sp50) * D_801012A4)) * 0.5);
                alSndpSetPan(D_800D4D20, sp50);
            }
        }
    }
}

void func_80057E28(s32 arg0)
{
    alCSPStop(D_800D4D58);
    alCSeqNew(D_800D4D5C, D_80219FB0[arg0]);
    alCSPSetSeq(D_800D4D58, D_800D4D5C);
    alCSPPlay(D_800D4D58);
}

void func_80057E90(void)
{
    alCSPStop(D_800D4D58);
}

void func_80057EB4(s32 arg0)
{
    s16 i;

    for (i = 0; i < ARRAY_COUNT(D_801CE710); i++)
    {
        if ((D_801CE710[i].unk0 == 0) || (arg0 != D_801CE710[i].unkC))
            continue;

        alSndpSetSound(D_800D4D20, D_801CDA10[D_801CE710[i].unk8][D_801CE710[i].unk4]);
        alSndpStop(D_800D4D20);
        D_801CE710[i].unk0 = 0;
        D_801CE7D0[D_801CE710[i].unk4] ^= pow2char[D_801CE710[i].unk8];
        testcallback(D_801CE710[i].unk4);
    }
}

void __amHandleFrameMsg(void)
{
    AudioTaskPtr = AudioTask[curList];
    AudioCmdListPtr = AudioCmdList[curList];
    frameIndex = frameNumber % 3;
    D_800D4D30 = 0;
    __clearAudioDMA();
    audioPtr = (s16 *)osVirtualToPhysical(frameData[frameIndex]);
    frameSamples[frameIndex] = 16 + ((frameSize - samplesLeft) + EXTRA_SAMPLES) & ~0xf;
    if (frameSamples[frameIndex] < minFrameSize)
        frameSamples[frameIndex] = minFrameSize;

    AudioCmdListPtr = alAudioFrame(AudioCmdListPtr, &cmdLen, audioPtr, frameSamples[frameIndex]);
    AudioTaskPtr->list.t.data_ptr = (u64 *)AudioCmdList[curList];
    AudioTaskPtr->list.t.data_size = ((AudioCmdListPtr - AudioCmdList[curList])) * sizeof(Acmd);
    AudioTaskPtr->list.t.type = M_AUDTASK;
    AudioTaskPtr->list.t.flags = 0;
    if ((AudioCmdListPtr - AudioCmdList[curList]) > MAX_CLIST_SIZE)
        printf("AUDIO Task overrun, give me more MAX_CLIST_SIZE\n");

    AudioTaskPtr->list.t.ucode_boot = (u64 *)rspbootTextStart;
    AudioTaskPtr->list.t.ucode_boot_size = (u32)rspbootTextEnd - (u32)rspbootTextStart;
    AudioTaskPtr->list.t.ucode = (u64 *)aspMainTextStart;
    AudioTaskPtr->list.t.ucode_data = (u64 *)aspMainDataStart;
    AudioTaskPtr->list.t.ucode_data_size = 0x800;
    AudioTaskPtr->list.t.dram_stack = NULL;
    AudioTaskPtr->list.t.dram_stack_size = 0;
    AudioTaskPtr->list.t.output_buff = NULL;
    AudioTaskPtr->list.t.output_buff_size = NULL;
    AudioTaskPtr->list.t.yield_data_ptr = NULL;
    AudioTaskPtr->list.t.yield_data_size = 0;
    AudioTaskPtr->next = NULL;
    AudioTaskPtr->flags = OS_SC_NEEDS_RSP | OS_SC_LAST_TASK;
    AudioTaskPtr->msgQ = &audioReplyMsgQ;
    AudioTaskPtr->msg = NULL;
    AudioTaskPtr->framebuffer = NULL;
    osSendMesg(osScGetCmdQ(&scheduler), AudioTaskPtr, OS_MESG_BLOCK);
    osRecvMesg(&audioReplyMsgQ, NULL, OS_MESG_BLOCK);
    samplesLeft = IO_READ(AI_LEN_REG) >> 2;
    frameIndex = frameNumber % 3;
    osAiSetNextBuffer(frameData[frameIndex], frameSamples[frameIndex] * 4);
    audFrameCt += 1;
    curList ^= 1;
    frameNumber += 1;
    func_80057C78();
    func_8005780C();
}
