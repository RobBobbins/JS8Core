# Lab test results (js8core-plus)

js8core-plus's decoder (commit `6012b1e`, Normal mode) measured against the
original JS8Core decoder (commit `cba17f3`) and the decoder of
JS8Call-improved v3.0.3, the current desktop release (published 2026-07-13),
run as a test program on the same audio. All on real-style JS8 traffic.
Measured 2026-11-21; js8core-plus re-run 2026-11-22 after the whole-frame read fix
and the bolder spread reading, and 2026-11-23 after the subtraction window
and the last-try position rule.

The tables below test JS8Call-improved v3.0.3 exactly as released.

**Test audio.** 700 messages, 100 of each kind: heartbeat, CQ, contact reply
(SNR, ACK, GRID? and similar), directed short text, relay, ragchew and weather
report, from 40 stations in 7 world regions, including portable and
operating-abroad callsigns. JS8's own message builder turns each message into
the frames a station sends; a message of several frames goes out in
consecutive cycles. The same list, in the same order, is sent at all four
speeds and at 8 signal levels (-6 to -27 dB in 3 dB steps), with no fading,
with 1 Hz fading and on a crowded band, each plain, with frequency drift (up
to 0.25 Hz/s), with station clocks off (up to 1 s) and with both: 48 test
sets, 718,880 frames, 268,800 messages. None of this audio was used to choose
any decoder setting.

**Whole messages received: every frame of the message decoded** (± is the 95% range)

| Speed | Messages sent | Original JS8Core | JS8Call-improved v3.0.3 | js8core-plus | js8core-plus vs original JS8Core | js8core-plus vs v3.0.3 |
|---|---:|---:|---:|---:|---:|---:|
| Normal | 67,200 | 51.3% ±0.4 (34,450) | 52.4% ±0.4 (35,240) | 58.9% ±0.4 (39,569) | +14.9% | +12.3% |
| Fast | 67,200 | 37.4% ±0.4 (25,163) | 38.0% ±0.4 (25,561) | 49.9% ±0.4 (33,556) | +33.4% | +31.3% |
| Turbo | 67,200 | 21.4% ±0.3 (14,403) | 25.1% ±0.3 (16,872) | 35.6% ±0.4 (23,897) | +65.9% | +41.6% |
| Slow | 67,200 | 51.7% ±0.4 (34,728) | 51.9% ±0.4 (34,863) | 61.5% ±0.4 (41,358) | +19.1% | +18.6% |
| **All speeds** | **268,800** | **40.5% ±0.2 (108,744)** | **41.9% ±0.2 (112,536)** | **51.5% ±0.2 (138,380)** | **+27.3%** | **+23.0%** |

Frames: 44.5% (320,213 of 718,880) original JS8Core, 45.8% (328,902) v3.0.3, 54.8%
(393,902) js8core-plus: +23.0% against the original JS8Core, +19.8% against v3.0.3.

These rates cover all 8 levels. At the strong levels all three decoders
receive nearly everything and at the weakest all receive nothing; the
difference is in the weak signals near each speed's limit:

**Frames decoded at the level where each speed decodes about half**

| Speed | Level | Frames sent | Original JS8Core | JS8Call-improved v3.0.3 | js8core-plus | js8core-plus vs original JS8Core | js8core-plus vs v3.0.3 |
|---|---:|---:|---:|---:|---:|---:|---:|
| Normal | -18 dB | 22,408 | 51.0% ±0.7 | 52.6% ±0.7 | 81.0% ±0.5 | +58.8% | +53.8% |
| Fast | -15 dB | 22,492 | 51.9% ±0.7 | 54.6% ±0.7 | 90.3% ±0.4 | +74.0% | +65.4% |
| Turbo | -12 dB | 22,716 | 42.7% ±0.6 | 52.8% ±0.6 | 86.0% ±0.5 | +101.3% | +62.8% |
| Slow | -21 dB | 22,244 | 42.0% ±0.6 | 41.5% ±0.6 | 60.0% ±0.6 | +43.0% | +44.7% |

**Wrong frames: decoded frames that were never sent**

| Decoder | Wrong / decoded | Wrong rate | 95% range |
|---|---:|---:|---:|
| Original JS8Core | 0 / 320,213 | 0% | 0% to 0.0012% |
| JS8Call-improved v3.0.3 | 0 / 328,902 | 0% | 0% to 0.0011% |
| js8core-plus | 12 / 393,914 | 0.0030% | 0.0016% to 0.0053% |

Results by audio kind and by message kind, and how the audio was made:
[decoder-research-2026-11-18.md](decoder-research-2026-11-18.md),
section 12.

Results page: [decoder-vs-original.html](decoder-vs-original.html)
(GitHub shows it as source; download it and open it in a browser). Method,
sources and every test behind these numbers:
[decoder-research-2026-11-18.md](decoder-research-2026-11-18.md).
