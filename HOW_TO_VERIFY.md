# HOW_TO_VERIFY — TinyHowl-C

```bash
make test
make
./howl list
./howl a /tmp/a.wav
```

Expect:

- `test_host` prints `ok 14 atoms` and exits 0
- `/tmp/a.wav` starts with `RIFF....WAVE`
- unknown name (`./howl click`) exits 1
- frame math: 128 samples / 16000 = 8 ms (`HOWL_FRAME`)

Python twin: `github.com/guilt/tinyhowl` `bananey`.
