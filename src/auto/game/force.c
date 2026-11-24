#include "game/levels.h"
#include "numath/nuvec.h"

typedef struct NUFORCEOBJECT_s {
    u8 unk0[4];
    struct nuhspecial_s special;
    u8 unk10[0xc];
    f32 rate;
    u16 flags;
    u8 unk22[0x2a];
    struct NUFORCEOBJECTPARAMS_s {
        u8 unk0[2];
        u16 unk2;
        u8 unk4[0x1c];
        f32 unk20;
    } *params;
    u8 unk50[0x18];
    f32 timer;
    u8 unk6C[0xc];
    struct nuvec_s position;
    u8 unk84[0x24];
    struct NUFORCEACTOR_s {
        u8 unk0[0x284];
        s8 unk284;
    } *actor;
    u8 unkAC[4];
    struct NUFORCEOBJECT_s *next;
    u8 unkB4[0xc];
    u8 unkC0;
    u8 unkC1[0x33];
} NUFORCEOBJECT;

typedef struct NUFORCEENTRY_s {
    NUFORCEOBJECT *object;
    u8 unk4[4];
    u16 size;
    u8 count;
    u8 flags;
} NUFORCEENTRY;

typedef struct NUFORCELEVELGROUP_s {
    u8 unk0;
    u8 count;
    u8 unk2[6];
    NUFORCEOBJECT *head;
    u8 unkC[8];
} NUFORCELEVELGROUP;

typedef struct NUFORCESAVESTATE_s {
    u8 unk0[0xe0];
    u64 objectBits[4];
    u8 unk100[0x40];
    struct NUFORCESAVEGROUP_s {
        u16 values[8];
        u8 count;
    } groups[8];
} NUFORCESAVESTATE;

typedef struct NUFORCESCENE_s {
    u8 unk0[0x1b4];
    NUFORCESAVESTATE *save;
    u8 unk1B8[0x3c4];
    s32 unk57C;
    u8 unk580[0xc];
    NUFORCEOBJECT *objects;
    s32 objectCount;
    NUFORCELEVELGROUP *groups;
    u8 unk598[0x3c];
    NUFORCEENTRY *entries;
    s32 entryCount;
} NUFORCESCENE;

extern s32 lbl_8040AFB8;
extern f32 lbl_803E8394[];
extern f32 lbl_801ACC48[];
extern f32 lbl_801ACC4C[];
extern f32 lbl_801ACC50[];
extern f32 lbl_801ACC54[];
extern f32 lbl_80409978;
extern f32 NuFmod(f32, f32);
extern s32 fn_800B9F38(s32, const struct nuvec_s *, s32, s32, s32, f32, f32, f32, s32);

extern void *memset();

void fn_801205AC(NUFORCESCENE *scene)
{
    s32 i = 0;

    if (scene->unk57C == 0 && scene->save != NULL) {
        memset(scene->save->objectBits, 0, sizeof(scene->save->objectBits));
        memset(scene->save->groups, 0, sizeof(scene->save->groups));
        for (; i < scene->objectCount; i++) {
            NUFORCEOBJECT *object = (NUFORCEOBJECT *)((u8 *)scene->objects + i * sizeof(NUFORCEOBJECT));

            if ((object->flags & 0x101) != 0) {
                scene->save->objectBits[i / 64] |= 1ULL << (i % 64);
            }
        }
        for (i = 0; i < 8; i++) {
            NUFORCEOBJECT *member;
            s32 index;

            scene->save->groups[i].count = scene->groups[i].count;
            member = scene->groups[i].head;
            index = scene->groups[i].count - 1;
            for (; member != NULL && index >= 0; member = member->next, index--) {
                scene->save->groups[i].values[index] = member->params->unk2;
            }
        }
    }
}

void fn_80121724(NUFORCESCENE *scene)
{
    NUFORCEENTRY *entry;
    s32 i;
    u16 phase;
    u16 angle;

    if (lbl_8040AFB8 != 0 && scene->unk57C == 0 && (entry = scene->entries) != NULL) {
        i = 0;
        phase = (s32)(NuFmod(lbl_803E8394[0], lbl_801ACC48[0]) * lbl_801ACC4C[0] * lbl_801ACC50[0]);
        angle = (s32)(NuSinApprox(phase * 2) * lbl_801ACC54[0]);
        for (; i < scene->entryCount; i++, entry = (NUFORCEENTRY *)((u8 *)entry + entry->size)) {
            if (entry->flags & 8) {
                NUFORCEOBJECT *object = entry->object;

                if ((object->flags & 0x101) == 0 && object->unkC0 != 0) {
                    fn_800B9F38(0xea, &object->position, angle, phase, 0,
                                lbl_80409978, lbl_80409978, lbl_80409978, 2);
                }
            }
        }
    }
}

extern s32 fn_800130F4(struct nuhspecial_s *);
extern struct nuinstanim_s *fn_8001384C(struct nuhspecial_s *);
extern void fn_80120EF8(NUFORCEOBJECT *, s32, s32);
extern void JudderGameCamera(void *, s8, s8, f32);
extern char MenuHeader[];
extern f32 lbl_80408E9C;
extern f32 lbl_801ACC6C[];
extern f32 lbl_801ACC70[];
extern f32 lbl_801ACC74[];
extern f32 lbl_801ACC78[];
extern f32 lbl_801ACC7C[];

void fn_801220A8(void *unused, NUFORCEOBJECT *object)
{
    struct nuhspecial_s *special;
    struct NUFORCEACTOR_s *actor;
    s32 judder = 0;

    if ((object->flags & 0x100) == 0) {
        special = &object->special;

        if (fn_800130F4(special) != 0 && fn_8001384C(special) != NULL) {
            struct nuinstanim_s *anim = fn_8001384C(special);
            f32 limit = *object->special.scene->instanim_data[anim->anim_ix];

            if (anim->ltime >= limit) {
                actor = object->actor;
                if (actor != NULL) {
                    goto hasActor;
                }
                goto noActor;
            } else {
                f32 normalRate = lbl_801ACC70[0];
                f32 rate;

                anim->flags |= 0x80000000;
                rate = object->rate;
                if (rate > normalRate) {
                    object->rate = rate - lbl_80408E9C * lbl_801ACC74[0];
                    if (object->rate < normalRate) {
                        object->rate = normalRate;
                    }
                } else if (rate < normalRate) {
                    object->rate = lbl_80408E9C * lbl_801ACC74[0] + rate;
                    if (object->rate > normalRate) {
                        object->rate = normalRate;
                    }
                }
                *(f32 *)((u8 *)anim + 0x40) = *(f32 *)((u8 *)anim + 0x54) * object->rate;
            }
        } else {
            f32 zero = lbl_801ACC6C[0];

            if (object->params->unk20 > zero) {
                s32 index;

                object->timer -= lbl_80408E9C;
                if (object->timer <= zero) {
                    actor = object->actor;
                    if (actor != NULL) {
hasActor:
                        index = actor->unk284;
                    } else {
noActor:
                        index = -1;
                    }
                    fn_80120EF8(object, 1, index);
                    judder = 1;
                }
            } else {
                s32 index = object->actor != NULL ? object->actor->unk284 : -1;

                fn_80120EF8(object, 1, index);
                judder = 1;
            }
        }
    }
    if (judder) {
        f32 amount;

        if (lbl_8040AFB8 != 0) {
            amount = lbl_801ACC78[0];
        } else {
            amount = lbl_801ACC7C[0];
        }
        JudderGameCamera(MenuHeader, 2, 0, amount);
    }
}
