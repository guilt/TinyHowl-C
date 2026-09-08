#include "howl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int fails = 0;
static void expect(int cond, const char *msg) {
    if (!cond) { fprintf(stderr, "FAIL %s\n", msg); fails++; }
}

int main(void) {
    const howl_atom_t *atoms = NULL;
    int n = howl_list(&atoms);
    expect(n >= 10, "inventory size");
    int16_t buf[HOWL_MAX_SAMPLES];
    int ns = howl_primitive("a", buf, HOWL_MAX_SAMPLES);
    expect(ns > HOWL_RATE / 8, "vowel length");
    int peak = 0;
    for (int i = 0; i < ns; i++) {
        int a = buf[i] < 0 ? -buf[i] : buf[i];
        if (a > peak) peak = a;
    }
    expect(peak > 200, "vowel energy");
    ns = howl_primitive("silence", buf, HOWL_MAX_SAMPLES);
    expect(ns > 0 && buf[0] == 0 && buf[ns / 2] == 0, "silence");
    expect(howl_primitive("click", buf, HOWL_MAX_SAMPLES) < 0, "unknown");
    const char *tmp = "howl-test.wav";
    ns = howl_primitive("s", buf, HOWL_MAX_SAMPLES);
    expect(howl_write_wav(tmp, buf, ns) == 0, "wav write");
    FILE *fp = fopen(tmp, "rb");
    expect(fp != NULL, "wav open");
    if (fp) {
        char magic[12];
        expect(fread(magic, 1, 12, fp) == 12, "wav header read");
        expect(memcmp(magic, "RIFF", 4) == 0 && memcmp(magic + 8, "WAVE", 4) == 0, "riff/wave");
        fclose(fp);
        remove(tmp);
    }
    if (fails) { fprintf(stderr, "%d failures\n", fails); return 1; }
    printf("ok %d atoms\n", n);
    return 0;
}
