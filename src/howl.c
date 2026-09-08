#include "howl.h"

#include <math.h>
#include <stdio.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

static float clampf(float x, float lo, float hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

static int16_t to_i16(float x) {
    return (int16_t)(clampf(x, -1.f, 1.f) * 28000.f);
}

static float env(int i, int n) {
    if (n <= 1) return 1.f;
    float t = (float)i / (float)n;
    float a = t < 0.04f ? t / 0.04f : 1.f;
    float r = t > 0.88f ? (1.f - t) / 0.12f : 1.f;
    if (r < 0.f) r = 0.f;
    return a * r;
}

static float noise(int i) {
    float x = sinf((float)i * 12.9898f + 0.17f) * 43758.5453f;
    return (x - floorf(x)) * 2.f - 1.f;
}

int howl_silence_pcm(int16_t *out, int n) {
    for (int i = 0; i < n; i++) out[i] = 0;
    return n;
}

int howl_vowel_pcm(const howl_vowel_t *v, int16_t *out, int n) {
    float p0 = 0, p1 = 0, p2 = 0, p3 = 0;
    float i0 = (float)(2.0 * M_PI * v->f0 / HOWL_RATE);
    float i1 = (float)(2.0 * M_PI * v->f1 / HOWL_RATE);
    float i2 = (float)(2.0 * M_PI * v->f2 / HOWL_RATE);
    float i3 = (float)(2.0 * M_PI * v->f3 / HOWL_RATE);
    for (int i = 0; i < n; i++) {
        float e = env(i, n) * v->energy;
        float buzz = 0.45f * sinf(p0) + 0.22f * sinf(2.f * p0) + 0.10f * sinf(3.f * p0);
        float y = 0.28f * sinf(p1) + 0.42f * sinf(p2) + 0.12f * sinf(p3) + 0.18f * buzz;
        out[i] = to_i16(y * e);
        p0 += i0; p1 += i1; p2 += i2; p3 += i3;
    }
    return n;
}

int howl_noise_pcm(float energy, float color_hz, int16_t *out, int n) {
    float phase = 0.f;
    float inc = (float)(2.0 * M_PI * color_hz / HOWL_RATE);
    float prev = 0.f;
    for (int i = 0; i < n; i++) {
        float e = env(i, n) * energy;
        float white = noise(i);
        float hp = white - prev;
        prev = white;
        float y = 0.7f * hp + 0.3f * sinf(phase) * white;
        out[i] = to_i16(y * e);
        phase += inc;
    }
    return n;
}

typedef struct {
    const char *name;
    const char *kind;
    const char *ipa;
    float f0, f1, f2, f3, energy, color;
    int n;
    int is_noise;
} atom_t;

static const atom_t ATOMS[] = {
    {"silence", "silence", "0", 0, 0, 0, 0, 0, 0, 2880, 0},
    {"i", "vowel", "i", 380, 310, 2565, 3160, 0.65f, 0, 4160, 0},
    {"e", "vowel", "e", 380, 610, 2061, 2604, 0.65f, 0, 4160, 0},
    {"a", "vowel", "a", 380, 840, 1221, 2562, 0.65f, 0, 4160, 0},
    {"o", "vowel", "o", 380, 655, 941, 2530, 0.65f, 0, 4160, 0},
    {"u", "vowel", "u", 380, 345, 974, 2352, 0.65f, 0, 4160, 0},
    {"schwa", "vowel", "schwa", 380, 575, 1680, 2625, 0.65f, 0, 4160, 0},
    {"s", "fricative", "s", 0, 0, 0, 0, 0.45f, 2200, 3520, 1},
    {"sh", "fricative", "sh", 0, 0, 0, 0, 0.45f, 3200, 3520, 1},
    {"f", "fricative", "f", 0, 0, 0, 0, 0.40f, 800, 3520, 1},
    {"h", "fricative", "h", 0, 0, 0, 0, 0.28f, 400, 3520, 1},
    {"p", "stop", "p", 0, 0, 0, 0, 0.50f, 800, 352, 1},
    {"t", "stop", "t", 0, 0, 0, 0, 0.50f, 1800, 352, 1},
    {"k", "stop", "k", 0, 0, 0, 0, 0.50f, 1400, 352, 1},
};

int howl_list(const howl_atom_t **out) {
    static howl_atom_t pub[sizeof(ATOMS) / sizeof(ATOMS[0])];
    static int ready = 0;
    int n = (int)(sizeof(ATOMS) / sizeof(ATOMS[0]));
    if (!ready) {
        for (int i = 0; i < n; i++) {
            pub[i].name = ATOMS[i].name;
            pub[i].kind = ATOMS[i].kind;
            pub[i].ipa = ATOMS[i].ipa;
        }
        ready = 1;
    }
    *out = pub;
    return n;
}

int howl_primitive(const char *name, int16_t *out, int cap) {
    int n_atoms = (int)(sizeof(ATOMS) / sizeof(ATOMS[0]));
    for (int i = 0; i < n_atoms; i++) {
        if (strcmp(ATOMS[i].name, name) != 0) continue;
        int n = ATOMS[i].n;
        if (n > cap) n = cap;
        if (strcmp(ATOMS[i].kind, "silence") == 0)
            return howl_silence_pcm(out, n);
        if (ATOMS[i].is_noise)
            return howl_noise_pcm(ATOMS[i].energy, ATOMS[i].color, out, n);
        howl_vowel_t v = {ATOMS[i].f0, ATOMS[i].f1, ATOMS[i].f2, ATOMS[i].f3, ATOMS[i].energy};
        return howl_vowel_pcm(&v, out, n);
    }
    return -1;
}

int howl_write_wav(const char *path, const int16_t *pcm, int n) {
    unsigned int data_bytes = (unsigned int)n * 2u;
    unsigned int riff = 36u + data_bytes;
    unsigned char hdr[44] = {0};
    memcpy(hdr + 0, "RIFF", 4);
    hdr[4] = (unsigned char)(riff); hdr[5] = (unsigned char)(riff >> 8);
    hdr[6] = (unsigned char)(riff >> 16); hdr[7] = (unsigned char)(riff >> 24);
    memcpy(hdr + 8, "WAVEfmt ", 8);
    hdr[16] = 16;
    hdr[20] = 1;
    hdr[22] = 1;
    hdr[24] = (unsigned char)(HOWL_RATE);
    hdr[25] = (unsigned char)(HOWL_RATE >> 8);
    unsigned int br = HOWL_RATE * 2u;
    hdr[28] = (unsigned char)(br); hdr[29] = (unsigned char)(br >> 8);
    hdr[30] = (unsigned char)(br >> 16); hdr[31] = (unsigned char)(br >> 24);
    hdr[32] = 2;
    hdr[34] = 16;
    memcpy(hdr + 36, "data", 4);
    hdr[40] = (unsigned char)(data_bytes);
    hdr[41] = (unsigned char)(data_bytes >> 8);
    hdr[42] = (unsigned char)(data_bytes >> 16);
    hdr[43] = (unsigned char)(data_bytes >> 24);
    FILE *fp = fopen(path, "wb");
    if (!fp) return -1;
    if (fwrite(hdr, 1, 44, fp) != 44) { fclose(fp); return -1; }
    if (fwrite(pcm, 2, (size_t)n, fp) != (size_t)n) { fclose(fp); return -1; }
    fclose(fp);
    return 0;
}
