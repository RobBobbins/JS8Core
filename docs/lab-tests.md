# Lab test results (js8core-plus)

js8core-plus's decoder (commit `149b046`, Normal mode) measured against the
js8core (cba17f3) decoder and the decoder of
JS8Call-improved v3.0.3, the current desktop release (published 2026-07-13),
run as a test program on the same audio. All on real-style JS8 traffic.
Measured 2026-11-21; js8core-plus re-run 2026-11-22 after the whole-frame read fix
and the bolder spread reading, 2026-11-23 after the subtraction window
and the last-try position rule, and 2026-11-24 and 2026-11-25 after the second
start time, the leftover pass and the unreadable-frame check.

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

| Speed | Messages sent | js8core (cba17f3) | JS8Call-improved v3.0.3 | js8core-plus (149b046) | js8core-plus vs js8core (cba17f3) | js8core-plus vs JS8Call-improved v3.0.3 |
|---|---:|---:|---:|---:|---:|---:|
| Normal | 67,200 | 51.3% ±0.4 (34,450) | 52.4% ±0.4 (35,240) | 59.2% ±0.4 (39,773) | +15.5% | +12.9% |
| Fast | 67,200 | 37.4% ±0.4 (25,163) | 38.0% ±0.4 (25,561) | 51.2% ±0.4 (34,439) | +36.9% | +34.7% |
| Turbo | 67,200 | 21.4% ±0.3 (14,403) | 25.1% ±0.3 (16,872) | 37.1% ±0.4 (24,931) | +73.1% | +47.8% |
| Slow | 67,200 | 51.7% ±0.4 (34,728) | 51.9% ±0.4 (34,863) | 61.7% ±0.4 (41,481) | +19.4% | +19.0% |
| **All speeds** | **268,800** | **40.5% ±0.2 (108,744)** | **41.9% ±0.2 (112,536)** | **52.3% ±0.2 (140,624)** | **+29.3%** | **+25.0%** |

Frames: 44.5% (320,213 of 718,880) js8core (cba17f3), 45.8% (328,902) JS8Call-improved v3.0.3, 55.5%
(398,854) js8core-plus: +24.6% against js8core (cba17f3), +21.3% against JS8Call-improved v3.0.3.

These rates cover all 8 levels. At the strong levels all three decoders
receive nearly everything and at the weakest all receive nothing; the
difference is in the weak signals near each speed's limit:

**Frames decoded at the level where each speed decodes about half**

| Speed | Level | Frames sent | js8core (cba17f3) | JS8Call-improved v3.0.3 | js8core-plus (149b046) | js8core-plus vs js8core (cba17f3) | js8core-plus vs JS8Call-improved v3.0.3 |
|---|---:|---:|---:|---:|---:|---:|---:|
| Normal | -18 dB | 22,408 | 51.0% ±0.7 | 52.6% ±0.7 | 81.7% ±0.5 | +60.1% | +55.1% |
| Fast | -15 dB | 22,492 | 51.9% ±0.7 | 54.6% ±0.7 | 91.9% ±0.4 | +77.0% | +68.2% |
| Turbo | -12 dB | 22,716 | 42.7% ±0.6 | 52.8% ±0.6 | 88.4% ±0.4 | +106.9% | +67.3% |
| Slow | -21 dB | 22,244 | 42.0% ±0.6 | 41.5% ±0.6 | 60.3% ±0.6 | +43.7% | +45.4% |

**Wrong frames: decoded frames that were never sent**

| Decoder | Wrong / decoded | Wrong rate | 95% range |
|---|---:|---:|---:|
| js8core (cba17f3) | 0 / 320,213 | 0% | 0% to 0.0012% |
| JS8Call-improved v3.0.3 | 0 / 328,902 | 0% | 0% to 0.0011% |
| js8core-plus (149b046) | 9 / 398,863 | 0.0023% | 0.0010% to 0.0043% |

**Costs**

JS8Call-improved v3.0.3 uses very little of the processor to decode.
js8core-plus uses more, but it is still a small share.

Measured with js8core-plus `149b046`, the build the live test has run since
2026-11-25 15:14 UTC, on one core of an AMD Ryzen 9 5900X. **CPU use** is
the share of one core used during each transmit period. **Decode time** is
how long one period takes to decode.

| Band                 | Speed         | JS8Call-improved v3.0.3 CPU use | js8core-plus CPU use | JS8Call-improved v3.0.3 decode time | js8core-plus decode time |
|----------------------|---------------|--------------:|-------------:|------------------:|-----------------:|
| Crowded, 60 stations | Normal (15 s) | 1.9%          | 25.1%        | 0.29 s            | 3.76 s           |
| Crowded, 37 stations | Fast (10 s)   | 2.5%          | 18.3%        | 0.25 s            | 1.83 s           |
| Crowded, 19 stations | Turbo (6 s)   | 2.0%          | 14.6%        | 0.12 s            | 0.88 s           |
| Crowded, 60 stations | Slow (30 s)   | 2.7%          | 12.1%        | 0.80 s            | 3.62 s           |
| 4 stations           | Normal (15 s) | 0.35%         | 2.1%         | 0.05 s            | 0.32 s           |
| Noise only           | Normal (15 s) | 0.13%         | 1.3%         | 0.02 s            | 0.20 s           |

Even the busiest case, a crowded Normal band, uses a quarter of one
core. On this 12-core processor, that is about 1% of the whole
processor. On that band, js8core-plus decoded 416 frames where
JS8Call-improved v3.0.3 decoded 140.

Most of the extra work is js8core-plus trying the operator's own callsign
and the stations heard in the last 10 minutes. These figures are with a
full list of 17 callsigns.

Not measured yet: Deep Decoding with callsigns, Raspberry Pi and older
PCs.

Results by audio kind and by message kind, and how the audio was made:
[decoder-research-2026-11-18.md](decoder-research-2026-11-18.md),
section 12.

Results page: [decoder-vs-original.html](decoder-vs-original.html)
(GitHub shows it as source; download it and open it in a browser). Method,
sources and every test behind these numbers:
[decoder-research-2026-11-18.md](decoder-research-2026-11-18.md).
