/* TinyHowl-C — formant mouth for a watch. 16 kHz, frame 128.
 * Host writes WAV. ESP32-S3 will push the same PCM to I2S.
 */
#ifndef TINYHOWL_HOWL_H
#define TINYHOWL_HOWL_H

#include <stddef.h>
#include <stdint.h>

#define HOWL_RATE  16000
#define HOWL_FRAME 128
#define HOWL_MAX_SAMPLES (HOWL_RATE)

typedef struct {
    float f0, f1, f2, f3;
    float energy;
} howl_vowel_t;

int howl_vowel_pcm(const howl_vowel_t *v, int16_t *out, int n);
int howl_noise_pcm(float energy, float color_hz, int16_t *out, int n);
int howl_silence_pcm(int16_t *out, int n);
int howl_primitive(const char *name, int16_t *out, int cap);
int howl_write_wav(const char *path, const int16_t *pcm, int n);

typedef struct {
    const char *name;
    const char *kind;
    const char *ipa;
} howl_atom_t;

int howl_list(const howl_atom_t **out);

#endif
