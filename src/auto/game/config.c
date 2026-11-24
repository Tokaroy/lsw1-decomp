#include "game/levels.h"

extern struct lbl_80409410_s *lbl_80409410;
extern void CreditBufferAlloc(void **, void **);
extern void fn_800B9B6C(void);
extern s32 lbl_804093B4;
extern f32 lbl_8040B0E0, lbl_8040B0E4, lbl_8040B0E8;

void fn_8010C8C8(void)
{
    struct lbl_80409410_s *level = lbl_80409410;

    CreditBufferAlloc(&level->creditBuffer, &level->creditBufferEnd);
    if (lbl_804093B4 != 0x7A)
        fn_800B9B6C();
    lbl_8040B0E4 = 1.0f;
    lbl_8040B0E0 = 0.0f;
    lbl_8040B0E8 = 0.0f;
}

extern void fn_800A1BD4(void);
extern void fn_800B9974(f32 *);
extern f32 NuRndrHackWaterLevel[];
extern s32 lbl_80408FE0;
extern u8 lbl_8040B02C[], lbl_8040B02D[];
extern s32 lbl_8040B24C;
extern f32 lbl_80408E98, lbl_80408E9C;

struct NUPADSTATE_s {
    u8 pad0[8];
    u32 flags8;
    u8 padC[0x50];
};

extern struct NUPADSTATE_s lbl_803D9A58[];

s32 fn_8010C920(void)
{
    s32 applied;
    s32 active;

    fn_800A1BD4();
    applied = 0;
    if (NuRndrHackWaterLevel[0] >= 0.2f) {
        active = 0;
        if (lbl_803D9A58[0].flags8 & 0x10) {
            active = 1;
            if (lbl_80409410 && lbl_80409410->unk198 == 0x7C && !lbl_80408FE0)
                lbl_8040B02C[0] = 1;
        } else if (lbl_803D9A58[1].flags8 & 0x10) {
            active = 1;
            if (lbl_80409410 && lbl_80409410->unk198 == 0x7C && !lbl_80408FE0)
                lbl_8040B02D[0] = 1;
        }
        if (active) {
            applied = 1;
            lbl_8040B24C = applied;
        }
    }
    lbl_80408E9C = lbl_80408E98;
    fn_800B9974(NuRndrHackWaterLevel);
    return applied;
}

extern void NuStrCpy(char *, const char *);
extern void NuStrCat(char *, const char *);
extern void fn_80044D20(void);
extern void fn_80044D4C(void);
extern void fn_800B9928(f32 *, f32);
extern void fn_800A1C70(s32);
extern s32 fn_8004901C(s32, const char *, ...);

struct NUMOVIEENTRY_s {
    const char *name;
    u32 unk4;
};

extern struct NUMOVIEENTRY_s lbl_801DEC2C[];
extern s32 lbl_804086E8;
extern char lbl_803E04F0[];
extern const char lbl_801AA3A8[], lbl_801AA3B0[], lbl_801AA3B8[];
extern const f32 lbl_801AA3C0[];

void fn_8010CA18(s32 movie, void **buffer, void **bufferEnd)
{
    void *end;
    s32 offset = movie * sizeof(struct NUMOVIEENTRY_s);

    lbl_8040B24C = 0;
    if (((struct NUMOVIEENTRY_s *)(offset + (u8 *)lbl_801DEC2C))->name) {
        NuStrCpy(lbl_803E04F0, lbl_801AA3A8);
        NuStrCat(lbl_803E04F0, lbl_801AA3B0);
        NuStrCat(lbl_803E04F0, ((struct NUMOVIEENTRY_s *)(offset + (u8 *)lbl_801DEC2C))->name);
        NuStrCat(lbl_803E04F0, lbl_801AA3B8);
    }
    if (lbl_804086E8 == 0) {
        fn_80044D20();
        fn_80044D4C();
    }
    fn_800B9928(NuRndrHackWaterLevel, lbl_801AA3C0[0]);
    if (((struct NUMOVIEENTRY_s *)(offset + (u8 *)lbl_801DEC2C))->name && buffer && bufferEnd) {
        end = *bufferEnd;
        fn_8004901C(2, lbl_803E04F0, 3, 4, 0, 7, fn_8010C920, 0xA, 0, 8, buffer, &end, 1);
    }
    fn_800A1C70(2);
}

#include "game/gamemenu.h"

struct NUCREDITENTRY_s {
    char *text;
    f32 x;
    f32 time;
    f32 scale;
    u8 red, green, blue;
    u8 style;
    u8 type;
};

extern s32 fn_800CC764(s32);
extern char *lbl_803DF67C[], *lbl_803DF680[], *lbl_803DF684[];
extern f32 lbl_80409920, lbl_8040A750, lbl_8040B0E4;
extern u8 lbl_8040B060[];

void fn_8010C570(struct NUCREDITENTRY_s *entry, f32 elapsed)
{
    s32 alpha = (s32)(lbl_8040B0E4 * 128.0f);

    if (lbl_8040B060[0] == 0) {
        f32 position = lbl_8040A750 * (elapsed / lbl_80409920);

        for (; entry->text; entry++) {
            f32 time = entry->time;
            if (position >= time && position <= time + 2.2f) {
                fn_8007B298(entry->text, entry->x, -1.0f - (time - position), 1.0f, entry->scale, entry->scale, entry->scale, entry->style,
             entry->red, entry->green, entry->blue, ((u32)(entry->type - 2) <= 1) ? 0.825f : 1.65f, 1, 0, 0, alpha);
            }
        }
    } else {
        f32 a = (f32)alpha;
        s32 firstAlpha = !fn_800CC764(0) ? (s32)(a * 0.25f) : (s32)a;
        fn_8007B298(lbl_803DF67C[0], 0.0f, 0.25f, 1.0f, 0.7f, 0.7f, 0.7f, 0,
                     255, 191, 0, 1.7f, 1, 0, 0, firstAlpha);
        a = (f32)alpha;
        fn_8007B298(lbl_803DF680[0], 0.0f, 0.0f, 1.0f, 0.7f, 0.7f, 0.7f, 0,
                     255, 191, 0, 1.7f, 1, 0, 0, (s32)(!fn_800CC764(1) ? a * 0.25f : a));
        a = (f32)alpha;
        fn_8007B298(lbl_803DF684[0], 0.0f, -0.25f, 1.0f, 0.7f, 0.7f, 0.7f, 0,
                     255, 191, 0, 1.7f, 1, 0, 0, !fn_800CC764(2) ? (s32)(a * 0.25f) : (s32)a);
    }
}
