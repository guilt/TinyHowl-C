# TinyHowl-C

C port of [TinyHowl](https://github.com/guilt/tinyhowl). Growable formant mouth.
Writes sound, not the knowledge base. Target: **ESP32-S3** (16 kHz I2S), host first.

Python stays the reference. This tree is the watch-sized twin so people can
collab in C without pulling a Python runtime onto a microcontroller.

## Bare minimum inventory

Not every IPA phone. UPSID/Maddieson: `/i a u/` sit in ~90% of languages; the
most common full system is `/i e a o u/`. Consonants collapse to three places
(labial, coronal, velar) and a handful of manners. Those atoms compose baby
babble and English CV words. Everything is generated in code — no recordings.

```
vowels     i e a o u schwa
fricatives s sh f h
stops      p t k
silence
```

Voiced stops / nasals / approximants live in the Python inventory and will
land here next. v0.1 C is the atoms an S3 can render in one 8 ms frame.

## Host

```bash
make test
make
./howl list
./howl a /tmp/a.wav
./howl dataset datasets/wav
```

## ESP32-S3

See `esp32/README.md`. Same `howl.c`, I2S TX instead of `fwrite`.

## License

MIT — `LICENSE.md`.
