#include "howl.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static int mkdir_p(const char *path) {
    struct stat st;
    if (stat(path, &st) == 0) return 0;
    return mkdir(path, 0755);
}

int main(int argc, char **argv) {
    const char *name = argc > 1 ? argv[1] : "a";
    const char *out = argc > 2 ? argv[2] : NULL;

    if (strcmp(name, "list") == 0) {
        const howl_atom_t *atoms = NULL;
        int n = howl_list(&atoms);
        for (int i = 0; i < n; i++)
            printf("%s\t%s\t%s\n", atoms[i].name, atoms[i].kind, atoms[i].ipa);
        return 0;
    }

    if (strcmp(name, "dataset") == 0) {
        const char *dir = out ? out : "datasets/wav";
        mkdir_p("datasets");
        mkdir_p(dir);
        const howl_atom_t *atoms = NULL;
        int n = howl_list(&atoms);
        int16_t *buf = (int16_t *)malloc((size_t)HOWL_MAX_SAMPLES * 2);
        if (!buf) return 2;
        for (int i = 0; i < n; i++) {
            int ns = howl_primitive(atoms[i].name, buf, HOWL_MAX_SAMPLES);
            if (ns < 0) continue;
            char path[256];
            snprintf(path, sizeof(path), "%s/%s.wav", dir, atoms[i].name);
            if (howl_write_wav(path, buf, ns) != 0) {
                fprintf(stderr, "write failed %s\n", path);
                free(buf);
                return 1;
            }
            printf("wrote %s (%d samples)\n", path, ns);
        }
        free(buf);
        return 0;
    }

    int16_t buf[HOWL_MAX_SAMPLES];
    int n = howl_primitive(name, buf, HOWL_MAX_SAMPLES);
    if (n < 0) {
        fprintf(stderr, "unknown primitive '%s' (try: list)\n", name);
        return 1;
    }
    char path[256];
    if (out) snprintf(path, sizeof(path), "%s", out);
    else snprintf(path, sizeof(path), "%s.wav", name);
    if (howl_write_wav(path, buf, n) != 0) {
        perror(path);
        return 1;
    }
    fprintf(stderr, "wrote %s (%d samples @ 16 kHz)\n", path, n);
    return 0;
}
