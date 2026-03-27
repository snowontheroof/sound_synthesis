# minisynth

A command-line music synthesizer written in C++ that converts text-based song notation into synthesized audio and plays it in real-time.

## How it works

Write a `.synth` file describing your song, run the program, and it plays through your speakers.

```
.synth file → Parser → Waveform generation → Mixing & effects → PortAudio playback
```

## Song format

`.synth` files are plain text with this structure:

```
tempo <BPM>
tracks <instrument1>,<instrument2>,...
volume <vol1>,<vol2>,...       (optional, defaults to 1)

<track_number>: <note sequence>
<track_number>: <note sequence>
```

**Instruments:** `sine`, `saw`, `square`, `triangle`, `kick`

**Notes:** `<pitch>[octave][/<duration>]`
- Pitch: `c d e f g a b`, with `#` (sharp) or `b` (flat)
- Octave: `0–8` (inherits from previous note if omitted, defaults to 4)
- Duration: number of beats (inherits from previous note if omitted)
- Rest: `r`
- Bar lines: `|` (optional, ignored by the parser)

**Example:**

```
tempo 160
tracks triangle,sine

1: | c d e f e d | c/3  g | c/5  r/1 |
2: | c2/3  d     | a2   g | c3/5 r/1 |
```

See the `examples/` directory for more complete songs (Super Mario, Imperial March, FF7, etc.).

## Building

**Dependencies:**
- C++20 compiler
- PortAudio
  - macOS: `brew install portaudio`
  - Linux: pre-built static library included in `lib/`

```bash
make        # build
make re     # clean rebuild
make debug  # build with debug symbols
make clean  # remove object files
make fclean # remove everything including binary
```

## Running

```bash
./minisynth <path/to/file.synth>

# Examples:
./minisynth examples/Simple_Tune.synth
./minisynth examples/Super_Mario.synth
```

## Project structure

```
├── src/
│   ├── main.cpp          # Entry point, PortAudio stream setup
│   ├── parsing.cpp       # .synth file parser
│   ├── samples.cpp       # Waveform generation (sine, saw, square, triangle, kick)
│   ├── trackToFile.cpp   # Track mixing, ADSR, low-pass filter, reverb
│   └── writeToWAV.cpp    # WAV file output
├── inc/
│   └── minisynth.hpp     # Type definitions and function declarations
├── examples/             # Sample .synth files
├── hertz.txt             # Note-to-frequency mapping (C0–B8)
└── lib/                  # Pre-built PortAudio static library (Linux)
```

## Audio pipeline

1. **Parsing** — reads tempo, instruments, volumes, and note sequences from the `.synth` file
2. **Sample generation** — each note is synthesized at 44100 Hz using the chosen waveform
3. **Mixing** — all tracks are combined into a single buffer, with per-track volume applied
4. **Effects** — ADSR envelope, 1000 Hz low-pass filter, and reverb are applied
5. **Playback** — the mixed buffer streams to your audio device via PortAudio
