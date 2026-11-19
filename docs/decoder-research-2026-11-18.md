# Decoder research, 2026-11-18

Where the js8core-new decoder stands against the best published decoder, and what to work on next. Research only on 2026-11-18. On 2026-11-19 the fading test was built and run; its results are in section 5. Every finding names its source; sources are listed at the end as [S1] to [S39].

## 1. Summary

- Our decoder, without callsign hints, decodes half of the weak test signals at about **-20.2 dB**. [S3]
- The best published decoder for the same signal structure (WSJT-X, FT8) reaches **-20.8 dB**, so we are about **0.6 dB** behind. [S1, Table 6]
- The paper uses three methods we do not: reading signals 2 tones at a time, a deeper last-try decode, and a tuned scaling setting. [S1, section 6] [S6]
- The paper shows real HF fading costs about 2 dB and changes which methods help most. [S1, Tables 5 and 6]
- **Measured 2026-11-19 on standard fading (mid-latitude disturbed):** ours decodes half at **-17.7 dB**, the PC decoder at **-16.8 dB**, the paper's FT8 at **-18.6 dB**. On fading we are 0.9 dB ahead of the PC decoder and 0.9 dB behind FT8. [S9]
- On fading, one wrong message got through, from the last-try decode. Details in section 5. [S9]
- **Deeper last try, done 2026-11-19:** small gain (+0.1 dB fade-free, +0.06 dB fading), no new wrong messages, no slower. The acceptance limits, not the search depth, now hold back most of what it finds; loosening them raises wrong messages too much. [S10]
- **Reading 2 tones at a time and a Deep option, done 2026-11-19:** Normal decodes a little more and has fewer wrong messages; Deep decodes the most (+0.1 dB on fading), at 1.5 times the wrong-message chance and up to 1.5 times the time on a crowded band. Section 4. [S13]
- **Scaling setting, measured and changed 2026-11-19, Deep only:** raising the scale from 2.83 to 6.0 gains a few decodes (Normal mode: fading +5 of 800, fade-free +2 of 600, crowded +7 of 1,800; Deep: fading +4, crowded +3), loses none, and adds no wrong messages. The cost: about 1.4 to 1.5 times the time on a crowded band, because the standard decode's early-stop rule fires half as often. So Deep uses 6.0 and Normal keeps 2.83 at today's speed. Section 4. [S19]
- **Wrong messages, measured 2026-11-19; Forney's test added to Deep:** each last-try result that reaches the CRC check without being the sent message is a 1 in 4,096 chance of a wrong message, and the wrong messages seen match that rate (the earlier "9 sync tones" pattern was chance). Both Normal and Deep now require the best last-try answer to beat the runner-up by 0.2% of the evidence: about 41% fewer chances for 1.6% fewer messages in each mode. Section 4. [S30]
- **Fast and Turbo speeds, measured 2026-11-19:** against the original JS8Core decoder, ours gains about 0.5 dB fade-free and 1.3 dB on fading at both speeds, and more than doubles the messages on a crowded band. Deep adds little at these speeds. The last try lets through wrong messages at the same 1 in 4,096 per attempt as at Normal speed. Normal took 2.03 times the original's time on the crowded Fast set; a faster last-try scoring (identical results) brought that to 1.55 times, and the Normal-speed crowded set from 2.64 to 1.95 times. Section 5. [S31] [S32]
- **Last try fed the standard decode's output (Zhang et al. 2023), built 2026-11-19:** 3 short decode rounds order the last-try search; the order-4 search is skipped then. Normal +67 messages (+2.3%), Deep +63, Fast and Turbo +24 to +40; same speed; wrong-message chances +7% Normal, +5% Deep at today's runner-up gap. **The runner-up gap was then raised to 0.0025 (Rob's decision):** against the decoder before both changes, every speed and mode now decodes more (+15 to +59 messages) with 9% to 22% fewer wrong-message chances. Section 4. [S33] [S34] [S36]
- **Normal's 10-sync-tone rule for the 3-tone last try, re-measured 2026-11-19: kept.** No source backs it, but 9 sync tones adds 7% to 10% more wrong-message chances for 0.15% to 0.4% more messages, a worse trade than Deep's. Section 4. [S37] [S38] [S39]

## 2. Where we stand

Decodes without callsign hints (decoder `new`), 32 signals per level per set. Counted from saved results with their `truth.tsv` files. [S3]

| Level (dB) | synth | dsynth | dsynth2 | dsynth3 | Total | Rate |
|---|---|---|---|---|---|---|
| -19 | 30 | 27 | 30 | 30 | 117 / 128 | 91% |
| -20 | 16 | 16 | 20 | 22 | 74 / 128 | 58% |
| -21 | 7 | 7 | 5 | 7 | 26 / 128 | 20% |
| -22 | 0 | 0 | 0 | 1 | 1 / 128 | 1% |

Half the signals decode between -20 and -21. A straight line between those two points crosses 50% at about -20.2 dB.

Test audio signal strength uses the usual 2500 Hz reference bandwidth (`scorecard\synth_gen.cpp`, lines 94 to 96), the same definition the paper uses. [S1, page 11] [S3]

## 3. The published benchmark

The paper gives decoding thresholds as the signal strength in 2500 Hz at which half the signals decode. [S1, page 11]

FT8, from [S1] Table 5 (page 11) and Table 6 (page 14):

| WSJT-X decoder setup | Fade-free (AWGN) | Mid-latitude disturbed fading |
|---|---|---|
| Reading 1 tone at a time, standard decode | -19.6 | -16.5 |
| Reading 1, 2 and 3 tones at a time, standard decode | -20.3 | -17.0 |
| Reading 1, 2 and 3 tones at a time, standard plus deep last-try decode | -20.8 | -18.6 |
| Same, with the most guessing (end of a contact) | -22.7 | -22.1 |

FT8 and JS8 Normal speed share the structure that matters here: 79 tones of 0.16 s, 8 tone frequencies, 3 sync patterns of 7 tones, 174-bit error-correction codeword. [S1, sections 3 and 4] [S6]

## 4. Three methods the paper uses that we do not

1. **Reading 2 tones at a time.** Built 2026-11-19; see "Reading 2 tones at a time, and the Deep option" below.
   - The paper reads blocks of 1, 2 and 3 tones (FT8: "N = 1, 2, and 3") and tries the decode on each. [S1, page 11]
   - Ours read 1 and 3 until 2026-11-19 (`kBlockSymbols = 3` in `src\decoder\legacy_decoder.cpp` at commit b85d0af). [S6]
   - The paper's gain for adding 2 and 3 together over 1 alone is 0.7 dB fade-free. It does not give the gain of 2 on its own. [S1, Table 5]

2. **A deeper last-try decode.**
   - The paper's last try ("ordered statistics decoding") uses shortcuts from Wu and Hadjicostis [S5] to keep it fast. [S1, page 11]
   - Ours searches at depth 2: `kOsdOrder = 2`, line 53. [S6]
   - The paper does not state its depth.
- The Wu and Hadjicostis paper is now read in full, from a copy Rob placed in the project; it is now in `docs/`. [S5] Findings below.
- A 2024 paper on the same family of methods for short codes (128 bits, rate 1/2) reports its best versions "within 0.3dB of the ML performance", meaning the best possible decoding. It gives no figures for our code size (174 bits) or for depth 2 against depth 3, so the gain for us is still unknown. [S8]

### Wu and Hadjicostis: the deeper last try (read in full) [S5]

- **How the last try works:** it trusts the most reliable bits, re-builds the codeword from them, then tries flipping up to w of those bits ("order-w") and keeps the closest codeword. [S5, pages 2 to 3]
- **Ours is the plain version:** order 2, one set of trusted bits, every candidate fully checked (`src\decoder\ldpc.cpp`, lines 251 to 346). That is 1 + 87 + 3,741 = 3,829 candidates per try. A plain order 3 would add 105,995 more, and a plain order 4 would add 2,225,895 more. [S6]
- **Method 1, two filters ("preprocessing"):** cheap bit checks throw out hopeless candidates before the full check. [S5, section III]
  - Rule 1 drops a candidate of up to w + 1 flips if it disagrees with more than θ of the trusted bits plus the t most reliable remaining bits.
  - Rule 2 drops a candidate of w + 2 flips if it disagrees with more than 1 of the τ most reliable remaining bits.
  - Claim: order-w with the filters performs like plain order-(w + 2), with fewer full checks than plain order-w (Theorem 2, page 7). This is proved for strong signals; the paper's simulations show the gain at practical signal levels too.
  - Settings formula: t = R/2, θ = 2(w + 3), τ = 3 log2(K/w), where K = message bits and R = check bits (Theorem 2, page 7). For our code (K = 87, R = 87) at w = 2: t = 43, θ = 10, τ ≈ 16 (worked out here, not in the paper).
  - The paper says the method suits codes where at least half the bits are message bits (page 7). Ours is exactly half: 87 of 174.
- **Method 2, a second set of trusted bits ("two-basis"):** swap the κ least reliable trusted bits for the κ most reliable remaining bits, then search again. [S5, section IV]
  - In its simulations, two-basis order 3 beat plain order 5, and two-basis order 4 matched the best possible decoding. [S5, Figure 3, page 10]
- **Depth needed:** plain order-w approaches the best possible decoding when w ≈ (code's minimum distance) ÷ 4. [S5, page 5] I have not measured our code's minimum distance.
- **Not known before testing:** the gain on our signals, and the false-decode risk. Both measured below.

### Wu and Hadjicostis method 1, built and measured (2026-11-19) [S10]

**What was built.**
- `ordered_statistics_decode` in `src\decoder\ldpc.cpp` has a new `filtered` switch. With it on, after the plain order-2 search it also tries 3 and 4 flips:
  - 3 flips (105,995 candidates) go through rule 1 with t = 43, θ = 10. A candidate is fully checked only if it disagrees with at most 7 of the 43 most reliable non-pivot bits.
  - 4 flips go through rule 2 with τ = 16. Pairs of flips are matched through a 65,536-entry table on the 16 most reliable non-pivot bits, so only 4-flip candidates that disagree with at most 1 of those bits are built at all.
- Passes 7 and 8 (the last try) use it. The callsign-hint path is unchanged.

**Check on test codewords** (`scorecard\.osdcheck.cpp`, 300 noisy copies of valid codewords per level):

| Signal strength (mean LLR) | Plain order 2 found the sent codeword | Filtered order 2 + 3 + 4 | Time per call, plain | Time per call, filtered |
|---|---|---|---|---|
| 1.4 | 299 | 300 | 1.26 ms | 3.09 ms |
| 1.2 | 248 | 284 | 1.25 ms | 2.74 ms |
| 1.0 | 91 | 124 | 1.29 ms | 2.54 ms |
| 0.9 | 26 | 37 | 1.30 ms | 2.28 ms |

- The filtered search never ended with a worse discrepancy than the plain one (0 of 1,200).

**Decoder on test audio** (seed 11 sets from section 5, deciding levels only; old = commit b85d0af, saved results):

| Set and level | Old | New | Wrong, new |
|---|---|---|---|
| Fading -17 | 134 | 137 | 0 |
| Fading -18 | 84 | 87 | 0 |
| Fading -19 | 47 | 50 | 1 (the same one as before) |
| Fading -20 | 16 | 18 | 0 |
| Fade-free -19 | 187 | 189 | 0 |
| Fade-free -20 | 120 | 129 | 0 |
| Fade-free -21 | 30 | 34 | 0 |
| Noise only, 400 files (`scorecard\noise4`) | — | — | 0 |

- Half-decode point: fade-free -20.22 to -20.31 dB (+0.09); fading -17.68 to -17.74 dB (+0.06). Gains of 11 of 800 on fading and 15 of 600 fade-free at these levels.
- Time per 15 s file unchanged: 0.112 s old, 0.110 s new on fading; 0.116 s old, 0.113 s new fade-free. The last try is a small part of total decode time.
- All 19 project tests pass.

**Why the gain is small: the acceptance limits, measured.**
- On fading levels -17 to -20, the deeper search found 30 true messages the old decoder missed (CRC passed). Of those, 6 were rejected by the discrepancy limit (0.065 to 0.08) and 8 by the sync minimum (6 to 8 tones, needs 9).
- On the 400 noise files, last-try results that reach the CRC check, by band:

  | | Ratio ≤ 0.065 | 0.065 to 0.08 | Above 0.08 |
  |---|---|---|---|
  | Sync ≥ 9 (current) | 69 | 63 | 30 |
  | Sync 6 to 8 | 685 | 650 | 174 |

- The CRC is 12 bits, so a random wrong result passes about 1 time in 4,096. With today's limits that is 69 ÷ 4,096 ≈ 0.017 expected wrong messages per 400 files: about 1 per 24,000 cycles of pure noise (about 99 hours).
- Widening the ratio limit to 0.08 would double that (132 attempts) for 6 more of 800 fading messages. Allowing sync 6 to 8 would make it about 11 times as high. **Decision: limits unchanged.**

### Reading 2 tones at a time, and the Deep option (built and measured 2026-11-19) [S1] [S11] [S12] [S13]

**What the paper says, reread from a saved copy (article pages 11 and 12)** [S1]:
- Soft decisions are made for each block length and "submitted to the decoder, starting with the set for N = 1". Each block length is tried with the standard decode alone or with the last try as well.
- Reading several tones together "improves sensitivity ... when the received signal maintains phase coherence over multiple symbols". Single-tone reading holds up best on fast fading, and longer blocks help in slow fading. On the fastest fading channel the paper tested, it gave no significant gain, because the signal changes within one block.
- The bit metric is the same as ours: the best correlation magnitude with the bit set minus the best with it clear, times an empirical constant K.
- The paper's last try uses the Wu and Hadjicostis preprocessing rules [S5], the same method as our deeper search above.
- Not stated: whether blocks are fixed or sliding. Ours are fixed.
- The method's source [S12] is behind the IEEE paywall and was not read. Its abstract says it covers fade-free signals only.
- From the ITU model's formula (my calculation, not a measurement): at 1 Hz frequency spread, the fading gain is 0.88 correlated across a 2-tone block (0.16 s between the first and last tone centres) and 0.60 across a 3-tone block (0.32 s). So 2-tone blocks should hold up better on fading than 3-tone blocks.

**The Deep option** [S11]:
- WSJT-X offers Fast, Normal and Deep decoding. Its guide says Deep usually gives the best sensitivity, at the cost of slower processing and a slightly higher chance of false decodes.
- JS8Core now has `Js8Engine::set_deep_decoding(bool)` (`include\js8core\engine.hpp`), off by default. It sets `DecodeParams::deep` (`include\js8core\decoder_state.hpp`). The test tool takes `--deep`.
- Rule: a decoder step that measurably raises the chance of a wrong message runs only with Deep. Easy JS8 needs its own switch to turn it on; that code is not ours to open.

**What was built** (`src\decoder\legacy_decoder.cpp`):
- Pass 6: standard decode on 2-tone readings. Runs always.
- Pass 9: last try on 2-tone readings. Deep only.
- The 3-tone last try (pass 10) needs sync 10 or more without Deep (`kBlockOsdMinSyncMatches`); with Deep, 9 as before. Reason below.

**Noise only, 400 files (`scorecard\noise4`): results reaching the CRC check, by pass**

| Pass | Results reaching the CRC check | CRC passed |
|---|---|---|
| 1 to 7, standard decode (1-, 2- and 3-tone) | 0 | 0 |
| 8, last try, 1-tone | 36 | 0 |
| 9, last try, 2-tone (Deep only) | 32 | 0 |
| 10, last try, 3-tone | 30 | 0 |

- Normal: 66 results reach the CRC check, about 0.016 expected wrong messages per 400 files. Deep: 98, about 0.024 (1.5 times). No wrong message in either run.
- Correction, 2026-11-19 [S19]: the pass 10 row above (30) was counted at 9 sync tones. With the final Normal rule (10 sync tones) pass 10 brings 8 results to the CRC check, so Normal is 36 + 8 = **44** (about 0.011 expected wrong messages per 400 files). Pass 8 recounted at exactly 36.

**Which pass found the messages, whole fading set with Deep (400 files):** pass 6: 4; pass 7: 3; pass 8: 207; pass 9: 24; pass 10: 7.
- On fading, the 2-tone last try found about 3 times as many as the 3-tone last try, as the paper and the correlation figures above predict.
- The 2-tone standard pass found only 4, and the 1-tone last try (pass 8) finds those anyway. So 2-tone reading pays off on fading only in the last try, which is the Deep part.

**The two wrong messages seen so far share one pattern.**
- Fading `ffade\S-19_21.wav` (1469 Hz) and crowded `crowd60\S-19_7.wav` (772.4 Hz, a real signal at 773.9 Hz): both from the 3-tone last try, at exactly 9 sync tones, on top of a real weak signal.
- Across the fading, fade-free and crowded sets, the 3-tone last try at exactly 9 gave 2 right and 2 wrong messages. The 1-tone last try at 9 gave 21 right and 0 wrong.
- Hence sync 10 for the 3-tone last try without Deep. It is four events only, so the evidence is thin; the cost is small either way.

**Results** (deciding levels; "before" is commit 02d757d for fading and fade-free, and b85d0af for the crowded set, where 02d757d was not run):

| Set | Before | Normal | Deep |
|---|---|---|---|
| Fading, -17 to -20 dB, 800 messages | 292, 1 wrong | 291, 0 wrong | 305, 1 wrong |
| Fade-free, -19 to -21 dB, 600 messages | 352 | 354 | 358 |
| Crowded: 60 overlapping signals per file, -19 to -21 dB, 1,800 messages (`scorecard\crowd60`) | 283, 1 wrong | 321, 0 wrong | 344, 1 wrong |
| Noise only, 400 files | 0 wrong | 0 wrong | 0 wrong |

- Half-decode point: fading Normal -17.74 dB (unchanged), Deep -17.84 dB; fade-free Normal -20.32 dB, Deep -20.33 dB.
- Time per file: fading 0.110 s before, 0.114 s Normal, 0.130 s Deep. Crowded 0.52 s before (b85d0af), 0.61 s Normal, 0.76 s Deep; slowest crowded file 0.80 s before, 0.91 s Normal, 1.11 s Deep. All inside the limit of twice as slow.
- All 19 project tests pass.


3. **A tuned scaling setting.**
   - The paper scales its readings by a constant K, "adjusted empirically". [S1, page 11]
   - Ours scaled to a fixed spread of 2.83 and had never been tuned. [S6] Since 2026-11-19 Deep uses 6.0 (section 4).
   - ft8_lib uses a different fixed value, also found by experiment. [S2]

### The scaling setting, researched and measured (2026-11-19) [S1] [S2] [S4] [S14] [S15] [S16] [S17] [S18] [S19]

**What the scale does in our decoder**
- `normalize` in `src\decoder\legacy_decoder.cpp` divides each set of 174 readings by its own spread and multiplies by 2.83. It is used for the 1-tone amplitude readings (passes 1, 3, 4, 5), the log readings (pass 2) and the 2- and 3-tone readings (passes 6, 7). Pass 5 multiplies by a further 1.5, so it runs at 4.25.
- Our standard decode is full sum-product (`tanh`/`atanh`, `src\decoder\ldpc.cpp` lines 206 to 233), so the scale changes its results. A min-sum decoder would ignore it.
- The last try (passes 8 to 10) does not depend on the scale: its limit is discrepancy relative to total evidence. Measured: identical noise results at every scale (below).
- The readings are amplitudes (`std::abs`, line 702), despite the variable name `tone_power`.

**Where the 2.83 and other values come from**
- WSJT-X `lib/ft8/ft8b.f90`: same readings as ours (best amplitude with the bit set minus best with it clear), divided by their spread, then `scalefac=2.83`. No comment explains the value. [S15]
- The FT8 paper: K "is adjusted empirically to optimize performance over a range of SNRs and channel conditions". [S1, page 11]
- ft8_lib scales to variance 24 (spread 4.90), with the comment "experimentally found coefficient". Its readings are in decibels (`10*log10` of power, stored in 0.5 dB steps), the same kind as our pass 2 log readings once divided by their spread. [S2: `ft8/decode.c` `ftx_normalize_logl`, `ft8/decode.h` `WF_ELEM_MAG`, `common/monitor.c`]
- A Python re-implementation of WSJT-X calls 2.83 a "fudge factor" with the comment "Square root of 8 ???". That is a guess, not a source.

**What the research says about the right scale**
- The correct reading for this kind of signal is log I0(2a√Es·|y|/N0): the multiplier grows with signal-to-noise, so a fixed scale is a compromise. [S14, MILCOM 2005 equation 3; 2007 equation 1]
- Valenti and Cheng assume the signal and noise levels are known and leave estimating them to later work. [S4, page 1746]
- Their later work estimates both per block. With known levels as the benchmark it lost 0.3 dB with 4 symbols per block and 0.03 dB with 32, on fade-free signals. [S14, 2007 page 3146]
- On turbo codes, too big is safer than too small: "overestimating B is better than underestimating it", quoting a result that signal-to-noise can be overestimated by up to 3 dB with little harm. [S14, 2007 page 3145] Summers and Wilson (1998) are reported to say the same; only the abstract and citing papers were reachable. [S17]
- On LDPC sum-product decoding (our kind), one modem project warns that over-confident readings converge "to the wrong codeword"; that is a stated claim, not a measurement. The same project measured a per-frame scale estimate about 1 dB worse than a fixed scale in fading, because the estimate read low. [S16]
- The main LDPC paper on this (Saeedi and Banihashemi, 2007) could not be reached. [S18]

**Test** [S19]: one build with the scale settable at run time (a copy, never the repo file), checked to decode file-for-file the same as the committed decoder at 2.83. Every value ran on the same files, Normal mode. Values from the sources: 2.83 (WSJT-X), 4.25 (our pass 5), 4.9 (ft8_lib); then 2.0, 6.0, 8.0, 10.0 and 12.0 to find the direction and the peak.

| Scale | Fading, -17 to -20 dB, 800 | Fade-free, -19 to -21 dB, 600 | Crowded, 60 signals per file, -19 to -21 dB, 1,800 |
|---|---|---|---|
| 2.0 | 289 | 352 | not run |
| **2.83 (now)** | **291** | **354** | **321** |
| 4.25 | 293 | 356 | 327 |
| 4.9 | 293 | 356 | 328 |
| 6.0 | 296 | 356 | 328 |
| 8.0 | 296 | 356 | not run |
| 10.0 | 295 | 356 | not run |
| 12.0 | 295 | not run | not run |

- No wrong messages at any value on any set.
- Same files, signal by signal, against 2.83: 6.0 gained 5 / 2 / 7 and lost 0 / 0 / 0; 4.25 gained 2 / 2 / 6 and lost 0 / 0 / 0.
- Pass 2 (log readings) kept at 2.83 with the rest at 6.0: identical totals (296 fading, 356 fade-free). One shared value is enough.
- Which pass decoded, fading: at 2.83 the standard decode (pass 1) found 88 and the last try (pass 8) 144; at 6.0, 141 and 121. Fade-free: pass 8 falls from 63 to 57. The bigger scale moves decodes from the last try to the standard decode.
- Noise only, 400 files, results reaching the CRC check: passes 1 to 7 brought **0** at 2.83, 4.9, 6.0 and 8.0; passes 8 and 10 brought 36 and 8 at every scale. No wrong messages. The warning about over-confident readings did not show up as noise reaching the CRC check.

**Time, crowded set, one file at a time:**

| Scale | Mean per file | Slowest file |
|---|---|---|
| 2.83 (now) | 0.65 s | 0.98 s |
| 4.25 | 0.84 s | 1.34 s |
| 4.9 | 0.93 s | 1.29 s |
| 6.0 | 0.97 s | 1.41 s |

- Why it is slower, counted over the 30 crowded files: the standard decode made about the same number of attempts (16,350 at 2.83, 16,402 at 6.0), but its early-stop rule (`ldpc.cpp` line 180: no progress for 5 rounds, at least 10 rounds, more than 15 failed checks) stopped hopeless attempts 13,282 times at 2.83 and only 6,380 times at 6.0. Attempts running all 60 rounds rose from 2,872 to 9,796, so total rounds rose from 423,143 to 879,335 (2.1 times).
- The early-stop rule was set with the 2.83 scale in place. Bigger readings keep failing attempts looking busy for longer.
- Fading and fade-free files showed no slowdown over the 1.0 s estimate at any value.

The best value is 6.0 (best on all three sets). The side effect is 1.5 times the time on a crowded band. Retuning the early-stop rule for the new scale might remove the slowdown; not tested.

**Changed in the code 2026-11-19: Deep uses 6.0, Normal keeps 2.83.** Extra decoding time belongs in Deep, not Normal. `src\decoder\legacy_decoder.cpp` picks the scale from the Deep setting. All the measurements above were Normal mode, so Deep was tested before the change (same files, Deep on, 2.83 against 6.0) [S19]:

| Deep on | Fading, 800 | Fade-free, 600 | Crowded, 1,800 | Noise, 400 files | Crowded, mean time per file |
|---|---|---|---|---|---|
| 2.83 | 305, 1 wrong | 358 | 344, 1 wrong | 0 wrong | 0.77 s |
| **6.0** | **309, 1 wrong** | **358** | **347, 1 wrong** | **0 wrong** | **1.10 s** |

- 6.0 against 2.83, signal by signal: gained 4 / 0 / 3, lost 0 / 0 / 0.
- The two wrong messages are the same at both scales: `ffade\S-19_21.wav` and `crowd60\S-19_7.wav`, both from the 3-tone last try (section 4, "The two wrong messages seen so far share one pattern"). The scale did not cause them.
- Built decoder checked after the change, on all four sets: Normal decodes exactly as before (291 / 354 / 321, 0 wrong, crowded 0.64 s per file); Deep decodes exactly as the 6.0 test above. 19 of 19 project tests pass. Copy kept as `scorecard\bin\dec-scale6`.

### The give-up rule and the try limit, researched and measured (2026-11-19) [S15] [S2] [S20] [S21] [S22] [S23] [S24]

**Where our rule comes from**
- Our standard decode gives up when the number of failed checks has not dropped for 5 rounds, at round 10 or later, with more than 15 checks failing (`src\decoder\ldpc.cpp` line 180). It is in the first JS8Core commit (587e222) and is the same rule as WSJT-X `bpdecode174_91.f90` and `decode174_91.f90` (2.6.1 and current master). No source explains the numbers 5, 10 and 15. [S15]
- Try limits: ours 60 (pass 5: 150, no give-up rule). WSJT-X FT8: `maxiterations=30`. [S15] ft8_lib: 25 (`kLDPC_iterations`), and no give-up rule at all, only "stop when all checks pass". [S2] WSJT-X in 2017 used "40 iterations for bp", aiming to keep decoding "well under 1 s". [S20]

**What published work says about giving up early**
- On attempts that will fail, the failed-check count "fluctuates in a range"; on attempts that will succeed it falls steadily, starting by round 4 to 6 on the code tested (a 2048-bit code, not ours). [S21]
- Our rule restarts its 5-round count on any drop, even a drop of one, so a bouncing count keeps restarting it. This fits the measurement above: at scale 6.0 it fired 6,380 times instead of 13,282.
- Published rules: compare the failed-check count against fixed limits at three set rounds (2.4 times fewer rounds, 0.04 dB loss, limits found by trial) [S21]; Shin et al. 2007, passed-check count with three limits said not to depend on signal strength; Cui et al. 2007, failed-check sum with two limits that do; Kienle and Wehn 2005, stop when total bit confidence stops growing for 2 rounds (fewest rounds on weak signals, most false alarms, and dependent on scale); Glikiotis and Paliouras 2005, detect the decoder repeating itself. These four are known here only from how [S22] describes them; the originals could not be reached.
- Every rule trades false alarms (giving up on an attempt that would decode: for us, a lost message) against misses (grinding on junk: for us, time). Rules must be tuned with both counted. [S22]
- Damping (blending each round's messages with the previous round's) is used "to mitigate oscillations", and message sizes tend to grow with each round. [S23]
- Stopping rules that detect early success (mutual information, cross-entropy) target turbo codes and finishing early, not giving up; not applicable here. [S24]

**Try limit measured** (scale 6.0, Normal mode, same files; time per file one at a time on the crowded set) [S19]:

| Try limit | Fading, lost of 296 | Fade-free, lost of 356 | Crowded, lost of 328 | Crowded, mean time per file |
|---|---|---|---|---|
| 60 (now) | 0 | 0 | 0 | 0.93 s |
| 50 | 0 | 0 | 2 | 0.88 s |
| 40 | 1 | 0 | 3 | 0.83 s |
| 30 | 2 | 0 | 4 | 0.78 s |
| Scale 2.83, limit 60 (today's decoder) | — | — | — | 0.62 s |

- No wrong messages at any limit. Noise-only not rerun: no standard-decode attempt reached the CRC check on noise with 60 tries, and a lower limit runs the same rounds and stops sooner, so it cannot add one.
- Successful standard-decode attempts needing more than 30 tries at scale 6.0 (all three sets): 13 of 171 fading, 9 of 285 fade-free, 19 of 226 crowded. Some need over 100; those are pass 5, which has its own limit of 150.
- **Result: lowering the try limit is not the fix.** Every cut loses real messages on the crowded set, and even at 30 it is still 1.26 times today's time. The limit stays at 60.
- Next idea, by the research above: a give-up rule that bouncing cannot dodge (checking at set rounds [S21]), then damping [S23]. Neither tested yet.

The paper's method of removing a decoded signal before the next pass is the same approach as ours: estimate the channel with a low-pass filter, then subtract. Both use up to 3 passes. [S1, page 12] [S6, lines 1087 to 1127 and 1168]

### Wrong messages from the last try: research (2026-11-19) [S25] [S26] [S27] [S28] [S29]

**The question.** With Deep, the fading and crowded sets each give 1 wrong message, both from the 3-tone last try at 9 sync tones, on top of a real weak signal (above). Four events cannot give a rate. What do others use to keep last-try results honest, and how can the rate be measured?

**What WSJT-X does after its last try (FT8)** [S25]:
- At most 36 hard errors (`nharderrors.gt.36` rejected).
- The all-zero codeword rejected (ours does the same).
- A message-format check: the 3-bit message type and subtype must be valid, and the message must unpack (`unpack77`); some odd forms are rejected too.
- A "likely false decode" rule: rejected if sync is 10 or less of 21 **and** the measured signal-to-noise is below -25 dB.
- Minimum sync before decoding at all: more than 6 of 21 at Deep, more than 8 at Normal.
- Its CRC is 14 bits; ours is 12, so a random result passes ours 4 times as often (1 in 4,096 against 1 in 16,384).
- It reports a quality figure, 1 - (hard errors + discrepancy) / 60, to the user (`ft8_decode.f90`).

**What published work says**
- Forney's rule (1968): accept the best answer only if it is clearly more likely than every other answer together; otherwise give no answer. In practice the runner-up stands in for "every other answer". [S26] [S27] [S28]
- Sauter et al. (2025), short codes: a likelihood threshold beats a CRC for catching wrong answers **when the signal model is accurate**. When the receiver's picture of the channel is off (their test: imperfect phase knowledge), the CRC is more robust and threshold tests "suffer a significant performance loss". [S26] Our signals fade and the signal-to-noise is unknown, so this argues for checks that do not rest on likelihood alone.
- Yuan, Médard, Galligan and Duffy (SO-GRAND, 2024): the chance a found answer is wrong grows with how many candidates were tried before it, about (2^k - 1)/(2^n - 1) per candidate for a random code. [S27] For us: every last-try result that reaches the CRC check is one more chance (1 in 4,096) of a wrong message.

**How JS8 messages can be checked.** JS8Core has functions that unpack each frame type (`src\protocol\varicode_frames.cpp`: heartbeat, compound, directed, data, fast data). Some reject malformed text. How many random payloads pass them is not known; it has to be measured. [S29]

**Measuring the rate without waiting for rare events.** As on the noise set (above), count the last-try results that reach the CRC check on the signal sets, split by pass and sync tones. Those that are not the sent message pass the CRC about 1 time in 4,096, so the count gives the expected wrong-message rate long before many wrong messages are seen.

### Wrong messages from the last try: measured, and Forney's test added to Deep (2026-11-19) [S30]

**Test sets.** The three signal sets above, plus fresh ones with new seeds so no limit is judged on the files it was picked from: `scorecard\ffade2` (fading, seed 12, 100 files per level, -17 to -20 dB), `fflat2` (fade-free, seed 12, 100 per level, -19 to -21 dB), `crowd2` (60 signals per file, seed 32, 20 files per level, -19 to -21 dB).

**Where the chances of a wrong message come from** (first three sets, Deep): last-try results reaching the CRC check without being the sent message: fading 1,208, fade-free 388, crowded 1,323, noise 98.
- Most are at 11 or more sync tones, on top of real weak signals. Few are at 9.
- Expected wrong messages: 2,919 ÷ 4,096 ≈ 0.71; seen: 2. Normal on the same files: 1,803 ≈ 0.44; seen: 0. Fresh sets: Deep 5,836 ≈ 1.4, seen 2; Normal 3,564 ≈ 0.9, seen 1.
- So the wrong messages match chance at 1 in 4,096 per attempt. **The "9 sync tones" pattern was chance:** a fresh-set wrong message came at 14 sync tones, in both Normal and Deep.
- Per message found, the 1-tone last try (pass 8) brings about 4 failed results to the CRC check; the 2-tone and 3-tone last tries (passes 9, 10) about 18 and 21.

**Checks measured, and what each would do** (right last-try messages lost / wrong-message chances removed):

| Check | First sets, Deep | Fresh sets, Deep | Verdict |
|---|---|---|---|
| Hard errors at most 36 (WSJT-X) | 4 of 341 / 6% | not run | Too little effect |
| Sync ≤ 9 and SNR below -30 dB (WSJT-X rule, our SNR scale) | 1 of 341 / 9% | 3 of 663 / 10% | Too little effect |
| Sync ≤ 10 and SNR below -30 dB | 3 of 341 / 14% | 12 of 663 / 15% | Too little effect, costs grow |
| Frame-format check with JS8Core's unpackers | — | — | Rejects 13% of random frames; both wrong messages passed it |
| **Best answer must beat the runner-up by 0.2% of the evidence** (Forney) | 18 of 341 / 43% | 45 of 663 / 41% | **Taken for Deep** |

- WSJT-X's -25 dB does not carry over: our decoder reports about 8 dB lower for the same signal (`scorecard\synth_gen.cpp`), and right and failed results overlap on signal strength.
- The runner-up gap is the measure Forney's rule calls for [S26] [S27]. It separates right from failed results far better than sync, signal strength, hard errors or the discrepancy alone. Of the 4 wrong messages seen, 3 had gaps under 0.002 (0.0007, 0.0008, 0.0014); the fourth 0.004.

**Full decoder with the gap test, all six sets** (passes after the last try can still find a rejected message, so this is the real cost):

| | Messages | Wrong seen | Wrong-message chances | Expected wrong |
|---|---|---|---|---|
| Normal (unchanged) | 2,953 | 1 | 5,367 | 1.31 |
| Deep before | 3,111 | 4 | 8,755 | 2.14 |
| Deep, gap 0.001 | 3,089 | 2 | 6,740 | 1.65 |
| **Deep, gap 0.002 (now)** | **3,061** | **1** | **5,067** | **1.24** |
| Deep, gap 0.003 | 3,038 | 1 | 3,747 | 0.91 |
| **Normal, gap 0.002 (now)** | **2,907** | **0** | **3,159** | **0.77** |

- **Decision: Deep uses gap 0.002** (now `kOsdMinRunnerUpGap`, `src\decoder\legacy_decoder.cpp`; runner-up from `ordered_statistics_decode`, `src\decoder\ldpc.cpp`). Deep then has fewer wrong-message chances than Normal and still decodes 108 more messages (3.7%). Cost: 50 messages (1.6%) against Deep before.
- Built decoder checked: Normal decodes file for file as before on all seven sets; Deep gives exactly the figures above; noise 0 wrong; busy band 1.12 s per file (1.15 s before). 19 of 19 project tests pass. Copy kept as `scorecard\bin\dec-forney`.
- **Normal uses the same test since 2026-11-19 (Rob's decision):** 41% fewer chances (5,367 → 3,159) for 46 of 2,953 messages (1.6%); the 1 Normal wrong message seen (`ffade2\S-20_4.wav`) is gone. Built decoder checked: Normal gives exactly the measured figures (282 / 567 / 354 / 699 / 315 / 690), noise 0 wrong, busy band 0.62 s per file (0.64 s before); Deep decodes file for file as before. 19 of 19 project tests pass. Copy kept as `scorecard\bin\dec-forney2`. The constant is now `kOsdMinRunnerUpGap`.
- What a busy band means in time: on the crowded sets (60 weak signals per 15 s cycle), Deep before had about 46 chances per cycle, 1 wrong message per about 90 cycles (22 minutes); now about 28 per cycle, about 1 per 145 cycles (36 minutes). Real bands mix strong and weak signals, so these are worst cases.

### Feeding the last try the standard decode's output: research (2026-11-19) [S33] [S34] [S8] [S35]

**What ours does now.** The last try (passes 8 to 10, `src\decoder\legacy_decoder.cpp`) orders its search by the received values alone, after the standard decode (belief propagation) has given up. The standard decode's own output is thrown away. [S6]

**What the papers found** (all four tested on/off signals in steady noise, not 8-tone signals or fading):
- **Fossorier 2001** [S33]: run the search after every round of the standard decode, ordered by that round's output, and score each candidate against the received values. On a 504-bit code this beat the standard decode by 0.55 dB, on a 1008-bit code by 0.25 dB, on a 155-bit code by about 1.2 dB. The 155-bit result matched a plain order-4 search after 10 rounds. Shorter codes gain more; ours is 174 bits. **Running the standard decode to the end and then searching once, as ours does, gave "negligible improvement"** (his Fig. 8).
- **Zhang, Yue, Li, Vucetic 2023** [S34]: when the standard decode fails, run it again from the start for only a few rounds, with the feedback from the checks multiplied by a setting below 1 (best 0.5 to 0.65 on their code, found by trial), then search once on that output. The number of rounds is ⌊girth/4⌋ + 1, where girth is the shortest loop in the code's check graph. Their order-2 search fed this way matched a plain order-3 search; order-3 matched a plain order-4. Feeding the search the output after 30 rounds was worse on both error rate and time.
- **Li and Yu 2025** [S8]: a trained neural network turns the standard decode's round-by-round output into the search order. Needs training; heavier.
- **Ma and Du 2015** [S35]: adds up the standard decode's values over all rounds (the method of Jiang, Zhao, Xu, Zhang 2007, which could not be found free) and feeds the total to the search.

**Our code's girth, computed 2026-11-19** from `kBitChecks` (`src\decoder\decoder_tables.hpp`, 174 bits, 87 checks): **8**. Zhang's rule gives **3 rounds**.

**Choice: Zhang's method first.** It keeps one search per pass, as now, so the chance of a wrong message per result reaching the CRC check stays 1 in 4,096. Fossorier's version searches after every round: more results reaching the CRC check (how many more is not known) and more time. Scoring stays against the received values, as in Fossorier, so the discrepancy and runner-up limits keep their meaning.

**Not known before measuring:** whether the gain carries over to 8-tone signals with fading; how much it adds to our order-2 search with the Wu and Hadjicostis order-3/4 additions; the best feedback setting for our code.

### Feeding the last try the standard decode's output: built and measured (2026-11-19) [S36]

**What was built.** `refine_reliabilities` (`src\decoder\ldpc.cpp`): 3 rounds of belief propagation on the last try's evidence, check messages multiplied by 0.6. `ordered_statistics_decode` takes the result as `order_by`: it picks the search positions and starting bits; candidates are still scored against the evidence. Passes 8 to 10 use it (`kRefineRounds`, `kRefineFeedback`, `src\decoder\legacy_decoder.cpp`). The callsign-hint retry is unchanged.

**Settings.** On the fading set (-17 to -20 dB), 2 to 4 rounds and feedback 0.5 to 1.0 all gave 285 to 288 messages against 282. The setting barely matters; 3 rounds (Zhang's rule) and 0.6 kept.

**A slowdown found and removed.** With the refine on, the order-4 search scored 18 times as many candidates on the crowded set (3.45 million → 61.7 million in Normal, 5.3 → 253 million in Deep): time per file +26% Normal, +53% Deep. Cause: the refined starting bits agree with more of the reliable positions, so the Wu and Hadjicostis filter passes far more candidates. Judging the filter against the received bits instead changed almost nothing (59.4 million). Without the order-4 search, the refine keeps almost all its gain: order 4 adds 53 messages without the refine but only about 9 with it, as Zhang reports (a refined order m - 1 search matches a plain order m). **So the order-4 search is skipped when the search is refined.**

**Results, all six Normal-speed signal sets plus 400 noise files** (chances = last-try results reaching the CRC check that are not the sent message, each 1 in 4,096):

| | Normal messages | Normal chances | Deep messages | Deep chances | Wrong seen |
|---|---|---|---|---|---|
| Before | 2,907 | 3,159 | 3,061 | 5,068 | Normal 0, Deep 1 |
| **Refine, gap 0.002 (built)** | **2,974** | **3,390** | **3,124** | **5,324** | Normal 0, Deep 1 |
| Refine, gap 0.0025 (not built) | 2,966 | 2,867 | 3,108 | 4,425 | Normal 0, Deep 1 |
| Before, gap 0.0025 (for comparison) | 2,901 | 2,716 | not run | not run | Normal 0 |

- Built version: Normal +67 messages (+2.3%) for +231 chances (+7.3%, expected wrong 0.77 → 0.83 on these sets); Deep +63 (+2.1%) for +256 (+5.1%).
- With the runner-up gap raised to 0.0025, the refine gives more messages **and** fewer chances than before in both modes. Not built: the gap is Rob's decision.
- Noise files: no decodes in any setting.
- **Time,** crowded set, one file at a time: Normal 0.51 s per file before and after; Deep 0.92 s before, 0.91 s after.
- **Fast and Turbo** (all their sets): Fast Normal 3,183 → 3,223, Deep 3,246 → 3,283; Turbo Normal 2,476 → 2,500, Deep 2,524 → 2,556. Wrong messages: the same 4 (Fast) and 1 (Turbo) as before in Normal, the same files and texts; in Deep the same counts (texts not compared). Wrong-message chances at these speeds not counted.
- **Checks:** with the refine switched off, the test build matched the previous decoder file for file (fade-free set). The built decoder matches the measured test setting file for file on all seven Normal-speed sets in Normal and Deep. 19 of 19 project tests pass.

**Runner-up gap raised to 0.0025, built 2026-11-19 (Rob's decision)** [S36]. No source gives a gap value for a code like ours; Forney's rule, Ma and Du and Zhang et al. all set their thresholds by measurement [S26] [S27] [S34] [S35]. Measured against the decoder before the refine (gap 0.002), all sets, messages / wrong-message chances:

| | Before | Now (refine, gap 0.0025) | Change |
|---|---|---|---|
| Normal speed, Normal | 2,907 / 3,159 | 2,966 / 2,867 | +59 messages, −9% chances |
| Normal speed, Deep | 3,061 / 5,068 | 3,108 / 4,425 | +47, −13% |
| Fast, Normal | 3,183 / 1,960 | 3,200 / 1,684 | +17, −14% |
| Fast, Deep | 3,246 / 3,098 | 3,267 / 2,508 | +21, −19% |
| Turbo, Normal | 2,476 / 2,072 | 2,491 / 1,676 | +15, −19% |
| Turbo, Deep | 2,524 / 3,335 | 2,544 / 2,588 | +20, −22% |

- Wrong messages seen: Normal speed 0 (Normal) and 1 (Deep), as before; Fast 4 → 3; Turbo 1 → 1. Noise files: no decodes.
- Against the refine at gap 0.002 (f173817), 0.0025 costs 8 to 23 messages per speed and mode and removes 14% to 20% of the chances.
- Checks: the built decoder matches the measured test setting file for file on all seven Normal-speed sets in Normal and Deep; 19 of 19 project tests pass. Time not re-measured (the gap only rejects answers already found).
- Caution from Sauter et al. [S26]: gap-style tests lose much of their benefit when the receiver's picture of the signal is off, as on real fading audio; on-air behaviour is not known.

### Normal's 10-sync-tone rule for the 3-tone last try: research and measurement (2026-11-19) [S25] [S37] [S38] [S39]

The rule (`kBlockOsdMinSyncMatches = 10`, `src\decoder\legacy_decoder.cpp`) came from four events at exactly 9 sync tones (2 right, 2 wrong), later shown to be chance (above).

**Research:**
- **WSJT-X** [S25]: one sync minimum, checked once before any decoding: more than 8 of 21 in Normal, more than 6 in Deep (more than 7 for the 2-symbol reading). The same minimum for its 1-, 2- and 3-symbol readings. Never stricter for the 3-symbol reading.
- **ft8mon** [S38]: no sync minimum for its last try. It runs the last try only when the standard decode satisfied at least 70 of 83 checks.
- **WSJT-X developers, 2017** [S37]: on 527 real recordings the last try gave 14 of 574 decodes, none false. Franke: the last try "can only be successful if it is given candidates". No sync numbers.
- **Published papers:** none found that sets a sync minimum for a last try.

**Measured** [S39]: Normal mode, today's decoder (refine, gap 0.0025), pass 10 at 10 sync tones (now) against 9. Messages / wrong-message chances:

| | 10 sync tones (now) | 9 sync tones | Change |
|---|---|---|---|
| Normal speed (7 sets) | 2,966 / 2,867 | 2,977 / 3,150 | +11 messages (+0.4%), +283 chances (+9.9%) |
| Fast (6 sets) | 18,489 / 12,508 | 18,517 / 13,474 | +28 (+0.15%), +966 (+7.7%) |
| Turbo (6 sets) | 9,545 / 11,043 | 9,574 / 11,807 | +29 (+0.3%), +764 (+6.9%) |

- Noise files, chances: Normal speed 22 → 31, Fast 10 → 16, Turbo 0 → 0. No decodes on noise. Wrong messages seen: unchanged (0, 4, 3).
- 9 sync tones gains about 120 to 160 messages per expected extra wrong message. Deep against Normal (same build) gains 142 messages for 1,558 chances, about 370 messages per expected wrong message. Dropping the rule is a worse trade than the one Deep already makes.
- **Decision: the rule stays.** Normal keeps 10; Deep keeps 9. The reason is now this measurement, not the four events. No code change.

## 5. The fading gap

- Until 2026-11-19 all our test audio was fade-free: tones at constant strength plus noise. [S3] The generator now has the ITU fading model; see "Measured on fading" below. [S9]
- FT8 loses 2.2 dB going from fade-free to mid-latitude disturbed fading (-20.8 to -18.6). [S1, Table 6]
- Reading 1, 2 and 3 tones together gains 0.7 dB fade-free but only 0.5 dB on that fading channel. [S1, Table 5]
- The paper builds its fading tests from the ITU standard model, ITU-R F.1487 [S7]: two fading paths with set frequency spread and delay. [S1, page 14]

### The ITU fading model (read in full) [S7]

- **Model (Annex 1, section 2, the "Watterson model"):**
  - The signal passes through a delay line with one tap per path.
  - Each tap multiplies the signal by a random complex gain: zero mean, real and imaginary parts independent with equal strength (Rayleigh fading).
  - Each gain's power spectrum is a Gaussian curve. "Frequency spread" is 2σ of that curve.
  - Noise is added after the paths are summed.
- **Test setup (Annex 3, section 1):** two independently fading paths, equal average strength, equal frequency spread, no frequency shift, separated by the "differential time delay".
- **Parameter sets (Annex 3, sections 2 to 4):**

  | Condition | Frequency spread (Hz) | Differential delay (ms) |
  |---|---|---|
  | Low latitude, quiet | 0.5 | 0.5 |
  | Low latitude, moderate | 1.5 | 2 |
  | Low latitude, disturbed | 10 | 6 |
  | Mid-latitude, quiet | 0.1 | 0.5 |
  | Mid-latitude, moderate | 0.5 | 1 |
  | Mid-latitude, disturbed | 1 | 2 |
  | Mid-latitude, disturbed near vertical incidence | 1 | 7 |
  | High latitude, quiet | 0.5 | 1 |
  | High latitude, moderate | 10 | 3 |
  | High latitude, disturbed | 30 | 7 |

- These match the values the FT8 paper lists in its Table 6. [S1, page 14]
- **Test length (Annex 3, section 6):** at least 3,000 ÷ frequency spread seconds. At 1 Hz that is 3,000 s, about 200 JS8 Normal messages of 15 s each, per signal level. At 0.1 Hz it is 30,000 s. Fading tests need far more messages than our fade-free tests.
- Nothing needed to build the model into our test audio is missing from the standard.

### Measured on fading (2026-11-19) [S9]

**The fading model in the test generator.**
- `scorecard\synth_gen.cpp` has a new option, `fade=<spread Hz>,<delay ms>`. It applies the two-path model above: each path's gain is filtered complex Gaussian noise with a Gaussian spectrum of σ = spread ÷ 2, and the paths have equal average power. `files=` and `levels=` set the set size.
- **Checked against the standard** at spreads of 0.1, 1, 10 and 30 Hz. All within a few percent of the model:
  - average power 0.499 to 0.512 (want 0.500);
  - time spent below 10% of average power 9.4% to 9.9% (want 9.5%, the Rayleigh figure);
  - correlation of the gain with itself 0.25 ÷ spread s later, 0.737 to 0.742 (want 0.735); 0.5 ÷ spread s later, 0.288 to 0.315 (want 0.291);
  - correlation between the two paths -0.005 to 0.007 (want 0).
- Without `fade=`, output is unchanged: the old 48-file set came out byte-identical.
- A faded set has the same messages, frequencies, times and noise as the fade-free set with the same seed, so the two compare directly.

**Test.** Seed 11, levels -15 to -22 dB, 50 files of 4 signals per level: 200 messages per level, the minimum the standard asks for at 1 Hz spread. One set mid-latitude disturbed (`fade=1,2`, folder `scorecard\ffade`), one fade-free (folder `scorecard\fflat`). Decoders: ours at commit b85d0af without callsign hints, and the PC decoder run as a test program.

| Level (dB) | Ours, fade-free | Ours, fading | PC, fade-free | PC, fading |
|---|---|---|---|---|
| -15 | 200 | 187 | 200 | 170 |
| -16 | 200 | 170 | 200 | 140 |
| -17 | 199 | 134 | 198 | 87 |
| -18 | 198 | 84 | 195 | 55 |
| -19 | 187 | 47 | 164 | 14 |
| -20 | 120 | 16 | 87 | 6 |
| -21 | 30 | 4 | 16 | 0 |
| -22 | 3 | 0 | 4 | 0 |
| Wrong messages | 0 | 1 | 0 | 0 |

Half decode at (straight line between the two levels around 50%):

| Decoder | Fade-free | Fading | Loss |
|---|---|---|---|
| Ours | -20.2 dB | -17.7 dB | 2.5 dB |
| PC decoder | -19.8 dB | -16.8 dB | 3.0 dB |
| FT8, best setup in the paper [S1, Table 6] | -20.8 dB | -18.6 dB | 2.2 dB |

- Fade-free, this new 1,600-message set gives the same -20.2 dB as the older sets in section 2.

**Which decoder pass found each message** (a diagnostic copy of the decoder, never the repo file):

| Pass | Fade-free | Fading |
|---|---|---|
| 1 to 5, standard decode, 1-tone readings | 1,024 | 429 |
| 6, standard decode, 3-tone readings | 34 | 4 |
| 7, last-try decode, 1-tone readings | 59 | 193 |
| 8, last-try decode, 3-tone readings | 20 | 16 right, 1 wrong |

- On fading, the last-try decode found 210 of 643 messages (33%); fade-free, 79 of 1,137 (7%). So improvements to the last try (section 4, method 2) should matter more on fading than fade-free. This is reasoning from these counts, not a measurement of a deeper last try.

**The wrong message.**
- File `ffade\S-19_21.wav`. A real faded signal was at 1469.64 Hz; the decoder reported a different message at 1469.25 Hz, at the right time.
- Found by pass 8 in the first subtraction pass, with 9 of 21 sync tones matching (the lowest allowed, `kOsdMinSyncMatches`, `src\decoder\legacy_decoder.cpp` line 54) and a discrepancy of 0.056 of the evidence (limit 0.065, line 55).
- Neither limit separates it from right answers on this set:
  - 29 of the 209 right last-try decodes had a higher discrepancy than 0.056; lowering the limit to stop this one would lose them.
  - 16 right last-try decodes had 9 sync tones; raising the minimum to 10 would lose them to stop this one.
- So it is 1 wrong in 643 faded messages, with no simple fix found.

### Fast and Turbo speeds, measured (2026-11-19) [S31]

**Setup.**
- `scorecard\synth_gen.cpp` takes `speed=fast` (1,200 samples per tone, 10 s cycle, start 0.2 s, Modified sync pattern) or `speed=turbo` (600 samples, 6 s, 0.1 s, Modified). Values from `include\js8core\protocol\constants.hpp` and `src\decoder\decoder_tables.hpp`.
- `tools\wav_decode_test.cpp` takes modes `B` (Fast) and `C` (Turbo).
- Checked at -10 dB: 12 of 12 decode at each speed; Turbo audio read as Fast gives 0.
- Decoders: original JS8Core (commit cba17f3, only the test program given modes B and C; `scorecard\bin\dec-orig`), ours at 7231c03 in Normal (`dec-bc`) and with `--deep`.
- Sets, each at the levels where the decoders differ (found by a 32-signal sweep): fade-free `bflat` / `cflat`, fading (mid-latitude disturbed, `fade=1,2`) `bfade` / `cfade`, noise `bnoise` / `cnoise` (400 files), crowded `bcrowd` (37 signals per file) / `ccrowd` (19 signals per file). The crowded counts give the same overlap as the 60-signal Normal set: Fast signals are 80 Hz wide, Turbo 160 Hz.

**Messages decoded** (total over the levels shown; wrong messages in brackets):

| Set | Levels (dB) | Signals | Original | Ours, Normal | Ours, Deep |
|---|---|---|---|---|---|
| Fast fade-free | -15 to -18 | 1,600 | 880 | 1,055 | 1,058 |
| Fast fading | -12 to -17 | 2,400 | 945 | 1,289 (1 wrong) | 1,313 (1 wrong) |
| Fast crowded | -15 to -18 | 2,960 | 506 | 839 (3 wrong) | 875 (3 wrong) |
| Turbo fade-free | -12 to -15 | 1,600 | 747 | 948 | 958 |
| Turbo fading | -9 to -14 | 2,400 | 809 | 1,115 | 1,133 |
| Turbo crowded | -12 to -15 | 1,520 | 187 | 413 (1 wrong) | 433 (1 wrong) |
| Noise, both speeds | — | 800 files | 0 wrong | 0 wrong | 0 wrong |

**Level where half the signals decode** (straight line between the two levels either side):

| Set | Original | Ours, Normal | Ours, Deep | Gain, Normal |
|---|---|---|---|---|
| Fast fade-free | -16.9 | -17.3 | -17.3 | 0.5 dB |
| Fast fading | -13.7 | -15.0 | -15.1 | 1.3 dB |
| Turbo fade-free | -13.5 | -14.0 | -14.0 | 0.5 dB |
| Turbo fading | -10.1 | -11.4 | -11.5 | 1.3 dB |

For comparison, Normal speed (section 2): ours half at -20.2 fade-free.

**Time per file** (seconds; original / ours Normal / ours Deep): Fast fade-free 0.078 / 0.096 / 0.118; Fast fading 0.075 / 0.093 / 0.118; Turbo fade-free 0.057 / 0.070 / 0.087; Turbo fading 0.055 / 0.070 / 0.092; Fast crowded 0.205 / 0.417 / 0.667; Turbo crowded 0.116 / 0.216 / 0.338. On the crowded Fast set Normal is 2.03 times the original's time, just over the limit of twice as slow used for Normal speed; Deep is 3.3 times.

**Speed fix, 2026-11-19** [S32]:
- Where the extra time went (timed copy, crowded Fast set, per file): ours spent 0.36 s in decode attempts against the original's 0.15 s. Last try on standard readings (pass 8) 0.093 s, last try on 3-tone readings (pass 10) 0.066 s, 2- and 3-tone belief propagation (passes 6 and 7) 0.038 s, extra subtraction rounds 0.010 s.
- Inside the last try: scoring candidates for orders 0 to 2 took 0.084 of 0.137 s, because each of about 3,800 candidates was scored by testing all 174 bits.
- Change (`src\decoder\ldpc.cpp`): a candidate is held as three 64-bit words and its score visits only the bits that differ, lowest first, so the same numbers are added in the same order.
- Checked: output identical line for line (message, SNR, frequency, time, quality) on 15 sets in Normal and Deep (`scorecard\.cmp_exact.py`): `bnoise bflat bfade bcrowd cnoise cflat cfade ccrowd noise4 fflat ffade fflat2 ffade2 crowd60 crowd2`, 30 of 30 runs with 0 files differing. 19 of 19 project tests pass.
- Time per file, alternating file by file (original / before / after): crowded Fast Normal 0.217 / 0.435 / 0.336 (2.00 → 1.55 times the original); crowded Fast Deep — / 0.684 / 0.541; Normal-speed crowded (`crowd60`) Normal 0.249 / 0.658 / 0.485 (2.64 → 1.95 times the original).
- The Normal-speed crowded figure was over twice the original before this fix: the earlier "twice as slow" checks compared against commit b85d0af, not the original JS8Core decoder. It is now 1.95 times, inside the limit with little margin.

**Wrong messages.**
- All 5 came from the last try (passes 8 and 10), the same 5 in Normal and Deep. The original decoder has no last try and gave none.
- In 4 of the 5 the wrong message sits at the frequency of a real sent signal (990 Hz against 989, 725 against 725, 989 against 989, 940 against 938): the last try turned a real weak signal into a wrong message.
- Counted with `scorecard\.patch_lasttry.py` on all 8 sets: about 1.0 wrong message expected at 1 in 4,096 per last-try result reaching the CRC check, 5 seen. Checked on fresh sets: `bcrowd2` / `ccrowd2` (seeds 57 / 58, 180 files each) 0.95 expected, 2 seen; `bcrowd3` / `ccrowd3` (seeds 59 / 60, 900 files each) 4.7 expected, 6 seen. The fresh sets fit the 1 in 4,096 rate; the first sets were an unlucky draw.
- What that means on a crowded band of weak signals, Normal mode: Fast about 11 last-try results per cycle reach the CRC check without being the sent message, 1 wrong message per about 366 cycles (about 1 hour); Turbo about 10 per cycle, 1 per about 394 cycles (about 40 minutes).

## 6. ft8_lib comparison

ft8_lib by Karlis Goba (MIT license) [S2]:

- Reads 1 tone at a time on a dB scale and scales the readings to a fixed spread (value 24, marked as found by experiment).
- Standard decode only: 25 rounds, no deep last try.
- One pass, with no removal of decoded signals.

It is simpler than our decoder, so nothing in it is worth borrowing.

## 7. Correction to an earlier claim

- Earlier I said "about half of the weakest signals are never noticed". No measurement supports it.
- A diagnostic run on 2026-11-18 found:
  - at -21 dB, 4 of 32 signals were never noticed;
  - at -19 and -20 dB, none were missed;
  - most failed signals were noticed, with 8 to 16 of 21 sync tones matching, and then failed in error correction.
- That run's output is only in that day's session record; it wasn't saved as a file.
- So the bigger losses are in reading and decoding, not in finding signals.

## 8. Recommended order

1. Add the ITU-R F.1487 fading model [S7] to the test audio generator, so changes are judged on realistic signals. **Done 2026-11-19** (section 5). [S9]
2. Add reading 2 tones at a time, and measure it. **Done 2026-11-19, with a Deep option** (section 4). [S13]
3. Tune the scaling setting, and measure it. **Measured 2026-11-19** (section 4): 6.0 is best on every set, with 1.5 times the time on a crowded band. **Changed 2026-11-19 for Deep only**; Normal keeps 2.83. [S19]
4. Deepen the last-try decode using the Wu and Hadjicostis shortcuts [S5], and measure it. **Done 2026-11-19** (section 4). [S10]

## 9. Not known yet

- **Tone shape, settled for JS8Core:** JS8Core's transmitter switches tones sharply with continuous phase and no smoothing (`src\tx\modulator.cpp`, lines 161 to 174). Our test signals use the same shape (`scorecard\synth_gen.cpp`, lines 131 to 133). FT8 smooths tone changes (Gaussian shaping, BT = 2) [S1, page 9], so the comparison with the paper may shift slightly. What other JS8 programs transmit is still unknown; their source is on the do-not-open list.
- How much each of the three methods adds on top of what our decoder already has. The paper does not break this out.
- ~~The Fast and Turbo speeds: none of this research covers them.~~ Measured 2026-11-19 (section 5) on computer-made audio. Still open: real on-air audio at these speeds, and the Slow speed.
- ~~How often the last-try decode lets a wrong message through near real signals.~~ Measured 2026-11-19 (section 4): about 1 in 4,096 per last-try result reaching the CRC check. Still open: real on-air audio and the other nine fading conditions. (The runner-up gap, now 0.0025, was measured on Fast and Turbo too; section 4.)
- ~~Normal's rule of 10 sync tones for the 3-tone last try (`kBlockOsdMinSyncMatches`) rests on the "9 sync tones" pattern, which turned out to be chance.~~ Re-measured 2026-11-19 (section 4): kept; 9 sync tones adds 7% to 10% more wrong-message chances for 0.15% to 0.4% more messages.
- How fast the decoder runs on a phone. Not tested (phone testing is off until Rob says).
- Our decoder on the other nine ITU fading conditions. Only mid-latitude disturbed has been run.
- Whether retuning the standard decode's early-stop rule would keep the bigger scale's gains without its slowdown on a crowded band. [S19]
- What a wrong scale costs an LDPC sum-product decoder in published measurements: the main paper (Saeedi and Banihashemi 2007) could not be reached. [S18]

## Sources

| ID | Source | How far I checked it |
|---|---|---|
| S1 | S. Franke, B. Somerville, J. Taylor, "The FT4 and FT8 Communication Protocols", QEX, July/August 2020, pages 7 to 17. https://wsjt.sourceforge.io/FT4_FT8_QEX.pdf | Read in full |
| S2 | K. Goba, ft8_lib (MIT license). https://github.com/kgoba/ft8_lib (files `ft8/decode.c`, `ft8/ldpc.c`, `common/monitor.c`, `demo/decode_ft8.c`, `README.md`) | Read |
| S3 | Our saved test results: `scorecard\records-synth.tsv`, `records-dsynth.tsv`, `records-dsynth2.tsv`, `records-dsynth3.tsv`, and `truth.tsv` in folders `synth`, `dsynth`, `dsynth2`, `dsynth3`. Test audio generator: `scorecard\synth_gen.cpp` | Counted |
| S4 | M. C. Valenti, S. Cheng, "Iterative demodulation and decoding of turbo-coded M-ary noncoherent orthogonal modulation", IEEE Journal on Selected Areas in Communications, vol. 23, no. 9, September 2005, pages 1739 to 1747. https://community.wvu.edu/~mcvalenti/documents/ValentiChengJSAC2005.pdf | Read in full 2026-11-19 (text; the equations are images) |
| S5 | Y. Wu, C. N. Hadjicostis, "Soft-Decision Decoding of Linear Block Codes Using Preprocessing and Diversification", IEEE Transactions on Information Theory, vol. 53, no. 1, January 2007, pages 378 to 393. DOI 10.1109/TIT.2006.887478. Copy read: `docs/Soft_Decision_Decoding_of_Linear_Block_C.pdf` (17 pages) | Read in full |
| S6 | Our decoder: `src\decoder\legacy_decoder.cpp` and `src\decoder\decoder_tables.hpp` (lines 19 to 29: 174 coded bits, 58 data tones, 79 tones, 8 tone frequencies), commit b85d0af | Read |
| S7 | Recommendation ITU-R F.1487, "Testing of HF modems with bandwidths of up to about 12 kHz using ionospheric channel simulators", ITU, 2000. https://www.itu.int/rec/R-REC-F.1487-0-200005-I/en | Read in full |
| S8 | Li and Yu, "Sliding Window-Aided Ordered Statistics Decoding for Short LDPC Codes", arXiv 2404.14165, 2024. https://arxiv.org/html/2404.14165v2; version 5 (2025, published in IEEE Communications Letters as "Boosting Ordered Statistics Decoding of Short LDPC Codes with Simple Neural Network Models") | v2 read (main text; conclusion only as a summary); v5 read in full 2026-11-19 |
| S11 | WSJT-X user guide, Decode menu (Fast, Normal, Deep). https://sources.debian.org/src/wsjtx-improved/3.0.0%2B250924%2Brepack-1/doc/user_guide/en/menus.adoc | Read as a search summary |
| S12 | M. K. Simon, D. Divsalar, "Maximum-Likelihood Block Detection of Noncoherent Continuous Phase Modulation", IEEE Transactions on Communications, vol. 41, no. 1, January 1993 (cited by S1 as [15]) | Not read: paywalled; abstract only, via search |
| S13 | 2-tone reading and Deep option test, 2026-11-19: sets `scorecard\noise4`, `ffade`, `fflat`, `crowd60` (60 signals per file, seed 31, generator option `signals=`), decoders `scorecard\bin\dec-head` (b85d0af), `dec-deep` (Normal), `dec-deepon` (Deep), scripts `scorecard\.run_stop.py`, `.speed.py`, `.reach_count.py`, `.diag_reach.py`, `.diag_osd.py`, `.diag_wrong.py` | Run and counted |
| S10 | Deeper last-try test, 2026-11-19: `src\decoder\ldpc.cpp` (filtered search), check program `scorecard\.osdcheck.cpp`, noise set `scorecard\noise4` (400 files, seed 21), new decoder `scorecard\bin\dec-osd4`, limit diagnostics `scorecard\.diag_gate.py`, `.gate_count.py`, `.gate_noise.py` | Run and counted |
| S14 | S. Cheng, M. C. Valenti, D. Torrieri, "Turbo-NFSK: Iterative Estimation, Noncoherent Demodulation, and Decoding for Fast Fading Channels", MILCOM 2005 (https://community.wvu.edu/~mcvalenti/documents/Milcom2005.pdf), and "Robust Iterative Noncoherent Reception of Coded FSK over Block Fading Channels", IEEE Transactions on Wireless Communications, vol. 6, no. 9, September 2007, pages 3142 to 3147 (https://community.wvu.edu/~mcvalenti/documents/J13ChengValentiTorrieriTWireless2006.pdf) | Both read in full |
| S15 | WSJT-X `lib/ft8/ft8b.f90`, SourceForge master. https://sourceforge.net/p/wsjt/wsjtx/ci/master/tree/lib/ft8/ft8b.f90 | Read (metric, `normalizebmet`, `scalefac`) |
| S16 | Rhizomatica mercury, pull request 359, "Ldpc decoder". https://github.com/Rhizomatica/mercury/pull/359 | Read in part; two comment ranges and the linked design record and data files could not be fetched |
| S17 | T. A. Summers, S. G. Wilson, "SNR mismatch and online estimation in turbo decoding", IEEE Transactions on Communications, vol. 46, no. 4, 1998, pages 421 to 423. https://ieeexplore.ieee.org/abstract/document/664291 | Not read: abstract and citing papers only, via search |
| S18 | H. Saeedi, A. H. Banihashemi, "Performance of belief propagation for decoding LDPC codes in the presence of channel estimation error", IEEE Transactions on Communications, vol. 55, no. 1, January 2007, pages 83 to 89 | Not read: ResearchGate refused the connection; no free copy found |
| S19 | Scaling and try-limit tests, 2026-11-19: sets `scorecard\ffade`, `fflat`, `crowd60`, `noise4`; scripts `scorecard\.patch_scale.py` (scale settable by `JS8_SCALE_A`, `JS8_SCALE_L`), `.patch_cap.py` (try limit settable by `JS8_MAXITER`, counts tries per success), `.patch_reach.py`, `.patch_bpcount.py` (diagnostic copies, never the repo file), `.scale_run.py` (`DEEP=1` runs with Deep on, `CTL` picks the control decoder), `.noise_reach.py`; control decoder `scorecard\bin\dec-deep`; decoder after the change `scorecard\bin\dec-scale6` | Run and counted |
| S20 | J. Taylor, "Re: [wsjt-devel] FT8 notes", wsjt-devel mailing list, 30 June 2017. https://sourceforge.net/p/wsjt/mailman/message/35921298/ | Read |
| S21 | T. Mohsenin, H. Shirani-mehr, B. Baas, "Low Power LDPC Decoder with Efficient Stopping Scheme for Undecodable Blocks", ISCAS 2011. http://vcl.ece.ucdavis.edu/pubs/2011.05.ISCAS.LDPC/ldpc.iscas.pdf | Read in full |
| S22 | E. Amador, R. Knopp, V. Rezard, R. Pacalet, "Hybrid Iteration Control on LDPC Decoders" (Eurecom). https://www.eurecom.fr/en/publication/3166/download/cm-publi-3166.pdf | Read in full |
| S23 | M. Lian, F. Carpi, C. Häger, H. D. Pfister, "Learned Belief-Propagation Decoding with Simple Scaling and SNR Adaptation", arXiv 1901.08621, 2019 | Read in full |
| S24 | J. Wu, B. R. Vojcic, J. Sheng, "Stopping Criteria for Iterative Decoding based on Mutual Information", arXiv 1302.1461, 2013. Also read and found not to bear on this: S. Kudekar, T. Richardson, A. Iyengar, "The Effect of Saturation on Belief Propagation Decoding of LDPC Codes", arXiv 1403.3678 | Read in full |
| S25 | WSJT-X `lib/ft8/ft8b.f90` and `lib/ft8_decode.f90`, GitHub mirror master. https://github.com/WSJTX/wsjtx/blob/master/lib/ft8/ft8b.f90 | Read (sync minimum, last-try acceptance, false-decode rule, quality figure); research only, nothing copied |
| S26 | A. Sauter, A. O. Kislal, G. Durisi, G. Liva, B. Matuz, E. G. Ström, "Undetected Error Probability in the Short Blocklength Regime: Approaching Finite-Blocklength Bounds with Polar Codes", arXiv 2503.02782, 2025 | Read (introduction, Forney's rule, threshold test, mismatch results, conclusions) |
| S27 | P. Yuan, M. Médard, K. Galligan, K. R. Duffy, "Soft-output (SO) GRAND and Iterative Decoding to Outperform LDPCs", arXiv 2310.10737v5, 2024 | Read (sections on Forney's approximation and Theorem 1 / Corollary 1) |
| S28 | N. Weinberger, N. Merhav, "Simplified Erasure/List Decoding", arXiv 1412.1964, 2014 | Abstract and introduction only (theory of error exponents; no practical rule beyond Forney's) |
| S29 | JS8Core frame unpacking: `src\protocol\varicode_frames.cpp`, `include\js8core\protocol\varicode.hpp` | Read (function list and reject points) |
| S30 | Wrong-message tests, 2026-11-19: sets `scorecard\ffade`, `fflat`, `crowd60`, `noise4` and fresh `ffade2`, `fflat2`, `crowd2`; scripts `scorecard\.patch_lasttry.py` (per result: pass, sync, hard errors, discrepancy, CRC, reported SNR), `.patch_gap.py` (runner-up gap), `.patch_gaprule.py` (gap test settable by `JS8_GAP`), `.lasttry_run.py`, `.gap_run.py`, `.gap_e2e.py`, `.cmp_bins.py`, `.formatcheck.cpp` (random frames through JS8Core's unpackers); decoders `scorecard\bin\dec-scale6` (before), `dec-forney` (Deep only), `dec-forney2` (Normal and Deep) | Run and counted |
| S31 | Fast and Turbo tests, 2026-11-19: `scorecard\synth_gen.cpp` (`speed=` option), `tools\wav_decode_test.cpp` (modes B and C); sets `scorecard\bflat`, `bfade`, `bnoise`, `bcrowd`, `cflat`, `cfade`, `cnoise`, `ccrowd` (seeds 51 to 56) and fresh `bcrowd2`, `ccrowd2`, `bcrowd3`, `ccrowd3` (seeds 57 to 60); scripts `.run_stop.py`, `.speed.py`, `.lasttry_run.py` (each now takes `MODE=`); decoders `scorecard\bin\dec-orig` (cba17f3 plus modes B and C in the test program), `dec-bc` (7231c03 plus modes B and C) | Run and counted |
| S32 | Speed tests, 2026-11-19: timing patches `scorecard\.patch_passtime.py` (per pass, per subtraction round), `.patch_osdtime.py` (parts of the last try), summaries `.passtime_sum.py`, `.round_sum.py`; identical-output check `.cmp_exact.py`; timing `.speed.py`; decoders `scorecard\bin\dec-orig`, `dec-bc` (before), `dec-fast1` (after), wrappers `dec-bcdeep`, `dec-fast1deep` | Run and timed |
| S33 | M. P. C. Fossorier, "Iterative reliability-based decoding of low-density parity check codes", IEEE Journal on Selected Areas in Communications, vol. 19, no. 5, May 2001, pages 908 to 917. DOI 10.1109/49.924874. Copy from academia.edu (uploaded by the author), downloaded by Rob | Read in full |
| S34 | W. Zhang, C. Yue, Y. Li, B. Vucetic, "Efficient Near Maximum-Likelihood Reliability-Based Decoding for Short LDPC Codes", arXiv 2306.00443v3, 2023 | Read in full |
| S35 | Ma Zhuo, Du Shuanyi, "Improved Reliability-Based Iterative Decoding of LDPC Codes Based on Dynamic Threshold", ETRI Journal, vol. 37, no. 4, August 2015. DOI 10.4218/etrij.15.0113.1265 | Read the parts on accumulated values and the threshold |
| S36 | Refine tests, 2026-11-19: all Normal-speed sets (`ffade`, `fflat`, `ffade2`, `fflat2`, `crowd60`, `crowd2`, `noise4`) and Fast/Turbo sets (`bflat`, `bfade`, `bnoise`, `bcrowd`, `cflat`, `cfade`, `cnoise`, `ccrowd`); patch `scorecard\.patch_refine.py` (settable `JS8_RR`, `JS8_RF`, `JS8_GAP`, `JS8_NO4`; COPY only), scripts `.refine_run.py`, `.refine_reach.py`, `.osdtime_sum.py`, `.cmp_exact.py`, `.speed.py`; decoders `scorecard\bin\dec-fast1` (before), `dec-refine` (test build), `dec-zhang` (built code), wrapper `dec-refinedeep` | Run, counted and timed |
| S37 | wsjt-devel mailing list, 2017: S. Franke, https://sourceforge.net/p/wsjt/mailman/message/35921321/ ; B. Somerville quoting J. Taylor, https://sourceforge.net/p/wsjt/mailman/message/35921283/ | Read |
| S38 | R. Morris, ft8mon, `ft8.cc` (lines 69 to 70, 2209). https://github.com/rtmrtmrtmrtm/ft8mon | Read (last-try gate); research only, nothing copied |
| S39 | Sync-rule test, 2026-11-19: patch `scorecard\.patch_sync10.py` (Normal's pass-10 sync minimum settable by `JS8_SYNC`; COPY only) on top of `.patch_refine.py` and `.patch_lasttry.py`; script `.refine_reach.py` on the Normal-speed sets and `bflat`, `bfade`, `bcrowd`, `bcrowd2`, `bcrowd3`, `bnoise`, `cflat`, `cfade`, `ccrowd`, `ccrowd2`, `ccrowd3`, `cnoise` | Run and counted |
| S9 | Our fading test, 2026-11-19: `scorecard\synth_gen.cpp` (fading option), sets `scorecard\ffade` and `scorecard\fflat` with their `truth.tsv`, results `scorecard\records-ffade.tsv` and `records-fflat.tsv`, decoder at commit b85d0af | Run and counted |
