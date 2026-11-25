#include "types.h"

extern void fn_8002C3A0(s32 a, s32 b, s32 c);


struct nustream_mtx_s {
    f32 m[16];
};

/* +0x48 target of fn_8002CAC8/fn_8002CD0C (flag word at +0 via fn_8002CD00). */
struct nustream_xf_s {
    s32 flag;
    f32 pos[3];
    struct nustream_mtx_s mtx;
};

/* Font glyph rectangle in texels: the 0xC-byte table at text +0x34. */
struct nustream_glyph_s {
    f32 u;
    f32 v;
    f32 w;
};

/* Vertex passed to NuRndrTriStrip3dClip (0x24 bytes). */
struct nustream_vtx_s {
    f32 x, y, z;
    f32 pad[3];
    s32 colour;
    f32 u, v;
};

struct nustream_font_s {
    u8 pad0[0xC2];
    u16 digit; /* 0xC2: glyph used for fixed-width digits */
};

/* fn_8002C644's view of the stream object (same object as nustream_obj_s). */
struct nustream_text_s {
    u8 pad0[0x14];
    f32 height; /* 0x14 */
    u8 pad18[4];
    f32 space; /* 0x1c */
    u8 pad20[4];
    f32 gap; /* 0x24 */
    u8 pad28[4];
    f32* xscale; /* 0x2c */
    f32* yscale; /* 0x30 */
    struct nustream_glyph_s* glyphs; /* 0x34 */
    struct nustream_font_s* font; /* 0x38 */
    u8 pad3c[4];
    s32 mtl; /* 0x40 */
    s32* colour; /* 0x44 */
    struct nustream_xf_s* xf; /* 0x48 */
};

extern f32 lbl_8040ADA0;
extern f32 lbl_8040ADA4;
extern f32 lbl_8040ADA8;
extern f32 lbl_80407CC4;
extern void NuRndrTriStrip3dClip(struct nustream_vtx_s* v, s32 n, struct nustream_mtx_s* m, s32 mtl);
extern void fn_800374C8(s32 prim, s32 fmt, s32 mtl);
extern void fn_8003777C(f32 u, f32 v);
extern void fn_80037788(s32 colour);
extern void fn_800377AC(f32 x, f32 y, f32 z);
extern void fn_80037864(void);

/* NONMATCHING 122/232: retail keeps mode != 0 in cr4 and reloads 16.0f; see work/fn_8002C644/status.md. */
void fn_8002C644(struct nustream_text_s* o, u16* str, s32 mode)
{
    struct nustream_vtx_s vtx[4];
    struct nustream_glyph_s* g;
    f32 h;
    f32 w;
    f32 dw;
    f32 x;
    f32 y;
    f32 x0, x1, y0, y1;
    f32 u0, u1, v0, v1;
    s32 col;
    u32 c;

    lbl_8040ADA4 = o->xf->pos[0];
    lbl_8040ADA0 = o->xf->pos[1];
    lbl_8040ADA8 = o->xf->pos[2];
    h = o->height * *o->yscale;
    for (;;) {
        c = *str;
        if (c == 0) {
            return;
        }
        g = &o->glyphs[c];
        y = lbl_8040ADA0;
        str++;
        y0 = y * 16.0f;
        if (c == ' ') {
            if (lbl_80407CC4 != 0.0f) {
                w = lbl_80407CC4 * *o->xscale;
            } else {
                w = *o->xscale * o->space;
            }
        } else {
            w = g->w * *o->xscale;
        }
        y1 = (y + h) * 16.0f;
        u0 = g->u;
        v0 = g->v;
        u1 = g->u + g->w;
        v1 = g->v + o->height;
        if (mode != 0 && c > 0x2F && c <= 0x39) {
            dw = o->glyphs[o->font->digit].w * *o->xscale;
            x = lbl_8040ADA4 + (dw - w);
            x1 = (x + w) * 16.0f;
            w = dw;
            x0 = x * 16.0f;
        } else {
            x1 = (lbl_8040ADA4 + w) * 16.0f;
            x0 = lbl_8040ADA4 * 16.0f;
        }
        if (c != ' ') {
            col = *o->colour;
            x0 = x0 * 0.0625f;
            x1 = x1 * 0.0625f;
            y0 = y0 * 0.0625f * 0.5f;
            y1 = y1 * 0.0625f * 0.5f;
            u0 = u0 * 0.001953125f;
            v0 = v0 * 0.001953125f;
            u1 = u1 * 0.001953125f;
            v1 = v1 * 0.001953125f;
            if (mode == 2) {
                vtx[0].x = x0;
                vtx[0].y = y0;
                vtx[0].z = lbl_8040ADA8;
                vtx[0].colour = col;
                vtx[0].u = u0;
                vtx[0].v = v1;
                vtx[1].x = x0;
                vtx[1].y = y1;
                vtx[1].z = lbl_8040ADA8;
                vtx[1].colour = col;
                vtx[1].u = u0;
                vtx[1].v = v0;
                vtx[2].x = x1;
                vtx[2].y = y0;
                vtx[2].z = lbl_8040ADA8;
                vtx[2].colour = col;
                vtx[2].u = u1;
                vtx[2].v = v1;
                vtx[3].x = x1;
                vtx[3].y = y1;
                vtx[3].z = lbl_8040ADA8;
                vtx[3].colour = col;
                vtx[3].u = u1;
                vtx[3].v = v0;
                NuRndrTriStrip3dClip(vtx, 4, &o->xf->mtx, o->mtl);
            } else {
                fn_800374C8(4, 7, o->mtl);
                fn_8003777C(u0, v0);
                fn_80037788(col);
                fn_800377AC(x0, y0, 0.0f);
                fn_8003777C(u1, v0);
                fn_80037788(col);
                fn_800377AC(x1, y0, 0.0f);
                fn_8003777C(u1, v1);
                fn_80037788(col);
                fn_800377AC(x1, y1, 0.0f);
                fn_8003777C(u0, v1);
                fn_80037788(col);
                fn_800377AC(x0, y1, 0.0f);
                fn_80037864();
            }
        }
        lbl_8040ADA4 += o->gap * *o->xscale + w;
    }
}


struct nustream_obj_s {
    u8 pad0[0x18];
    f32 scale; /* 0x18: fn_8002CAC8 */
    u8 pad1c[0x10];
    f32* x; /* 0x2c: fn_8002C9E4 */
    f32* y; /* 0x30: fn_8002C9E4 */
    u8 pad34[0x14];
    s32* p;
};

#define XF(o) ((struct nustream_xf_s*)(o)->p)

extern s32 lbl_80407CC0;
extern s32 lbl_8040AD20;
extern s32 lbl_8040AD24;
extern s32 lbl_8040AD28;
extern s32 lbl_8040AD2C;
extern f32 lbl_80407CA8;
extern f32 lbl_80407CAC;
extern f32 lbl_80407CB0;
extern f32 lbl_80407CB4;

void fn_8002C9E4(void* a, struct nustream_obj_s* b, f32 x, f32 y)
{
    if (lbl_80407CC0 == 4) {
        *b->x = x;
        *b->y = y;
        return;
    }
    *b->x = x * (f32)lbl_8040AD28 / (f32)lbl_8040AD20;
    *b->y = y * 0.5f * (f32)lbl_8040AD2C / (f32)lbl_8040AD24;
}

void fn_8002CAC8(void* a, struct nustream_obj_s* b, f32 x, f32 y, f32 z)
{
    if (lbl_80407CC0 != 4) {
        x = x * 640.0f * lbl_80407CA8 + (320.0f - lbl_80407CB0 * 320.0f);
        y = y * 448.0f * lbl_80407CAC + (224.0f - lbl_80407CB4 * 224.0f) - b->scale * *b->y;
    }
    XF(b)->pos[0] = x;
    XF(b)->pos[1] = y;
    XF(b)->pos[2] = z;
}

extern f32 lbl_80191E30[];
void fn_8002CB94(struct nustream_obj_s* b, f32 x, f32 y)
{
    if (lbl_80407CC0 == 4) { *b->x = x; *b->y = y; return; }
    *b->x = x * (f32)lbl_8040AD28 / (f32)lbl_8040AD20;
    *b->y = y * lbl_80191E30[0] * (f32)lbl_8040AD2C / (f32)lbl_8040AD24;
}

void fn_8002CC74(struct nustream_obj_s* b, f32 x, f32 y, f32 z)
{
    if (lbl_80407CC0 != 4) {
        x = x * 640.0f * lbl_80407CA8 + (320.0f - lbl_80407CB0 * 320.0f);
        y = y * 448.0f * lbl_80407CAC + (224.0f - lbl_80407CB4 * 224.0f) - b->scale * *b->y;
    }
    XF(b)->pos[0] = x;
    XF(b)->pos[1] = y;
    XF(b)->pos[2] = z;
}

void fn_8002CD0C(struct nustream_obj_s* b, struct nustream_mtx_s* m)
{
    XF(b)->mtx = *m;
}

extern s32 lbl_80407EC8;

void fn_8002CEF0(void)
{
}

void fn_8002CB8C(s32 a)
{
    lbl_80407EC8 = a;
}

void fn_8002CB54(void* a, struct nustream_obj_s* b, s32 c)
{
    *b->p = c;
}

void fn_8002CD00(struct nustream_obj_s* a, s32 c)
{
    *a->p = c;
}

void fn_8002CDA8(s32 a, s32 b)
{
    fn_8002C644((struct nustream_text_s*)a, (u16*)b, 0);
}

void fn_8002CDCC(s32 a, s32 b)
{
    fn_8002C644((struct nustream_text_s*)a, (u16*)b, 2);
}

void fn_8002CB60(s32 a, s32 b, s32 c, s32 d)
{
    fn_8002C644((struct nustream_text_s*)b, (u16*)c, d);
}

void fn_8002CD7C(u8* a, s32 b)
{
    fn_8002C3A0(*(s32*)(a + 0x38), *(s32*)(a + 0x10), b);
}

/* Material fields copied by fn_8002CDF0 (struct numtl_s offsets, see nu3dx.h). */
struct nustream_mtlattr_s {
    u32 w[5];
};

struct nustream_mtl_view_s {
    u8 pad0[0xB8];
    struct nustream_mtlattr_s attr; /* 0xB8 */
    u8 padCC[0x28];
    s32 tid; /* 0xF4 */
    u16 unkF8;
};

struct nustream_gobj_s {
    u8 pad0[6];
    u16 flags; /* 0x06 */
    s32 size; /* 0x08 */
    u8 padC[0x28];
    s32 p34;
    s32 p38;
    u8 pad3C[4];
    struct nustream_mtl_view_s* mtl; /* 0x40 */
};

extern void fn_8000759C(void* dst, void* src, s32 size);
extern struct nustream_mtl_view_s* fn_80024C18(s32 a);
extern void fn_80024CD4(struct nustream_mtl_view_s* mtl);

struct nustream_gobj_s* fn_8002CDF0(struct nustream_gobj_s* g, s32 flags, s32 c, u32* cur, u32* end)
{
    struct nustream_gobj_s* n;

    if (!(flags & 8)) {
        return g;
    }
    n = (struct nustream_gobj_s*)((*cur + 0xF) & ~0xF);
    *cur = (u32)n;
    *cur = (u32)n + g->size;
    if (*cur < *end) {
        fn_8000759C(n, g, g->size);
        n->flags |= 1;
        n->p34 = n->p34 - (s32)g + (s32)n;
        n->p38 = n->p38 - (s32)g + (s32)n;
    }
    n->mtl = fn_80024C18(1);
    n->mtl->attr = g->mtl->attr;
    n->mtl->tid = g->mtl->tid;
    n->mtl->unkF8 = g->mtl->unkF8;
    fn_80024CD4(n->mtl);
    return n;
}

/* Double-buffered render stream ("Render stream buffer overflow."). */
extern u32 lbl_8040A368; /* buffer 0 */
extern u32 lbl_8040A36C; /* buffer 1 */
extern u32 lbl_8040A370; /* bytes used this frame */
extern u32 lbl_8040A374; /* peak bytes used */
extern s32 lbl_80407ED0; /* current buffer index */
extern u32 lbl_80407ED4; /* buffer size */
extern u32 lbl_80407ED8; /* bytes used last frame */
extern u32 lbl_8040AD00; /* write cursor */
extern u32 lbl_8040AD88; /* buffer end */
extern char lbl_80191EE8[];
extern char lbl_80191F18[];
extern void (*fn_80009948(char* file, s32 line))();
extern u32 NuMemAlloc(u32 size, char* file, s32 line);
extern void* memset();

void fn_8002CEF4(void)
{
    switch (lbl_80407ED0) {
    case 0:
        lbl_8040A370 = lbl_8040AD00 - lbl_8040A368;
        break;
    case 1:
        lbl_8040A370 = lbl_8040AD00 - lbl_8040A36C;
        break;
    }
    lbl_80407ED8 = lbl_8040A370;
    if (lbl_8040A370 > lbl_8040A374) {
        lbl_8040A374 = lbl_8040A370;
    }
    if (lbl_8040A370 > lbl_80407ED4) {
        fn_80009948(lbl_80191EE8, 0x74)(lbl_80191F18);
        for (;;) {
        }
    }
    lbl_80407ED0 = (lbl_80407ED0 + 1) % 2;
    switch (lbl_80407ED0) {
    case 0:
        lbl_8040AD00 = lbl_8040A368;
        lbl_8040AD88 = lbl_8040AD00 + lbl_80407ED4;
        break;
    case 1:
        lbl_8040AD00 = lbl_8040A36C;
        lbl_8040AD88 = lbl_8040AD00 + lbl_80407ED4;
        break;
    }
    if (lbl_8040AD00 & 0xF) {
        lbl_8040AD00 = (lbl_8040AD00 + 0xF) & ~0xF;
    }
}

void fn_8002CFF8(u32 size, u32* mem)
{
    if (mem == NULL) {
        lbl_8040A368 = NuMemAlloc(size, lbl_80191EE8, 0x88);
        lbl_8040A36C = NuMemAlloc(size, lbl_80191EE8, 0x89);
    } else {
        u32 p0 = (*mem + 0x1F) & ~0x1F;
        u32 p1;
        *mem = p0;
        lbl_8040A368 = p0;
        p1 = (p0 + size + 0x1F) & ~0x1F;
        *mem = p1;
        lbl_8040A36C = p1;
        *mem = p1 + size;
    }
    lbl_80407ED0 = 0;
    lbl_80407ED4 = size;
    memset(lbl_8040A368, 0, size);
    memset(lbl_8040A36C, 0, size);
    switch (lbl_80407ED0) {
    case 0:
        lbl_8040AD00 = lbl_8040A368;
        lbl_8040AD88 = lbl_8040AD00 + lbl_80407ED4;
        break;
    case 1:
        lbl_8040AD00 = lbl_8040A36C;
        lbl_8040AD88 = lbl_8040AD00 + lbl_80407ED4;
        break;
    }
    if (lbl_8040AD00 & 0xF) {
        lbl_8040AD00 = (lbl_8040AD00 + 0xF) & ~0xF;
    }
}
