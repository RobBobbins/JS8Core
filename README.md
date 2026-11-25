# js8core

js8core-plus is a working title for this fork. Any legitimate improvements
made here are meant to be shared back with JS8Core.

An independent, MIT-licensed implementation of the [JS8](https://www.js8call.com/) modem library.

`js8core` is a C++20 library implementing the core of the JS8
digital text/data protocol: the JSC frame codec, varicode framing, the JS8 modem
DSP (resampler + FIR), Costas/submode handling, the JS8 protocol engine, and the
decoder. It ships only **functional protocol data** (the JSC alphabet, JSC symbol
tables, and the on-air protocol constants) together with independently
written implementation code. Receive FIR coefficients and JSC lookup indexes
are now generated rather than copied from desktop source.

## What is JS8?

JS8 is a low-power, text-based digital text/data mode for amateur radio that uses a
8-FSK audio waveform and contains a built-in FEC scheme. JS8 allows
two-way text and low-bandwidth data communication over the air with very weak
signals. This library implements the signal-processing and framing core.

## Decoder results (js8core-plus)

Testing began in a lab: 48 sets of computer-made JS8 audio, the same 700
messages sent at all four speeds and eight signal levels, with fading, drift,
clock errors and a crowded band. That audio was not close to a real band. It
spread frames evenly over every speed and level, while the live band is 85%
Normal speed and 62% strong signals, and it was all made by js8core-plus's own
encoder. So testing moved to live audio: real JS8 traffic on 7.078 MHz from
public KiwiSDR receivers, decoded at the same moment by js8core (cba17f3),
JS8Call-improved v3.0.3 and js8core-plus.

The live test uses JS8Call-improved v3.0.3 with one bug fixed: the released JS8Call-improved v3.0.3 misses
stations that overlap a stronger one. The JS8Call-improved team already fixed
this in their development code, but it is not in a release yet, so we applied
their fix for the live test.

Live test, running now: https://va3oso.ca/js8core/livetest/

**Live scoreboard, 2026-11-24 15:29 UTC.** Frames decoded since 2026-11-22 22:16 UTC, public KiwiSDR receivers on 7.078 MHz.

Numbers are JS8 transmissions decoded; the percentages are how many more js8core-plus decoded.

| Speed | js8core (cba17f3) | JS8Call-improved v3.0.3 (bug fixed) | js8core-plus (6012b1e) | js8core-plus vs js8core (cba17f3) | js8core-plus vs JS8Call-improved v3.0.3 |
|---|---:|---:|---:|---:|---:|
| Normal | 10,164 | 9,956 | 11,557 | +13.7% | +16.1% |
| Fast | 1,456 | 1,347 | 1,613 | +10.8% | +19.7% |
| Turbo | 198 | 189 | 210 | +6.1% | +11.1% |
| Slow | 126 | 104 | 149 | +18.3% | +43.3% |
| **All** | **11,944** | **11,596** | **13,529** | **+13.3%** | **+16.7%** |

Lab test results, kept for reference: [docs/lab-tests.md](docs/lab-tests.md)

## How the decoder was improved (js8core-plus)

Every change below was measured before it was kept. Each was compared with
the decoder just before it, on the test set named, so the gains are not added
up into the lab totals in [docs/lab-tests.md](docs/lab-tests.md). Numbers in square brackets refer to
[Sources](#sources) at the bottom of this file. The full method and every
measurement are in
[docs/decoder-research-2026-11-18.md](docs/decoder-research-2026-11-18.md).

The js8core (cba17f3) decoder runs the standard decode only (belief propagation on the
tone readings). Signals it can't finish with that are lost. Most of the gain
comes from four kinds of change: a last-try decode for those signals, better
tone readings, reading through fast fading, and a Deep setting that spends
more time on each signal.

### Last-try decoding

| Commit | Change | Source | Measured |
|---|---|---|---|
| `6a85ca0` | **Last-try decode (ordered statistics decoding, order 2).** When the standard decode fails on a signal with at least 9 of 21 sync tones right, the most reliable bits are taken as fixed and the rest is searched; the answer is kept only if it stays within an evidence limit and passes the CRC. | [S1] (Lin and Costello, as cited there) | 95 → 110 of 192 test signals, 0 wrong, +9% time |
| `02d757d` | **Deeper last-try search**, orders 3 and 4, using Wu and Hadjicostis' preprocessing. | [S5] | +0.1 dB without fading, +0.06 dB with fading, no new wrong messages, no slower |
| `e186de1`, `7231c03` | **Runner-up check (Forney's rule)**: a last-try answer must beat the second-best answer by a set share of the evidence. Deep first, then Normal mode. | [S25] [S26] [S27] | About 41% fewer chances of a wrong message for 1.6% fewer messages, in each mode |
| `02e9174` | **Faster last-try scoring**: only the bits that differ are scored. | none (engineering) | Identical results; crowded Normal-speed time 2.64 → 1.95 times js8core (cba17f3) |
| `f173817` | **Last try ordered by 3 damped decoding rounds**, instead of by the raw readings; the order-4 search is skipped then. | [S33] [S34] | Normal mode +2.3% messages, Deep +63, Fast and Turbo +24 to +40, same speed |
| `a962669` | **Runner-up gap set to 0.0025** of the evidence. | [S26] [S27] | Against the decoder before `f173817`: every speed and mode +15 to +59 messages with 9% to 22% fewer wrong-message chances |

### Reading the tones

| Commit | Change | Source | Measured |
|---|---|---|---|
| `2ea88b2` | **3-tone block reading**: bit readings built from the best match over all 512 three-tone sequences. JS8 tones are phase-continuous, so a block adds up in phase. Used when the other passes fail. | [S1] | 110 → 117 of 192 test signals, 0 wrong, +15% time |
| `15d0cd2` | **2-tone block reading** in the standard passes, and the **Deep setting**. | [S1] | Normal mode decodes a little more with fewer wrong messages; Deep +0.1 dB on fading |
| `4639d53` | **Reading scale 6.0 in Deep** (Normal mode keeps 2.83): how strongly the tone readings are trusted. | [S1] [S14] [S15] [S16] | Deep: fading +4, crowded +3 messages, none lost, no wrong messages |
| `4a8ec2a` | **Iterative demapping**: after every decoding round the tone readings are rebuilt from what the decoder now believes about each bit. Runs at every speed, in both modes. | [S4] [S43] [S44] | Normal-speed sets +5.5% messages (2,966 → 3,128), Deep +4.3%, Fast +5.9%, Turbo +4.5%, fewer wrong-message chances, no extra time at Normal speed |
| `2428ff6` | **Damping 0.1**: every decoding message, in both directions, keeps 0.1 of its value from the round before, which curbs round-to-round swings. No published value fits JS8's code; 0.1 and 0.2 gained, 0.5 lost. | [S23] [S66] [S67] [S68] | 10 Hz fading +9% (1,274 → 1,387 of 2,800 signals, Deep 1,477 → 1,589), elsewhere +0.4% to +1%; same wrong messages; Deep 4% to 5% faster crowded |
| `803477b` | **Bolder spread reading**: the spread-aware tone reading trusted its bits too little; measured against the true bits on fade-free audio, its most informative scale was 1.4 times what it gave. Its readings are now scaled by 1.4 in Normal mode and 2.5 in Deep (the decoder gains more at bolder scales than the most informative one, as with Deep's 6.0). The wide spread reading is unchanged. | [S98] [S107] [S109] | Test sets at all speeds (53,286 signals): Normal mode +1.4% messages (26,049 → 26,402), wrong 4 → 3; Deep +1.5% (27,492 → 27,902), wrong 10 → 10; real-style traffic +0.3% whole messages (136,723 → 137,164 of 268,800), wrong frames 13 → 12; crowded time +3% Normal mode, +8% Deep |

### Fast fading

The js8core (cba17f3) decoder decodes nothing at 10 Hz spread, even on strong
signals. WSJT-X's FT8 decodes half of such signals at -8.6 dB [S1].

| Commit | Change | Source | Measured |
|---|---|---|---|
| `5ad9bbc` | **Spread-aware tone reading** and a last try on it, used when the sync tones show the signal is spread. | [S42] | 10 Hz test sets 101 → 253 messages, ordinary sets unchanged, +0.9% time |
| `6cadbfb` | **Deep orders the spread last try with iterative demapping.** | [S4] [S33] [S34] | 10 Hz, Deep: 587 → 596 messages |
| `07bdf6f` | **Wide spread reading** (Normal and Slow speed): the signal is cut out 4 tones wider each side and each tone is scored from bins up to 2 tones away. | [S1] [S42] [S56] | 10 Hz sets +34% (587 → 787), new test sets +38%, fewer wrong-message chances, +0.6% time; on the high-latitude 10 Hz set half the signals decode at -8 dB |
| `3fbf2c3` | **Wide reading reaches 4 tones at Slow speed**, so the same width in Hz as at Normal speed. | [S57] | Slow 10 Hz: 0 → 51 of 800, 0 wrong, no extra time |
| `798e6ce` | **Fit smoothing and weak-sync rescue**: under heavy spread the time and frequency fit uses a sync score smoothed across neighbouring tones, and a signal whose sync looks weak is kept if the spread reading confirms it. Deep retries every failed signal with the smoothed fit. | [S57] | Normal mode: Normal-speed 10 Hz +11%, Slow 10 Hz 49 → 74 of 800, no losses; Deep +5.9% ordinary messages |

### Fitting time and frequency

| Commit | Change | Source | Measured |
|---|---|---|---|
| `4f21eb5` | **Candidate start time without its lag, and a second time fit.** The candidate search's window peaks half a symbol in, so candidate start times came out 1.5 coarse steps early (Normal speed -0.043 s, Fast -0.039 s, Turbo -0.020 s, Slow -0.107 s); they now count from the true step. After the frequency fit, time is searched again at the corrected frequency over half the first window, as WSJT-X does. | [S82] [S83] | Normal-speed sets +13.5% messages (3,152 → 3,578; Deep +16.4%), Fast +66%, Turbo +62%, Slow +21%, 10 Hz fading +12%; wrong-message chances down on the signal sets except Slow and 10 Hz Deep, up on noise-only files; crowded time Normal speed 2.04 times js8core (cba17f3) |
| `4e9ecd2` | **Whole frame read at Normal, Fast and Turbo speeds.** Each frame was read only to 12.64 s after the start of the period (Normal speed), but a frame starts 0.5 s plus its time offset into the period, so its last sync tones, and above about 0.62 s offset some data tones, read as silence. It is now read to 2,812 samples (14.06 s at Normal speed), as WSJT-X's FT8 decoder and JS8Call-improved v3.0.3 do. Slow keeps the old length: there it gained 0.7% messages for 15% to 20% more wrong-message chances. | [S82] [S102] | Normal-speed sets +4.5% messages (Deep +4.2%), real-style traffic at Normal speed +1.1% frames (6 sets), Fast +5.1%, Turbo +17.2%; wrong-message chances 4% to 43% fewer; no test set more than 5.1% slower; decodes the two live-test frames js8core-plus missed and JS8Call-improved v3.0.3 got |

### Signal subtraction

| Commit | Change | Source | Measured |
|---|---|---|---|
| `f1e6ca9` | **Subtraction window of 3 symbols** at Normal, Fast and Slow speed (5,760, 3,600 and 11,520 samples; it was 1,400 samples at every speed). After a signal is decoded, its strength and phase are measured over this window and the signal is removed from the audio, so weaker signals under it can be found. Turbo keeps 1,400 samples (already 2.33 symbols; shorter and longer measured no better). WSJT-X's FT8 uses 2.08 symbols. | [S111] [S112] [S113] | Real-style busy-band test audio: Normal speed +2.3% messages (8,686 → 8,889), Fast +1.3% (7,523 → 7,619), Slow +0.8% (6,945 → 7,002, half the levels), Turbo unchanged. Ordinary band (4 stations per file): no change. Extreme band (37 to 60 stations on top of each other; fresh sets, not used to choose the length): Normal speed +19.3% (1,761 → 2,100 of 7,200; Deep +20.8%), Fast +7.8% (Deep +7.5%), Slow +4.2% (Deep +3.7%). 0 or 1 wrong message per run, as before; no change in decode time measured. Real-style traffic, all 48 sets [S130]: +0.9% whole messages (137,164 → 138,380 of 268,800; busy band +2.6%, Turbo unchanged); wrong frames 12 → 21 (9 new, 8 of them at Slow speed, 0 gone) |
| `6012b1e` | **Last-try position rule.** A last-try result (ordered statistics decoding, passes 8 to 11) is refused when a frame with different text was already decoded within 3 Hz and 0.05 s of it in the same period. The 9 wrong frames the subtraction window added were all of this kind: what the subtraction left of a decoded signal, read by the last try as a new frame. The same frame decoded again at its own place is still accepted, because subtracting it a second time helps the decodes after it (refusing that too lost 13 right frames in a test copy). | [S132] [S133]; no other decoder read checks position against earlier decodes | Real-style traffic, all 48 sets: wrong frames 21 → 12 (exactly the 9 new ones), 0 of 393,902 right frames changed, whole messages unchanged (138,380). 24 fresh sets (Normal and Slow speed, not used to choose the limits): wrong frames 14 → 8 (Deep 53 → 35), 0 right frames changed (226,539; Deep 231,257) |

### Deep's time

| Commit | Change | Source | Measured |
|---|---|---|---|
| `d87fe38` | **Deep's retry skips a fit that repeats the first attempt**; that retry could only fail again. | none (measured: 0 of about 2,470 such retries decoded) | Same messages; crowded Deep time −14% at Normal speed, −11% at Slow |

### Other changes

- `b85d0af` **Callsign hints**: a weak directed frame that fails every pass is
  tried again with its callsigns assumed (to the operator's own callsign, or
  from a station heard in the last 10 minutes). Off unless the host sets the
  operator's callsign; the lab results were measured with hints off.
  Measured: frames to the own callsign decode about 50% more often, 0 wrong.
- `a152060` Host hooks for Easy JS8 (not a decoder change).
- `e446576` Directed, compound and frame building like JS8Call-improved v3.0.3 (not a
  decoder change).
- `18b266a`, `7385c6e` Waterfall spectrum like JS8Call's dataSink: one
  spectrum per 3,096 samples, leftover samples kept for the next one (not a
  decoder change).

### Tested, not built

- **Give-up rule at set rounds** [S21]: on JS8's 87-check code the
  failed-check count does not tell signals that will decode from those that
  won't. Every setting lost 2 to 20 of 3,432 messages, for at most 9% less
  time.
- **Heartbeat and CQ hints** [S84]: one hint per heard station for its
  heartbeat and CQ frames. Those frames +26% to +54% at every speed, 0 wrong
  messages, but crowded-band time +10% to +38%.
- **Re-reading the same audio with small changes** [S73] [S81]: +4.5%
  messages in Deep on top of the time-fit fixes, for twice the wrong-message
  chances.
- **Heartbeat replies** [S106]: a hint "anyone to the answered station,
  HEARTBEAT SNR" decoded replies from stations not on the heard list
  1,174 → 2,002 of 5,000 (+71%), 0 wrong messages; matching weak signals
  against a list of likely replies decoded listed replies 226 → 364 of 800
  (+61%). Both need engine work (the answered station, more than 16 heard
  stations).
- **Reading scale per bit position** [S107]: the three bits of a tone want the
  same scale (within 0.1), so a scale per position gains nothing. The same
  measurement led to `803477b`.
- **CRC bits inside the last try** [S108]: with the acceptance limit set back
  to today's wrong-message risk, it decodes the same as today (+0.5% to
  −0.5%).
- **Per-bit starting belief from real traffic** (as ft8mon does) [S115]
  [S124] [S125]: counted, not tested. A real Normal-speed frame has 3.3
  predictable bits of 72 (1.8 to 3.7 on frames held out), a tenth or less of
  what the published gains need.
- **Frame-end correction for the subtraction** (WSJT-X and WSPR use one)
  [S113] [S114] [S126]: no effect (Normal speed: 12,242 messages without,
  12,241 with).

## Authors

`js8core` is authored by Kelly-Shane Fuller
([`punk-kaos`](https://github.com/punk-kaos)).

JS8 is a public amateur-radio digital mode; this library implements that protocol
as an independent, MIT-licensed reimplementation.


## License

`js8core` is released under the **MIT License**. All implementation code in this
repository was written for `js8core`.

Third-party headers bundled under
`vendor/` and linked header-only libraries carry their own licenses:

| Component | License |
|---|---|
| `js8core` source | MIT |
| `vendor/CRCpp/CRC.h` | MIT / custom (standard CRC-16/CRC-32 reference) |
| `vendor/pocketfft/pocketfft_hdronly.h` | BSD-3-Clause (Max-Planck-Society / P. Bell / M. Reinecke) — header-only FFT|
| Eigen (header-only) | MPL-2.0 |
| `<android/log.h>` shim (`vendor/android/`) | MIT (desktop fallback only; real header used on Android) |

See [LICENSE](LICENSE) for the full text.

## Repository layout

```
js8core/
├── include/js8core/     # public headers (the library API)
├── src/                 # independent implementations
│   ├── protocol/        # JSC, split varicode codec modules, submode, Costas
│   │                    #   tables.hpp: wire identities / codebooks
│   │                    #   varicode_policy.hpp: app behavior and power references
│   │                    #   derived JSC text search index
│   ├── decoder/         # legacy_decoder.cpp (JS8 decoder)
│   ├── dsp/             # flatten.cpp, resampler.cpp (JS8 modem DSP + FIR)
│   ├── engine/          # engine.cpp (JS8 protocol engine)
│   └── tx/              # modulator.cpp (JS8 modulator)
├── tools/               # verification helpers (round-trip checks)
├── vendor/              # small shim libraries (see table above)
└── CMakeLists.txt
```


## Build

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --target js8core -j
```

Notes:

- The core library builds with the default target. Focused test tools are
  `EXCLUDE_FROM_ALL` and should be built explicitly before running CTest.
- The library links nothing of its own — Eigen and pocketFFT are all
  header-only. The JS8 detection path in `legacy_decoder.cpp` uses
  `vendor/pocketfft/pocketfft_hdronly.h` (BSD-3-Clause) for its FFTs, and
  Eigen (MPL-2.0) only in the spectrum flattener and decoder baseline fit, so the
  library has no external link dependency and no GPL dependency.
- `<android/log.h>` is an external NDK header on Android. A functional
  `vendor/android/log.h` shim provides a desktop fallback (`__ANDROID__` is not
  defined) and otherwise defers to the real NDK header via `#include_next`.

The [protocol data and provenance inventory](docs/protocol-data-provenance.md)
describes which exact on-air tables are retained and which indexes and
receive filters are derived. The legacy `commons.h` compatibility scaffold
is not distributed: consumers use the `include/js8core/` C++ API and
`protocol/constants.hpp` instead. Application-level command behavior and
power presentation have their own internal policy specification in
`src/protocol/varicode_policy.hpp`. Its compatibility decisions are
tested independently of the wire tables.

## Verification

Two focused checks are provided in `tools/`:

- `verify_jsc_roundtrip.cpp` — round-trips the JSC codec: asserts
  `JSC_SIZE == 262144`, and checks single-char and multi-char
  `compress()` → `decompress()` correctness (expected: 41/41 single-char and
  30000/30000 multi-char round-trips).
- `varicode_roundtrip.cpp` — round-trips the varicode framing against the
  desktop reference (57 frames).

```bash
cmake --build build --target js8core-crc-test verify_jsc_roundtrip js8core-varicode-test js8core-policy-test js8core-clock-math-test js8core-vox-test
ctest --test-dir build --output-on-failure
./build/verify_jsc_roundtrip          # prints: PASS
./build/js8core-varicode-test         # prints: 57 OK frames
```

## Audio capture requirements

The decode engine operates internally at 12 kHz. Capture accepts 16-bit
mono or interleaved multi-channel PCM. Integral ratios (including 48 kHz)
use an independently designed 97-tap Blackman-windowed FIR;
fractional ratios such as 44.1 kHz use a streaming,
band-limited 49-tap, 256-phase windowed-sinc resampler. Source buffers
must arrive in order, with `AudioInputBuffer.captured_at` marking the first
frame of each buffer in the steady-clock domain. Discontinuities over 250 ms
trigger a fresh UTC alignment. Capture is serialized internally.


## Platform integration and transmission timing

Outgoing frames are scheduled using `AudioOutputBuffer.playback_at`, the
steady-clock timestamp at which the **first** output sample is expected to reach
the audio device. Without a timestamp, the engine falls back to callback time;
that can introduce a device-buffer latency offset. The fractional TX resampler
uses a windowed-sinc polyphase reconstruction filter to suppress imaging.

If a `RigControl` adapter is supplied, it must report a connected, current
`RigState` in its state callback before TX can begin and report confirmation
after a PTT state change. The engine requests PTT on a control worker and
mutes payload audio until keyed state is confirmed. It requests key-up on TX
completion or cancellation. `RigControl::apply()` must remain nonblocking and
preserve tuning/mode when given an otherwise unchanged state. When no rig
adapter is supplied, PTT is the responsibility of the host application.

RX snapshots are copied on the decode worker in bounded chunks rather than
holding the capture mutex for the full ring. Discontinuous or stale snapshots
are discarded.

## Android-port API compatibility

The public C++ engine, events, frame codec, and callbacks are source-compatible
with the original Android-port JS8Core API. The legacy JNI adapter may supply an
offline rig stub even though its host manages PTT: an explicit
`set_tx_ready(true)` enables that host-managed path without relaxing
confirmed PTT requirements for an online rig. The grant expires when the host
calls `set_tx_ready(false)`, the session ends, or a real rig comes online.

Legacy JNI capture may timestamp callback delivery instead of the first audio
sample. When timestamps move backwards relative to the sample timeline, the
engine switches to contiguous-sample accounting for that receive session.
This prevents accelerated WAV submissions from clearing the capture ring.
Actual forward timestamp gaps still cause realignment while source timestamps
are trustworthy.

The original header `js8core/protocol/time_utils.hpp` is provided with
independently written, wire-compatible time packing helpers. The existing
`js8core::js8core` CMake target is preserved, and Eigen can be located from
a sibling host project's `vendor/` directory.

Run `js8core-android-compat-test` alongside the native regressions to check
external PTT, offline status callbacks, and accelerated audio submission.

## Building and testing the independent package

```sh
cmake -S . -B build -DBUILD_TESTING=ON -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2 --target js8core js8core-crc-test js8core-varicode-test verify_jsc_roundtrip js8core-vox-test js8core-fractional-rx-test js8core-decoder-roundtrip-test js8core-engine-smoke-test js8core-rig-test
ctest --test-dir build --output-on-failure
cmake --install build --prefix /tmp/js8core-stage
cmake -S tools/package_smoke -B build/package-smoke -DCMAKE_PREFIX_PATH=/tmp/js8core-stage
cmake --build build/package-smoke
```

The exported target is `js8core::js8core` and may be consumed using
`find_package(js8core CONFIG REQUIRED)`. GitHub Actions now builds Debug and
Release, executes the decoder/engine/PTT tests, and checks an installed
downstream consumer. Independent desktop WAV SNR parity tests should continue
to be maintained before cutting a release.

## Sources

Published work and code read for the decoder research in js8core-plus. The
numbers match [docs/decoder-research-2026-11-18.md](docs/decoder-research-2026-11-18.md),
which also lists our own test runs (S3, S6, S9, S10, S13, S19, S29 to S32,
S36, S39 to S41, S45 to S51, S54, S55, S60 to S65, S71, S72, S81, S84, S85, S103, S104, S106 to S109, S125, S126, S130, S131, S133). Where only part of a
source was read, it says so. js8core-plus's decoder changes copy no code from
these sources.

| # | Source | Read |
|---|---|---|
| S1 | S. Franke, B. Somerville, J. Taylor, "The FT4 and FT8 Communication Protocols", QEX, July/August 2020, pages 7 to 17. https://wsjt.sourceforge.io/FT4_FT8_QEX.pdf | In full |
| S2 | K. Goba, ft8_lib (MIT license). https://github.com/kgoba/ft8_lib | Read |
| S4 | M. C. Valenti, S. Cheng, "Iterative demodulation and decoding of turbo-coded M-ary noncoherent orthogonal modulation", IEEE Journal on Selected Areas in Communications, vol. 23, no. 9, September 2005, pages 1739 to 1747. https://community.wvu.edu/~mcvalenti/documents/ValentiChengJSAC2005.pdf | In full |
| S5 | Y. Wu, C. N. Hadjicostis, "Soft-Decision Decoding of Linear Block Codes Using Preprocessing and Diversification", IEEE Transactions on Information Theory, vol. 53, no. 1, January 2007, pages 378 to 393. DOI 10.1109/TIT.2006.887478 | In full |
| S7 | Recommendation ITU-R F.1487, "Testing of HF modems with bandwidths of up to about 12 kHz using ionospheric channel simulators", ITU, 2000. https://www.itu.int/rec/R-REC-F.1487-0-200005-I/en | In full |
| S8 | Li and Yu, "Sliding Window-Aided Ordered Statistics Decoding for Short LDPC Codes", arXiv 2404.14165, 2024 (version 5 published in IEEE Communications Letters as "Boosting Ordered Statistics Decoding of Short LDPC Codes with Simple Neural Network Models"). https://arxiv.org/abs/2404.14165 | In full (v5) |
| S14 | S. Cheng, M. C. Valenti, D. Torrieri, "Turbo-NFSK: Iterative Estimation, Noncoherent Demodulation, and Decoding for Fast Fading Channels", MILCOM 2005, https://community.wvu.edu/~mcvalenti/documents/Milcom2005.pdf ; and "Robust Iterative Noncoherent Reception of Coded FSK over Block Fading Channels", IEEE Transactions on Wireless Communications, vol. 6, no. 9, September 2007, pages 3142 to 3147 | Both in full |
| S15 | WSJT-X `lib/ft8/ft8b.f90`. https://sourceforge.net/p/wsjt/wsjtx/ci/master/tree/lib/ft8/ft8b.f90 | Read (bit metric and scaling) |
| S16 | Rhizomatica mercury, pull request 359, "Ldpc decoder". https://github.com/Rhizomatica/mercury/pull/359 | In part |
| S20 | J. Taylor, "Re: [wsjt-devel] FT8 notes", wsjt-devel mailing list, 30 June 2017. https://sourceforge.net/p/wsjt/mailman/message/35921298/ | Read |
| S21 | T. Mohsenin, H. Shirani-mehr, B. Baas, "Low Power LDPC Decoder with Efficient Stopping Scheme for Undecodable Blocks", ISCAS 2011. http://vcl.ece.ucdavis.edu/pubs/2011.05.ISCAS.LDPC/ldpc.iscas.pdf | In full |
| S22 | E. Amador, R. Knopp, V. Rezard, R. Pacalet, "Hybrid Iteration Control on LDPC Decoders", Eurecom. https://www.eurecom.fr/en/publication/3166/download/cm-publi-3166.pdf | In full |
| S23 | M. Lian, F. Carpi, C. Häger, H. D. Pfister, "Learned Belief-Propagation Decoding with Simple Scaling and SNR Adaptation", arXiv 1901.08621, 2019. https://arxiv.org/abs/1901.08621 | In full |
| S24 | J. Wu, B. R. Vojcic, J. Sheng, "Stopping Criteria for Iterative Decoding based on Mutual Information", arXiv 1302.1461, 2013; and S. Kudekar, T. Richardson, A. Iyengar, "The Effect of Saturation on Belief Propagation Decoding of LDPC Codes", arXiv 1403.3678 | In full |
| S25 | WSJT-X `lib/ft8/ft8b.f90` and `lib/ft8_decode.f90`. https://github.com/WSJTX/wsjtx/blob/master/lib/ft8/ft8b.f90 | Read (sync minimum, last-try acceptance, false-decode rule) |
| S26 | A. Sauter, A. O. Kislal, G. Durisi, G. Liva, B. Matuz, E. G. Ström, "Undetected Error Probability in the Short Blocklength Regime: Approaching Finite-Blocklength Bounds with Polar Codes", arXiv 2503.02782, 2025 | In part (Forney's rule, threshold test, results) |
| S27 | P. Yuan, M. Médard, K. Galligan, K. R. Duffy, "Soft-output (SO) GRAND and Iterative Decoding to Outperform LDPCs", arXiv 2310.10737v5, 2024 | In part (Forney's approximation) |
| S33 | M. P. C. Fossorier, "Iterative reliability-based decoding of low-density parity check codes", IEEE Journal on Selected Areas in Communications, vol. 19, no. 5, May 2001, pages 908 to 917. DOI 10.1109/49.924874 | In full |
| S34 | W. Zhang, C. Yue, Y. Li, B. Vucetic, "Efficient Near Maximum-Likelihood Reliability-Based Decoding for Short LDPC Codes", arXiv 2306.00443v3, 2023 | In full |
| S35 | Ma Zhuo, Du Shuanyi, "Improved Reliability-Based Iterative Decoding of LDPC Codes Based on Dynamic Threshold", ETRI Journal, vol. 37, no. 4, August 2015. DOI 10.4218/etrij.15.0113.1265 | In part |
| S37 | wsjt-devel mailing list, 2017: S. Franke, https://sourceforge.net/p/wsjt/mailman/message/35921321/ ; B. Somerville quoting J. Taylor, https://sourceforge.net/p/wsjt/mailman/message/35921283/ | Read |
| S38 | R. Morris, ft8mon, `ft8.cc`. https://github.com/rtmrtmrtmrtm/ft8mon | Read (last-try gate) |
| S42 | mfsk-core 0.4.3, `src/fec/qra/fast_fading.rs` (Rust port of the WSJT-X Q65 fast-fading metric, Palermo and Taylor). https://docs.rs/crate/mfsk-core/0.4.3/source/src/fec/qra/fast_fading.rs | Read for the method |
| S43 | T. Ferrett, M. C. Valenti, "Noncoherent Analog Network Coding using LDPC-coded FSK", arXiv 1703.01432, 2017; and "Noncoherent LDPC-Coded Physical-Layer Network Coding using Multitone FSK", arXiv 1801.10370, 2018 | 2017 in full; 2018 in part |
| S44 | A. Guillén i Fàbregas, A. Grant, "Parameter Free Iterative Decoding Metrics for Non-Coherent Orthogonal Modulation", draft 2006. https://www.upf.edu/documents/221459052/223029041/letter_ncfsk.pdf | In full |
| S52 | mfsk-core 0.4.3, `src/q65/search.rs` and `src/q65/sync_pattern.rs` (Rust port of WSJT-X Q65). https://docs.rs/crate/mfsk-core/0.4.3/source/src/q65/search.rs | Read for the search method |
| S56 | WSJT-X Q65 sources in `lib/qra/q65/` (`q65.c`, `fadengauss.c`, `fadenlorentz.c`, `q65_subs.c`) and `lib/q65_decode.f90`, https://github.com/WSJTX/wsjtx/tree/master/lib/qra/q65 ; mfsk-core 0.4.3 Q65 files, https://docs.rs/crate/mfsk-core/0.4.3/source/ | Read for the fast-fading window width |
| S57 | WSJT-X `lib/qra/q65/q65.f90`, `lib/qra/q65/q65_loops.f90`, `lib/q65_decode.f90`, `lib/smo121.f90`. https://github.com/WSJTX/wsjtx | Read |
| S66 | M. Fossorier, R. Palanki, J. Yedidia, "Iterative Decoding of Multi-step Majority Logic Decodable Codes", MERL TR2003-107 (Turbo Codes symposium 2003), https://www.merl.com/publications/docs/TR2003-107.pdf ; revised as R. Palanki, M. Fossorier, J. Yedidia, IEEE Transactions on Communications, vol. 55, no. 6, June 2007, pages 1099 to 1102, MERL TR2007-038, https://www.merl.com/publications/docs/TR2007-038.pdf | Read in full (both) |
| S67 | M. G. Stepanov, M. Chertkov, "Improving Convergence of Belief Propagation Decoding", arXiv cs/0607112, 2006. https://ar5iv.arxiv.org/html/cs/0607112 | Text read in full; figures are images, captions and text only |
| S68 | M. Pretti, "Lowering the error floor of Gallager codes: a statistical-mechanical view", arXiv 1407.0521, 2014. https://ar5iv.labs.arxiv.org/html/1407.0521 | Text read in full |
| S69 | V. Garcia Satorras, M. Welling, "Neural Enhanced Belief Propagation on Factor Graphs", AISTATS 2021, PMLR vol. 130. https://proceedings.mlr.press/v130/garcia-satorras21a/garcia-satorras21a.pdf | Read in full |
| S70 | WSJT-X 2.7.0 (Debian wsjtx 2.7.0+repack-1), `lib/ft8/bpdecode174_91.f90` and `lib/ft8/decode174_91.f90`. https://sources.debian.org/src/wsjtx/2.7.0%2Brepack-1/lib/ft8/bpdecode174_91.f90 | Both files read in full; `platanh` and `osd174_91` not read |
| S82 | WSJT-X `lib/ft8/ft8b.f90`, GitHub mirror master. https://raw.githubusercontent.com/WSJTX/wsjtx/master/lib/ft8/ft8b.f90 | Lines 102 to 152 (time search, frequency search, second time search) |
| S83 | WSJT-X `lib/ft8/sync8.f90`, GitHub mirror master. https://raw.githubusercontent.com/WSJTX/wsjtx/master/lib/ft8/sync8.f90 | Lines 28 to 50 and 123 (candidate spectra and candidate time) |
| S102 | JS8Call-improved v3.0.3 `JS8_Mode/JS8.cpp` (tag `v3.0.3`). https://raw.githubusercontent.com/JS8Call-improved/JS8Call-improved/v3.0.3/JS8_Mode/JS8.cpp ; WSJT-X `lib/ft8/sync8d.f90`, GitHub mirror master | `NP2` definitions (lines 197 to 364) and the symbol reading and sync check (lines 1150 to 1445); `sync8d.f90` `NP2` and Costas sums |
| S98 | J. Wu, M. El-Khamy, J. Lee, I. Kang, "BICM Performance Improvement via Online LLR Optimization", arXiv 1303.4452 (IEEE WCNC 2013) | In full except figures |
| S73 | T. Sokcevic (CE3TSK), "Deterministic Ensemble Decoding for FT8 and FT4: Re-Reading the Same Audio, and the Runtime Split That Pays for It", August 2026. https://ce3tsk.com/download/JTDX_CONTEST_DECODER_PAPER.pdf (derivative of JTDX, GPL) | Read in full (text extracted locally with pdftotext) |
| S111 | T. Sokcevic (CE3TSK), "The FT8 decoder of the JTDX CE3TSK fork: How it works, what changed, the presets", 6 September 2026. https://ce3tsk.com/FT8_DECODER.pdf (derivative of JTDX, GPL) | Text extracted locally with pdftotext (1,435 lines): lines 1 to 720 read in full, the rest searched by keyword; idea only |
| S112 | T. Sokcevic (CE3TSK), "JTDX CE3TSK fork: Improvements for WW Digi 2026", 3 November 2026. https://ce3tsk.com/IMPROVEMENTS_WW_DIGI_2026.pdf ; document list at https://ce3tsk.com/download/index.html | Text extracted locally (2,185 lines): items 1 to 36, 46, 47, 54, 71 and 75 read in full; the other items by title only; the "project notes" it cites are not published |
| S113 | WSJT-X `lib/ft8/subtractft8.f90` and `lib/ft8/ft8b.f90` (the call to it), GitHub mirror master. https://raw.githubusercontent.com/WSJTX/wsjtx/master/lib/ft8/subtractft8.f90 | Both fetched in full and read through a fetch report; `peakup` not read; research only, nothing copied |
| S114 | WSJT-X `lib/wsprd/wsprd.c` (K9AN, K1JT), GitHub mirror master. https://raw.githubusercontent.com/WSJTX/wsjtx/master/lib/wsprd/wsprd.c | Fetched in full (58,332 characters) and read through a fetch report; `fano`, `osdwspr_`, `unpk_` not read; research only, nothing copied |
| S115 | R. Morris (AB1HL), ft8mon, `ft8.cc` and `README.md`. https://raw.githubusercontent.com/rtmrtmrtmrtm/ft8mon/master/ft8.cc | Fetched in full (84,766 characters) and read through a fetch report; the LDPC, last-try and Python parts are in other files, not read; no licence stated, idea only |
| S124 | A. Abrardo, "Performance bounds and codes design criteria for channel decoding with a-priori information", arXiv 0711.1986. https://ar5iv.labs.arxiv.org/html/0711.1986 | Read through a fetch report: first 39,948 of 76,344 characters in full (model, decoder formula, the no-code case); the convolutional, turbo and long-code results as a summary only |
| S132 | WSJT-X `lib/ft8_decode.f90`, `lib/ft8/ft8b.f90`, `lib/ft8/sync8.f90`, GitHub mirror master (https://raw.githubusercontent.com/WSJTX/wsjtx/master/lib/ft8_decode.f90 and the same folder); ft8_lib `demo/decode_ft8.c` [S2]; JS8Call-improved tag `v3.0.3` `JS8_Mode/JS8.cpp` and `JS8_Mode/soft_combiner.h` (https://raw.githubusercontent.com/JS8Call-improved/JS8Call-improved/v3.0.3/JS8_Mode/soft_combiner.h); CE3TSK [S112] items 34 and 46 | Read through fetch reports (CE3TSK from a local text copy); how each treats a decode at the place of one already decoded; research only, nothing copied |
