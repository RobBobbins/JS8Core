# Decoder research, 2026-11-18

Where the js8core-new decoder stands against the best published decoder, and what to work on next. Research only on 2026-11-18. On 2026-11-19 the fading test was built and run; its results are in section 5. Every finding names its source; sources are listed at the end as [S1] to [S86].

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
- **The other ITU fading conditions, measured 2026-11-19 (Normal speed):** ours beats the original by 22% to 52% on every condition up to 1.5 Hz spread, more on faster fading. **At 10 Hz and 30 Hz spread nothing decodes, original or ours, even at 0 dB, while the FT8 paper gives WSJT-X a threshold of -8.6 dB at 10 Hz.** Cause not known. Section 5. [S1] [S40]
- **Fast fading (10 Hz spread) now decodes, built 2026-11-19:** a spread-aware tone reading and a last try on it, used only when the sync tones show spread. Fresh 10 Hz sets 101 → 253 messages, 0 wrong; ordinary sets unchanged, +0.7% wrong-message chances, +0.9% time. 30 Hz still 0. Section 5. [S42] [S47]
- **Iterative demapping, built 2026-11-20 (every speed, Normal and Deep):** pass 7 now decodes the spread reading with the tone reading redone after each decoder round, for every candidate; pass 6's 2-tone decode is retired. Ordinary Normal-speed sets 2,966 → 3,128 messages (+5.5%), Deep 3,108 → 3,241 (Normal now beats yesterday's Deep), Fast +5.9%, Turbo +4.5%, 10 Hz fading +17% (Normal) and +32% (Fast); fewer wrong-message chances on every set except the Normal-speed 10 Hz sets (702 → 705); crowded time unchanged at Normal speed, +1.5% to +1.9% at Fast and Turbo. Section 5. [S4] [S43] [S44] [S49]
- **Last try ordered with iterative demapping, built 2026-11-20, Deep only, not Turbo:** 10 Hz sets 587 → 596 in Deep for +5.6% chances there; ordinary sets unchanged. Opening the spread last try to every candidate was tested and rejected (noise chances doubled, one wrong message). Section 5. [S50]
- **Wide spread reading, built 2026-11-20 (Normal and Slow speed):** fast-fading signals are cut out again 4 tones wider each side and each tone is scored from bins up to 2 tones away. 10 Hz sets +34% (587 → 787), fresh 10 Hz sets +38% (260 → 358), with fewer wrong-message chances; ordinary sets unchanged; +0.6% time. At -8 dB, 10 Hz high-latitude moderate now decodes 50%, close to FT8's -8.6 dB. 30 Hz still 0. Section 5. [S1] [S42] [S51]
- **Slow speed, measured 2026-11-19:** the three fading changes above lose nothing at Slow: +78 messages in Normal (2,216 → 2,294), +66 in Deep, on six new Slow sets; fewer wrong-message chances in both modes; 0 wrong messages; time within ±3%. 10 Hz at Slow decodes nothing in either build. **Cause found:** the wide reading looks only 2 tones (6.25 Hz at Slow) each side; 4 tones (12.5 Hz, the same as Normal speed) decodes 51 of 800 on fresh Slow 10 Hz sets, 0 wrong, no extra time. **Built 2026-11-20, Slow only, Normal and Deep.** Then tested: smoothing the fit plus keeping weak-sync candidates the spread reading confirms nearly doubles Slow 10 Hz (fresh sets 49 → 74 of 800) and adds 11% to 22% at Normal-speed 10 Hz; not built (gate choice open). A retry design (smoothed fit only for candidates that fail) gains more with no losses, but doubles wrong-message chances and breaks Normal's time limit, so it fits only behind Deep. **Built 2026-11-20: gate 0.5 in Normal mode, retry every failure in Deep, both with the rescue, Normal and Slow speeds.** Deep: +5.9% ordinary messages, 1.9 times the chances, +59% crowded time. Section 5. [S54] [S55] [S60] [S61]
- **Against the original JS8Core decoder, measured 2026-11-20 on 44 sets at every speed:** ours decodes +66% at Normal speed (ordinary sets), +43% on slow fading, +64% Fast, +57% Turbo, +41% Slow, and 1,280 messages on 10 Hz fading where the original decodes none. Deep adds 1% to 16% more. Wrong messages: original 0, ours 5, Deep 11 (all from the last try). Crowded time: ours 1.43 to 1.91 times the original, Deep 2.15 to 5.14 times. Section 5; every set in `docsdecoder-vs-original.html`. [S62]
- **Deep's retry skips repeats, built 2026-11-20:** a third of Deep's retries landed on exactly the first attempt's fit and could only fail again (0 of about 2,470 decoded). Skipping them: same messages and wrong messages on every Normal- and Slow-speed set, 12% to 26% fewer counted wrong-message chances (the repeats were counted but could not produce a wrong message), crowded Deep time −14% at Normal speed (1.279 → 1.103 s) and −11% at Slow. No published source; WSJT-X Q65 does not skip repeats. Section 5. [S63]
- **Give-up rule at set rounds, tested 2026-11-20, not built:** on our 87-check code the failed-check count does not separate calls that will succeed from calls that won't; every setting loses 2 to 20 of 3,432 Deep messages, for at most 9% less crowded time. Section 5. [S21] [S64]
- **Damping, built 2026-11-20 at γ 0.1 (every speed, both modes):** blending each round's messages with the last round's (γ 0.1 or 0.2) adds messages at every speed in both modes (10 Hz fading +9%, elsewhere +0.4% to +1%; confirmed on fresh sets), same wrong messages, mostly fewer chances, no extra time; Deep −5% time. Section 5. [S23] [S65] Research after the test: no published γ fits our code (best values elsewhere 0.35 to 0.83, and one short-code study found no gain); all agree the value must be measured and too much damping loses decodes, matching our γ 0.5 loss. WSJT-X does not damp. [S66] to [S70] Built version checked: same messages, wrong messages and chances as the tested copy on every group, both modes; crowded time Normal mode unchanged at Fast and Turbo, Deep −5% and −4%. [S71]
- **Literature search, 2026-11-20 (section 10):** 11 leads found, best first: re-reading the same audio with small changes (+15.3% on recorded on-air FT8), hints for heartbeat and CQ frames (part of WSJT-X's "up to 4 dB"), a finer frequency fit from the sync tones' phases, and a phase check against wrong decodes. Nine areas searched with nothing usable. [S73] to [S80]
- **The time fit, tested 2026-11-20 (section 10):** candidate start times are early by 1.5 coarse steps (our candidate window peaks half a symbol in; WSJT-X's does not). A start-time fix plus WSJT-X's second time search: +13.5% messages at Normal speed (Deep +16.4%), +66% Fast, +62% Turbo, +21% to +23% Slow, +12% on 10 Hz fading; chances down on the signal sets except Slow and 10 Hz Deep; crowded time Normal mode 2.04 times the original. Re-reading the audio with small changes: +4.5% in Deep on top, for twice the chances: not worth building. Not built. [S73] [S81] [S82] [S83]
- **Heartbeat and CQ hints, tested 2026-11-20 (section 10):** one hint per heard station for its heartbeat and CQ frames: those frames +26% to +54% at every speed (all messages +20% on the Normal-speed test set), 0 wrong messages, every noise CRC pass rejected by the evidence limit; crowded time +10% to +38%. Found on the way: callsign hints with 16 heard stations make a crowded-band decode 1.5 to 4.6 times slower than with no hints, and no earlier time measurement included hints. Not built. [S84]
- **Time-fit fixes built 2026-11-21 (`4f21eb5`, section 10):** same results as the tested copies on every group. Against the original on the 44 sets: all speeds 39.8% → 51.6% of signals decoded in Normal mode (Deep 41.2% → 53.7%), Fast 68.0%, Turbo 62.1%; wrong messages 5 → 1 (Deep 11 → 5). The Slow Deep wrong messages were checked on fresh sets: chance level. Crowded time at Normal speed 2.04 times the original, over the 2-times limit. [S85]
- **Harsher audio, tested 2026-11-21 (section 10):** test audio with steady drift (up to 0.25 Hz/s) and station clock offsets (up to ±1 s). The gains over the original hold at every speed and mostly grow (Fast 2.08 → 2.30 times, Turbo 2.02 → 2.47). Drift costs every decoder over half its Slow-speed messages (a 25 s frame drifts up to two tone spacings): drift following is worth researching for Slow. [S86]

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

### The other ITU fading conditions, measured (2026-11-19) [S1] [S7] [S40]

Normal speed, Normal mode unless marked. One set per condition: 50 files × 4 signals per level, levels -15 to -22 dB (1,600 signals). High-latitude quiet (0.5 Hz, 1 ms) has the same numbers as mid-latitude moderate, so it was not run twice. Messages / wrong-message chances:

| Condition (spread Hz, delay ms) | Original (cba17f3) | Ours, Normal | Ours, Deep |
|---|---|---|---|
| Mid-latitude quiet (0.1, 0.5) | 595 | 726 / 340 (+22%) | 744 / 619 |
| Low-latitude quiet (0.5, 0.5) | 481 | 666 / 468 (+38%) | 686 / 728 |
| Mid-latitude moderate = high-latitude quiet (0.5, 1) | 478 | 682 / 416 (+43%) | 697 / 681 |
| Mid-latitude disturbed near vertical (1, 7) | 445 | 638 / 435 (+43%) | 653 / 739 |
| Low-latitude moderate (1.5, 2) | 397 | 603 / 450 (+52%) | 616 / 738 |
| Low-latitude disturbed (10, 6) | 0 | 0 / 92 | 0 / 184 |
| High-latitude moderate (10, 3) | 0 | 0 / 114 | 0 / 210 |
| High-latitude disturbed (30, 7) | 0 | 0 / 19 | 0 / 41 |

- Wrong messages seen: 0 in all three columns. Noise files (`noise4`): chances 22 Normal, 42 Deep; no decodes.
- **The gains hold on every condition up to 1.5 Hz**: +22% to +52% over the original. They grow with faster fading. Deep adds 2% to 3% more messages for about 1.7 times the chances.
- **The 2-tone and 3-tone readings did not hurt** where fading is fast but decodable (1.5 Hz): ours beats the original by the most there.
- **10 Hz and 30 Hz: nothing decodes, in the original or ours.** A spot check at 0, -5 and -10 dB (10 files each): 3 Hz decodes 40 of 40 at every level, 5 Hz 28 to 39 of 40, 10 Hz (3 ms and 0 ms delay) and 30 Hz 0 of 40, even at 0 dB.
- **This is a real gap, not the mode's limit.** The FT8 paper's Table 6 gives WSJT-X's FT8 a decoding threshold of -8.6 dB on high-latitude moderate (10 Hz, 3 ms) [S1, page 14]. JS8 Normal uses the same tones and tone spacing as FT8. Where JS8Core fails (finding the signal, sync, or the decode) is not known yet.
- The generator's fading math was re-checked against the standard: spread = 2σ of a Gaussian spectrum (`scorecard\synth_gen.cpp`, lines 75 to 84) [S7].
- On the 10 Hz and 30 Hz sets the last try spends 92 to 210 wrong-message chances for no messages.
- **Where the 10 Hz signals are lost (stage check, 2026-11-19)** [S41]: 24 signals (6 files) at 0 dB, 10 Hz spread, 3 ms delay, against the same files at 3 Hz (2 ms). At 3 Hz every signal is found, 12 to 18 of 21 sync tones are read right, and the standard decode succeeds with 1 to 14 hard errors. At 10 Hz, 20 of 24 signals are found (search score 1.7 to 5.7, minimum 1.5) and the fitted frequency is within 3 Hz, but only **2 to 14 of 21 sync tones are read right**, and the standard decode never succeeds. The loss is in reading the tones, not in finding the signal.
- Why the tones are misread: at 10 Hz spread (σ = 5 Hz) a tone's power spreads onto the tones 6.25 Hz either side, each getting about 0.46 of the centre's average power (exp(−6.25² ÷ (2 × 5²))). In any one symbol the strongest of the three is then the right one only about half the time, at any signal level. That matches the measured sync counts. Stronger signals cannot help, which fits the 0 of 40 at 0 dB.
- **Why FT8 still decodes, likely:** FT8 maps bits to tones with a Gray code [S1, page 9; S15, `graymap`], so a tone misread as its neighbour flips 1 bit. JS8 maps the 3 bits straight to the tone number (`legacy_encode`, `src\decoder\legacy_decoder.cpp`), so a neighbour misread flips 1 to 3 bits (tone 3 read as 4 flips all three). Not proven: the deciding test is the same audio with Gray-mapped test signals and a matching test decoder. The mapping is part of JS8's on-air format, so a fix has to come from the receiver.
- **Receiver-side fixes, researched 2026-11-19** [S42] [S43] [S44]: (1) WSJT-X's Q65 fast-fading decoder (Palermo and Taylor) scores each tone by the power in it and the bins around it, each weighted by t ÷ (1 + t) ÷ noise, where t is how much signal the spread puts in that bin; this is the textbook detector for a fading tone of known spread. Read from a description of the code (GPL, nothing copied). (2) Bit readings summed over all 8 tones instead of the strongest tone, which matters more without Gray coding. (3) Iterative demapping, about 1 dB on 8-tone noncoherent FSK with LDPC on Rayleigh fading [S43]; sources say it gains most with non-Gray mappings, but not from a direct LDPC test. Not built.
- **Fixes 1 and 2, built as a test and measured (2026-11-19)** [S45]: an extra try at the end of pass 7, only when every earlier pass failed. Weights come from the 21 sync tones (centre bin, the bins either side, and the rest as noise). Messages out of 100 signals per level:

  | Set | 0 dB | -4 dB | -8 dB | -12 dB |
  |---|---|---|---|---|
  | High-latitude moderate (10 Hz, 3 ms), now | 0 | 1 | 0 | 0 |
  | High-latitude moderate, with the new reading | 21 | 19 | 8 | 2 |
  | Low-latitude disturbed (10 Hz, 6 ms), now | 0 | 0 | 0 | 0 |
  | Low-latitude disturbed, with the new reading | 21 | 28 | 11 | 1 |

- High-latitude disturbed (30 Hz): still 0 of 400. Wrong messages: 0 everywhere. Wrong-message chances: +32 on these sets, +5 on the seven Normal-speed sets, noise unchanged (22).
- Other sets: the -15 to -22 dB fading sets gained 20 messages (3,315 → 3,335); the seven Normal-speed sets gained 14 (2,966 → 2,980). Nothing lost. Rescaling the new readings like the others (setting 2) gained less: 68 at 10 Hz instead of 111.
- Still far from FT8's 50% at -8.6 dB. The sync count was named as the likely next loss; that was wrong (next item). Time cost not measured.
- **Correction and follow-up (2026-11-19)** [S41] [S46]: the sync count is not the main loss. In the stage check it dropped 3 of 24 signals at 10 Hz, 0 dB; 4 had no candidate within 3 Hz; 17 reached the decode passes. Each possible cause in the decode stage was then switched on alone (both 10 Hz sets, 800 signals):

  | Setting | Messages | Wrong |
  |---|---|---|
  | New reading (as above) | 111 | 0 |
  | Plus accept on CRC alone (no 39/60 disagreement limit) | 111 | 0 |
  | Plus 150 rounds, no early give-up | 121 | 0 |
  | Plus last try (ordered statistics) on the new readings | **255** | **1** |

- With the last try on the new readings, out of 100 per level: high-latitude moderate 41 / 47 / 28 / 7 at 0 / -4 / -8 / -12 dB; low-latitude disturbed 51 / 46 / 29 / 6. FT8's threshold (50%) is -8.6 dB; ours is now between -4 and -8 dB. 30 Hz still 0.
- The test's last try had none of the existing limits (sync count, discrepancy, runner-up gap), so its wrong-message chances were not counted; the 1 wrong message came from that. A built version would put the new readings through the same limits as passes 8 to 10 and be measured for chances.
- **Built 2026-11-19** [S47] (`src\decoder\legacy_decoder.cpp`, `kSpreadMinShare`, `kSpreadMinSync`): the spread reading is used only when the sync tones show spread (neighbour signal at least 0.2 of the centre's). Then pass 7 also runs the standard decode on it, and a new pass 11 runs the last try on it, needing at least 9 sync tones read right by the spread reading and passing the same discrepancy and runner-up limits as passes 8 to 10. Normal and Deep.
- Picking the two minimums (10 Hz sets `itu_hm_s`, `itu_ld_s`, 800 signals, plus `ffade`, `crowd60`, `noise4`): with no spread minimum, slow fading gained 58 messages but chances on the crowded set doubled (518 → 1,124); at 0.2, 10 Hz went 111 → 250 for 1 more noise chance; 0.4 lost 10 Hz messages (170); sync 7 added chances, sync 11 lost messages.
- **Checked on fresh 10 Hz sets** (`itu_hm_f`, `itu_ld_f`, seeds 81 and 82): 101 → 253 messages, 0 wrong.
- All seven Normal-speed sets: Normal 2,966 / 2,867 → 2,966 / 2,886 (same messages, +0.7% chances); Deep 3,108 / 4,425 → 3,108 / 4,443; wrong messages unchanged (0 Normal, 1 Deep). Noise: 22 → 23 chances. The -15 to -22 dB 10 Hz sets (`itu_ld`, `itu_hm`) gain chances for no messages (92 → 161 and 114 → 167), as no decoder reaches those levels at 10 Hz.
- Time on the crowded set (60 signals per file): 0.470 → 0.474 s per file (+0.9%). A first version that ran the pass-7 decode on every failed candidate was 7% slower (0.480 → 0.515 s), over the 2-times-original limit, and gained 14 slow-fading messages; it was not kept.
- Project tests: 19 of 19 pass. 30 Hz still decodes nothing.
- **Decision (Rob, 2026-11-19): it stays in Normal as well as Deep.** Reason: on ordinary audio it adds about 0.005 expected wrong messages per full test run (+0.7% chances, wrong messages unchanged), against 150% more messages on 10 Hz fading, where Normal otherwise decodes almost nothing.
- **Short-test caution:** the standard asks for 3,000 ÷ spread seconds per level [S7]. These sets have 50 files (750 s) per level, enough for 4 Hz and up; the 0.1 Hz to 1.5 Hz results are short tests.

### The fast-fading fix at Fast and Turbo, measured (2026-11-20) [S48]

The fix (5ad9bbc) has no speed check, so it runs at Fast and Turbo too. It had not been measured there. Before = a962669, after = 5ad9bbc.

On the existing Fast and Turbo sets (flat, fading, three crowded sets, noise), messages / wrong messages / wrong-message chances:

| Speed and mode | Before | After |
|---|---|---|
| Fast, Normal | 18,489 / 4 / 12,508 | 18,489 / 4 / 12,646 (+1.1%) |
| Fast, Deep | 18,885 / 5 / 19,954 | 18,885 / 5 / 20,091 (+0.7%) |
| Turbo, Normal | 9,545 / 3 / 11,043 | 9,545 / 3 / 11,089 (+0.4%) |
| Turbo, Deep | 9,873 / 2 / 16,894 | 9,873 / 2 / 16,938 (+0.3%) |

- Noise sets: chances unchanged (Fast 10 Normal, 23 Deep; Turbo 0).
- Time on the crowded sets (`bcrowd`, `ccrowd`, 80 files each): Fast 0.335 → 0.341 s per file (+1.8%, 1.53 times the original); Turbo 0.184 → 0.187 s (+1.6%, 1.43 times the original). Limit is 2 times. The wrong messages there (2 Fast, 1 Turbo) are the same in both builds.

New fast-fading sets, 100 signals per level, Normal mode, messages before → after (wrong: 0 everywhere):

| Set | 0 dB | -4 dB | -8 dB | -12 dB | Chances |
|---|---|---|---|---|---|
| Fast, high-latitude moderate (10 Hz, 3 ms) | 28 → 64 | 24 → 52 | 11 → 42 | 2 → 12 | 150 → 177 |
| Fast, low-latitude disturbed (10 Hz, 6 ms) | 29 → 66 | 27 → 58 | 15 → 50 | 3 → 9 | 119 → 155 |
| Fast, high-latitude disturbed (30 Hz, 7 ms) | 0 → 0 | 0 → 0 | 0 → 0 | 0 → 0 | 11 → 15 |
| Turbo, high-latitude moderate (10 Hz, 3 ms) | 97 → 97 | 93 → 93 | 70 → 70 | 11 → 11 | 130 → 143 |
| Turbo, low-latitude disturbed (10 Hz, 6 ms) | 99 → 99 | 91 → 91 | 71 → 71 | 9 → 9 | 99 → 107 |
| Turbo, high-latitude disturbed (30 Hz, 7 ms) | 0 → 12 | 0 → 13 | 0 → 1 | 0 → 0 | 43 → 84 |

- **Fast: the fix more than doubles 10 Hz messages** (139 → 353 of 800). Fast decodes 10 Hz before the fix (unlike Normal) because its tones are 10 Hz apart, not 6.25 Hz.
- **Turbo: no gain at 10 Hz** (tones 20 Hz apart; it already reads them), but **30 Hz decodes for the first time anywhere**: 26 of 400. Fast and Normal still decode nothing at 30 Hz.
- Decision: the fix stays at every speed. It costs 0.3% to 1.8% more chances and time on ordinary audio, and gains at Fast (10 Hz) and Turbo (30 Hz). No code change.
- Short-test caution as above: 25 files per level.

### Iterative demapping: research (2026-11-20) [S4] [S43] [S44]

Iterative demapping redoes the tone reading after each decoder round. Each tone is weighted by the decoder's current belief in the other bits of that tone's label.

- **The formula, the same in all three sources:** a bit's reading = log of (sum over the tones whose label has that bit = 1 of tone likelihood × belief in the tone's other bits) ÷ (the same sum over tones with the bit = 0). The bit's own belief is left out. With all beliefs equal, this is the non-iterated reading. [S4, equations 4 to 8; S44, equation 3; S43 (2017), equations 27 and 30]
- **Gain, Valenti and Cheng (turbo code, 4, 16 and 64 tones):** 0.34 to 0.43 dB with 4 tones, 0.69 to 0.86 dB with 16, 0.75 to 1.04 dB with 64. The gain is bigger on Rayleigh fading than without fading. [S4, pages 1745 to 1746, Table I]
- **Gain, Ferrett and Valenti (LDPC code, DVB-S2 rate 1/2, Rayleigh fading):** **1 dB with 8 tones**, 0.9 dB with 4. This is the closest published case to JS8 (8 tones, LDPC code). [S43 (2017), section IV and Fig. 4] Their 2018 paper uses natural mapping (tone number = the bits, as in JS8) with iteration, but has no with-and-without comparison. [S43 (2018), section II-A]
- **Schedule:** redo the reading after every decoder round. Several decoder rounds per reading gave no gain [S4, page 1741]. After 4 rounds, the iterated version beat the non-iterated one after 30 [S4, Fig. 4]. Ferrett and Valenti also used one decoder round per reading [S43 (2017), section IV].
- **The reading must stay calibrated:** Guillén i Fàbregas and Grant's simpler reading that needs no signal level loses 0.4 to 0.6 dB without fading and 1.4 to 2.0 dB on Rayleigh fading [S44, Table I]. JS8Core's spread reading is already calibrated from the sync tones (section 5), so it is the right base.
- **Not found in any source:** a test with power leaking into neighbouring tones, as at 10 Hz spread. In the papers' signals, any wrong tone is as likely as any other, so mapping cannot matter there. At 10 Hz, with JS8's mapping, mistaking a tone for its neighbour can flip 1 to 3 bits. Feedback could help more there or less; only measurement will tell.
- **What this predicts for JS8:** about 1 dB on fading, from the closest published case. FT8 is ahead of us by 1 to 4 dB at 10 Hz (50% at -8.6 dB; ours between -4 and -8 dB).
- **How it fits JS8Core:** the spread reading (`build_spread` in `src\decoder\legacy_decoder.cpp`) already scores all 8 tones per symbol and sums over them. Iterating needs the decoder (`belief_propagation` in `src\decoder\ldpc.cpp`) to rebuild those bit readings each round from the tone scores and its own messages.

### Iterative demapping: built and measured (2026-11-20) [S49]

Test copy settable by `JS8_ID` (`scorecard\.patch_id.py`): 1 = pass 7's spread decode iterated (signals showing spread only); 2 = also an iterated try on every other candidate that failed pass 7; 3 = as 2, with pass 6's 2-tone decode removed; 4 = as 2, with pass 7's plain 3-tone decode removed. One reading per decoder round, as the sources do; up to 60 rounds, same early give-up as now.

Normal speed, Normal mode. Seven ordinary sets (messages / wrong-message chances), 10 Hz development sets (`itu_hm_s`, `itu_ld_s`) and crowded time (`crowd60`; the original took 0.257 s per file):

| Setting | Seven sets | 10 Hz development sets | Crowded time per file |
|---|---|---|---|
| Off (5ad9bbc) | 2,966 / 2,886 | 247 | 0.488 s |
| 1 | 2,966 / 2,886 | 279 | not run |
| 2 | 3,132 / 2,817 | 285 | 0.517 s (+6%) |
| 2, sync 10 or more | not run | not run | 0.515 s, 346 of 349 crowded messages |
| 2, sync 12 or more | not run | not run | 0.504 s, 341 of 349 |
| 2, 30 rounds | not run | not run | 0.510 s, 346 of 349 |
| **3** | **3,128 / 2,821** | 286 | **0.501 s (+2.2%)** |
| 4 | 3,123 / 2,844 | not run | 0.500 s, 343 of 349 |

- Wrong messages: 0 in every row. Noise files: 23 chances in every row.
- **Iterating gains on ordinary audio too, not only on spread signals** (setting 2: +166 messages). The papers predicted about 1 dB on fading with 8 tones [S43]; the gain here is largest on the fading sets (`ffade` 282 → 299, `ffade2` 570 → 601).
- **Wrong-message chances fall** (2,886 → 2,821): signals the risky last try used to catch now decode in the safer standard decode. Setting 2's decode succeeded on 741 candidates across sync counts 7 to 18, for 166 new messages.
- A sync minimum or a round cap saves little time and loses messages. The time is the extra decode run, not the reading math: computing each tone weight once per symbol (8 exp() calls, not 24; same result) changed 0.523 → 0.520 s.
- **Pass 6's 2-tone decode adds almost nothing once iterating is in** (setting 3 loses 4 of 3,132 messages), and dropping it pays for most of the time.
- **Built as setting 3** (`src\decoder\ldpc.cpp`, `belief_propagation` with `tone_scores`; `src\decoder\legacy_decoder.cpp`, passes 6 and 7). The 2-tone reading is now built only when pass 9 (Deep) needs it, so Normal does less work. Built version, same sets: identical numbers to setting 3 (3,128 / 2,821; Deep 3,241 / 4,365; 10 Hz 587 / 705).

Built version against the decoder before (5ad9bbc), messages / wrong / chances:

| Sets | Before | After |
|---|---|---|
| Seven Normal-speed sets, Normal | 2,966 / 0 / 2,886 | 3,128 / 0 / 2,821 |
| Seven Normal-speed sets, Deep | 3,108 / 1 / 4,443 | 3,241 / 1 / 4,365 |
| 10 Hz sets (development and fresh), Normal | 500 / 0 / 702 | 587 / 0 / 705 |
| Fresh 10 Hz sets only (`itu_hm_f`, `itu_ld_f`) | 253 | 301 (+19%) |
| Other ITU conditions, -15 to -22 dB (8 sets) | 3,315 / 0 / 2,477 | 3,423 / 0 / 2,443 |
| Fast, six sets | 18,489 / 4 / 12,646 | 19,576 / 4 / 12,187 |
| Fast, 10 Hz and 30 Hz sets | 353 / 0 / 347 | 465 / 0 / 279 |
| Turbo, six sets | 9,545 / 3 / 11,089 | 9,979 / 3 / 10,951 |
| Turbo, 10 Hz and 30 Hz sets | 567 / 0 / 334 | 585 / 0 / 318 |

- Fast and Turbo figures are from the test copy at setting 3; at Normal speed the built version matched setting 3 exactly.
- Other ITU conditions, per condition: mid-latitude quiet 726 → 748, low-latitude quiet 666 → 682, mid-latitude moderate 682 → 700, near vertical 638 → 665, low-latitude moderate 603 → 627. 10 Hz at -15 to -22 dB still about 0 (no decoder reaches those levels there).
- Fresh 10 Hz sets at -8 dB: high-latitude moderate 28 → 42 of 100, low-latitude disturbed 30 → 36. FT8's 50% point is -8.6 dB; ours is now about -4 to -5.3 dB at 10 Hz (straight line between the measured levels: high-latitude moderate 54% at -4 dB, 42% at -8 dB; low-latitude disturbed 50% at -4 dB), so the gap is about 3.3 to 4.6 dB. 30 Hz: still 0 at Normal and Fast; Turbo 26 → 29 of 400.
- Crowded time per file: Normal speed 0.500 → 0.500 s (1.89 times the original); Fast 0.360 → 0.367 s (1.56 times); Turbo 0.197 → 0.200 s (1.48 times). Limit 2 times.
- Project tests: 19 of 19 pass.
- Not tried: feeding the last try (passes 8 to 11) the readings the iterated decode ends with. Tried the next day, as below.
- Not open to us: Ferrett and Valenti also redesign the LDPC code for iterating, worth up to 1.1 dB [S43, 2018]. JS8's code is part of its on-air format.

### The last try with iterative demapping (2026-11-20) [S33] [S34] [S50]

**Research.** No source combines the last try (ordered statistics decoding) with iterative demapping (search, 2026-11-20). The earlier sources set the rules: order the search by a few short, damped decoder rounds, not by the decoder's final output. Fossorier found searching after the full decode gave "negligible improvement" [S33, Fig. 8]; Zhang et al. found 30 rounds worse than 3 [S34]. Score candidates against the received reading, so the discrepancy and runner-up limits keep their meaning [S33]. So the test changes only the ordering: pass 11's 3 ordering rounds also redo the tone reading.

**Test settings** (`JS8_LT`, `scorecard\.patch_lt.py`): 1 = pass 11's ordering rounds redo the tone reading; 2 = 1, with pass 11 open to every candidate (not only signals showing spread; the 9-sync-tone minimum kept); 3 = 2 with the ordering as now.

Normal speed, 10 Hz sets (`itu_hm_s`, `itu_ld_s`, `itu_hm_f`, `itu_ld_f`, 1,600 signals) and noise (`noise4`), messages / wrong-message chances / noise chances / wrong:

| Setting | Normal mode | Deep mode |
|---|---|---|
| 0 (now) | 587 / 705 / 23 / 0 | 587 / 1,133 / 43 / 0 |
| 1 | 596 / 770 / 22 / 0 | 596 / 1,197 / 42 / 0 |
| 2 | 599 / 904 / 46 / 1 | not run |
| 3 | 589 / 827 / 44 / 0 | not run |

- **Opening pass 11 to every candidate (settings 2 and 3) is rejected:** chances on noise doubled (23 → 44 or 46) and a wrong message got through, for 2 to 12 more messages.
- **Setting 1** gains on both the development sets (286 → 290) and the fresh sets (301 → 306), at +9% chances in Normal and +5.6% in Deep on these sets. On the seven ordinary Normal-speed sets in Deep: messages unchanged (3,241), chances 4,365 → 4,369.
- Fast, Deep (10 Hz, 30 Hz and `bfade` sets): 1,821 → 1,823 messages, chances 1,388 → 1,408, wrong 2 → 2. Turbo, Deep (same kind of sets): 1,749 → 1,749, chances 1,596 → 1,607. At Turbo (tones 20 Hz apart) the reading is rarely wrong, so it gains nothing.
- **Built, Deep only** (it raises wrong-message chances, so by Rob's rule it goes behind Deep), and only where tones are 10 Hz apart or closer (`kSpreadDemapMinSymbolSamples`, `src\decoder\legacy_decoder.cpp`; `refine_reliabilities` with `tone_scores`, `src\decoder\ldpc.cpp`). Slow speed (closer tones) is included; measured in "Slow speed, measured" below.
- Built version, same checks: identical numbers to the test (Normal 587 / 705; Deep 10 Hz 596 / 1,197; Deep ordinary 3,241 / 4,369; Normal ordinary 3,128 / 2,821; Fast Deep 1,823 / 1,408; Turbo Deep 1,749 / 1,596). Deep crowded time 0.846 → 0.856 s per file (+1.2%). Project tests: 19 of 19 pass.
- A small gain: 9 of 1,600 10 Hz signals in Deep. The 10 Hz gap to FT8 stays about 3.3 to 4.6 dB.

### The FT8 benchmark re-read, and a wide spread reading (2026-11-20) [S1] [S42] [S51]

**The benchmark, re-read from the paper's text.** FT8's -8.6 dB on high-latitude moderate (10 Hz, 3 ms) is **without** callsign help ("no AP"). It uses block detection with the standard decode and the last try (BP+OSD), and "threshold" means 50% decode probability, signal in 2500 Hz [S1, Tables 5 and 6, pages 11 and 14]. With the standard decode alone, FT8 never reaches 50% at 10 Hz [S1, Table 5]. Block detection gives little on this channel, because coherence time is close to the block length [S1, page 12]. **The paper gives no 30 Hz result**, so there is no FT8 benchmark there.

**Why 30 Hz is different.** At 30 Hz spread (σ = 15 Hz) and 6.25 Hz tone spacing, a tone's power falls to 0.92, 0.71, 0.46 and 0.25 of the centre at 1, 2, 3 and 4 tones away (exp(−d² ÷ (2σ²)), d in Hz). The spread reading looked only 1 tone away. The Q65 fast-fading method weights every bin the spread covers [S42]. But the decoder had no audio beyond 1 tone outside tones 0 to 7: `extract_channel` cuts each signal out from 1.5 tones below to 1.5 tones above, tapered at the edges.

**Test** (`scorecard\.patch_wide.py`): cut the signal out again 4 tones wider each side, with the same time and frequency fine-tuning. Score each tone from bins up to K tones away, weighted t ÷ (1 + t) ÷ noise, with t measured on the 21 sync tones and the noise taken from bins at least K + 2 tones away. This replaces the spread reading in pass 7 (iterative decode) and pass 11.

- **The old neighbour-share measure cannot spot 30 Hz signals:** 69% of 30 Hz readings showed under 0.1, because spread power inflates its noise estimate. So the gate uses the wide cut-out's own share, with the clean noise estimate.
- **30 Hz: still 0 of 400**, with K = 2, 3 or 4 and the wide reading always on. Not explained; the next item.
- **10 Hz: large gains.** Always on, high-latitude moderate (`itu_hm_s`) 134 → 194 with K = 2 (189 with K = 4). K = 1 on the wide cut-out gives 124, below today's 134, so **the gain comes from weighting bins 2 tones away**, not from the cleaner cut-out (my first guess, wrong). Why bins that far matter at 10 Hz is not proven.
- Always on loses on ordinary signals (seven Normal-speed sets 3,128 → 3,067 with K = 2), so it needs a gate. Wide share per reading: 10 Hz sets 27% to 28% under 0.1; fading set `ffade` 79%; crowded set 91%; noise 76%.

Gate on the wide share, K = 2 (messages / wrong-message chances):

| Gate | Seven sets | 10 Hz sets (four) | Noise chances |
|---|---|---|---|
| Off (6cadbfb) | 3,128 / 2,821 | 587 / 705 | 23 |
| 0.1 | 3,121 / 2,814 | 779 / 621 | 23 |
| **0.2** | **3,128 / 2,819** | **778 / 621** | 24 |
| 0.3 | 3,128 / 2,822 | 762 / 636 | 23 |

- Wrong messages: 0 in every row.
- **At Fast speed (tones 10 Hz apart) it loses:** 10 Hz and 30 Hz sets 465 → 401, ordinary sets 19,576 → 19,575. So it is built for tones 6.25 Hz apart or closer only (Normal and Slow; Slow measured below). Turbo's FFT has no room for the wider cut-out.
- **One wrong message appeared** on the -15 to -22 dB ITU sets (`itu_ld\S-15_8.wav`), from pass 11 (10 sync tones, discrepancy 3.7% of the evidence). It is the known 1 in 4,096 chance per last-try result: those sets had about 2,440 chances (0.6 wrong messages expected), and the wide reading lowered them (2,443 → 2,438).

**Built** (`extract_wide_energy`, `kWideMinSymbolSamples`, `kWideExtraTones`, `kWideReach`, `kWideMinShare` in `src\decoder\legacy_decoder.cpp`), gate 0.2, K = 2. Against 6cadbfb, messages / wrong / chances:

| Sets | Before | After |
|---|---|---|
| Seven Normal-speed sets, Normal | 3,128 / 0 / 2,821 | 3,128 / 0 / 2,819 |
| Seven Normal-speed sets, Deep | 3,241 / 1 / 4,369 | 3,241 / 1 / 4,368 |
| 10 Hz sets (four), Normal | 587 / 0 / 705 | **787 / 0 / 621** |
| 10 Hz sets (six, with the fresh `itu_hm_g`, `itu_ld_g`), Deep | 866 / 1 / 1,769 | **1,153 / 1 / 1,509** |
| Fresh 10 Hz sets only (`itu_hm_g`, `itu_ld_g`, seeds 85, 86, made after the gate was picked), Normal | 260 | **358 (+38%)** |
| Other ITU conditions, -15 to -22 dB (8 sets) | 3,423 / 0 / 2,443 | 3,427 / 1 / 2,438 |
| Fast, 10 Hz and 30 Hz sets | 465 / 0 / 279 | 465 / 0 / 279 (unchanged, as designed) |

- Fresh sets per level, high-latitude moderate: 0 dB 49 → 57, -4 dB 40 → 53, **-8 dB 39 → 50**, -12 dB 11 → 25 of 100. **FT8's 50% point is -8.6 dB; ours is now about -8 dB on this set.** Low-latitude disturbed: 50 → 58, 39 → 53, 24 → 38, 8 → 24.
- Time, crowded set: Normal 0.510 → 0.513 s per file (+0.6%, 1.91 times the original); Deep 0.840 → 0.845 s (+0.6%).
- Project tests: 19 of 19 pass.
- **Why 30 Hz still decodes nothing (stage check, 2026-11-20)** [S41]: high-latitude disturbed (30 Hz, 7 ms), 0 dB, 100 signals, decoder 07bdf6f. **The signals are mostly not found.** For 48 of 100 the nearest candidate is more than 12 Hz away; 17 are 6 to 12 Hz away, 17 are 3 to 6 Hz away, 18 within 3 Hz. Of the 18, 12 have 6 or fewer of the 21 sync tones read right and are dropped before any decode pass (`if (sync_matches <= 6)` in `attempt_decode`). At 10 Hz the stage check found 20 of 24 signals; at 30 Hz the candidate search, which scores the sync pattern in single bins, is the first loss. A fix belongs in the candidate search, which every signal goes through.
- **Researched 2026-11-20: no published method found** for finding signals whose spread is several tones wide. The Q65 decoder's candidate search (Rust port of WSJT-X's) scores single bins, with no spread handling; its fast-fading method acts only after a signal is found [S52]. Two literature searches found nothing on combining bins for sync under heavy spread. **The sources handle heavy spread with wider tone spacing at the transmitter:** FT4 beats FT8 on fast fading for that reason [S1, page 14], and Q65 has wide-spacing submodes for severe spread [S53]. In JS8 that is Turbo (20 Hz spacing), which decodes 26 to 29 of 400 at 30 Hz (section 5). A receiver-side search fix at Normal speed would be our own design, with no source behind it.
- **The built version first differed from the test** (10 Hz: 787 against 778). Cause, found and checked: the built code adds the two neighbour bins in double precision, the test copy in single precision. With the test copy changed to double, it gives exactly 787 / 621. So rounding alone moves about 9 of 1,600 borderline 10 Hz signals; differences that small on these sets are noise.

### Slow speed, measured (2026-11-19) [S54]

Today's three changes (4a8ec2a iterative demapping, 6cadbfb last try with demapping, 07bdf6f wide reading) all run at Slow (tones 3.125 Hz apart) and had not been measured there. Before = 5ad9bbc, after = 847f8aa. New Slow sets, made with the generator's new `speed=slow` option:

| Set | Fading | Signals | Levels |
|---|---|---|---|
| `eflat` | none | 200 | -18 to -25 dB, 25 per level |
| `efade` | 1 Hz, 2 ms (mid-latitude disturbed) | 200 | -18 to -25 dB |
| `emm` | 0.5 Hz, 1 ms (mid-latitude moderate) | 200 | -18 to -25 dB |
| `ehm` | 10 Hz, 3 ms (high-latitude moderate) | 100 | 0, -4, -8, -12 dB |
| `ecrowd` | none, 60 signals per file | 1,800 (30 files) | -21 to -23 dB |
| `enoise` | noise only | 100 files | — |

Messages / wrong-message chances, before → after:

| Set | Normal | Deep |
|---|---|---|
| `eflat` | 536 / 164 → 548 / 162 | 544 / 268 → 553 / 267 |
| `efade` | 230 / 261 → 244 / 258 | 235 / 412 → 248 / 409 |
| `emm` | 311 / 229 → 329 / 216 | 319 / 398 → 333 / 386 |
| `ehm` | 0 / 39 → 0 / 31 | 0 / 63 → 0 / 63 |
| `ecrowd` | 1,139 / 388 → 1,173 / 372 | 1,154 / 578 → 1,184 / 571 |
| `enoise` (chances only) | 14 → 11 | 21 → 18 |
| **All** | **2,216 / 1,081 → 2,294 / 1,039** | **2,252 / 1,719 → 2,318 / 1,696** |

- **No losses; gains on every set that decodes:** +78 messages in Normal (+3.5%), +66 in Deep (+2.9%). Wrong messages: 0 in every set, both builds.
- Wrong-message chances fall in both modes (Normal 1,081 → 1,039, Deep 1,719 → 1,696), and on noise (14 → 11, 21 → 18).
- Time per file, before → after: crowded set Normal 1.012 → 1.037 s (+2.5%), Deep 1.356 → 1.320 s (−2.7%); fading set Normal 0.146 → 0.149 s, Deep 0.190 → 0.187 s. Slow has a 30 s cycle, so this is far inside its time.
- **10 Hz at Slow decodes nothing in either build** (`ehm`, 0 of 100 even at 0 dB). 10 Hz spread is 3.2 tone spacings at Slow (3.125 Hz apart), against 1.6 for 10 Hz at Normal speed (which now decodes) and 4.8 for 30 Hz at Normal speed (which does not). Cause found the same day, below.
- Short-test caution: 25 files per level.

**Why 10 Hz decodes nothing at Slow (stage check, 2026-11-19)** [S55]. `ehm`, 0 dB, 100 signals, decoder 847f8aa:

- **Unlike 30 Hz at Normal speed, the signals are found:** 95 of 100 have a candidate within 3 Hz (66 within 1.5 Hz).
- The time and frequency fit is worse than without fading. Frequency within 0.5 Hz: 20 of 95 (no-fading Slow set at -18 dB: 100 of 100). Time more than 0.5 s off: 20 of 95 (no fading: 0). Sync tones read right: 3 to 11 of 21, median 7.
- **The fit is not the cause, and neither is the gate.** Test copy given each signal's true frequency and time, with the 6-sync-tone drop (`if (sync_matches <= 6)` in `attempt_decode`) switched off: still 0 of 100, Normal and Deep. At the true frequency and time, the plain reading gets 5 to 9 of 21 sync tones right.
- **The cause is the wide reading's reach** (`kWideReach = 2`, `src\decoder\legacy_decoder.cpp`): it scores each tone from bins up to 2 tones away. At Slow, 2 tones is 6.25 Hz; 10 Hz spread reaches further.

Reach test, `ehm` 0 dB, 100 signals:

| Reach (tones) | As found | True frequency and time, no 6-sync-tone drop |
|---|---|---|
| 2 (now) | 0 | 0 |
| 3 | 2 | 10 |
| 4 | 10 | 31 |

All six Slow sets, messages / wrong-message chances (extra tones each side = reach + 2 for reach 4 to 6, so the noise bins stay at least reach + 2 away):

| Reach | Normal: all sets | Normal: `ehm` | Deep: all sets | Deep: `ehm` | Noise chances, Normal / Deep |
|---|---|---|---|---|---|
| 2 (now) | 2,294 / 1,039 | 0 / 31 | 2,318 / 1,696 | 0 / 63 | 11 / 18 |
| **4** | **2,322 / 1,072** | **28 / 60** | **2,348 / 1,732** | **30 / 97** | 11 / 18 |
| 5 | 2,322 / 1,071 | 28 / 63 | 2,346 / 1,732 | 28 / 94 | 13 / 19 |
| 6 | 2,325 / 1,064 | 31 / 58 | 2,349 / 1,736 | 31 / 103 | 12 / 19 |

- Wrong messages: 0 in every row. The five other sets have the same messages at every reach. Their chances move by 2 to 11 either way.
- **Reach 4 at Slow looks the same distance in Hz as reach 2 at Normal speed: 12.5 Hz each side.** Reach 5 and 6 gain no more.
- **Fresh sets, made after reach 4 was picked** (`/tmp` in the test container only, seeds 121 and 122, Slow, 25 files per level, 0 to -12 dB). Reach 2 → 4, Normal: high-latitude moderate (10 Hz, 3 ms) 0 → 29 of 400, low-latitude disturbed (10 Hz, 6 ms) 0 → 22 of 400; Deep 0 → 30 and 0 → 22. 0 wrong messages. Noise chances unchanged (11 Normal, 18 Deep). Per level, high-latitude moderate: 0 dB 11, -4 dB 11, -8 dB 6, -12 dB 1 of 100. Low-latitude disturbed: 9, 5, 7, 1.
- Time, crowded Slow set: 1.025 → 1.021 s per file.
- **What is still lost:** with the true frequency and time **and the 6-sync-tone drop switched off**, reach 4 decodes 31 of 100 at 0 dB, against 10 as found. ~~So the time and frequency fit costs about two thirds of what is left.~~ Corrected 2026-11-20: that test removed the fit and the drop together. Split apart, neither alone helps; see "Fit smoothing and weak-sync rescue" below.
- ~~Not built: reach is a fixed constant now (`kWideReach`), shared by Normal and Slow.~~ At Normal speed, reach 4 was measured worse than reach 2 (189 against 194 on `itu_hm_s`, above).
- **Built 2026-11-20, Slow only, Normal and Deep** (`kWideReach<Mode>`, `kWideExtraTones<Mode>`, `kWideLongSymbolSamples` in `src\decoder\legacy_decoder.cpp`): reach 4 and 6 extra tones for 3,840-sample tones (Slow), reach 2 and 4 extra tones otherwise (Normal, as before). Built version, checked against the test copy and the decoder before:

| Sets | Expected | Built |
|---|---|---|
| Six Slow sets, Normal | 2,322 / 1,072 (reach-4 test) | 2,322 / 0 / 1,072 |
| Six Slow sets, Deep | 2,348 / 1,732 (reach-4 test) | 2,348 / 0 / 1,732 |
| Seven Normal-speed sets, Normal | 3,128 / 2,819 (847f8aa) | 3,128 / 0 / 2,819 |
| Seven Normal-speed sets, Deep | 3,241 / 4,368 (847f8aa) | 3,241 / 1 / 4,368 (the same wrong message as before) |
| Four Normal-speed 10 Hz sets, Normal | 787 / 621 (847f8aa) | 787 / 0 / 621 |

  Messages / wrong / chances. Identical everywhere, so Normal speed is unchanged and Slow matches the test. Crowded Slow time as in the test (1.025 → 1.021 s per file). Project tests: 19 of 19 pass.

**Research on the reach, 2026-11-20** [S42] [S56]. How the Q65 fast-fading method sets how many bins it weights around each tone:

- **The width comes from spread × symbol time, not from a fixed number of tones.** WSJT-X `q65.c` takes `B90Ts` ("spread bandwidth (90% fractional energy)" times a symbol period). It divides by the QRA64 symbol time (`B90 = B90Ts/TS_QRA64`, 0.576 s), takes a table index `hidx = (int)(logf(B90)/logf(1.09f) - 0.499f)` (64 windows, each 1.09 times wider) and weights `hlen - 1` bins each side. The tables were made for 0.576 s symbols and "continue to be valid if the B90Ts parameter is properly scaled to the QRA64 symbol interval". The tap weights are a fixed Gaussian or Lorentzian shape; ours are measured on the sync tones.
- **What that predicts for JS8, my arithmetic:** taking the scaling to keep spread × symbol time the same, with a Gaussian spectrum (90% energy width = 3.29 σ = 16.4 Hz for our 10 Hz, 2σ, spread), it gives B90 × Ts = 2.63 at Normal (0.16 s symbols) and 5.26 at Slow (0.32 s). Table index 17 → 2 bins each side at Normal (the same as our reach 2), index 25 → 3 bins at Slow. So the source supports a reach that grows with symbol time. It puts Slow at 3; the measured best is 4 (2 of 100 at reach 3 against 10 at reach 4, as found). The difference may come from the weights (fixed tapered shape there, measured here). That is not checked.
- ~~Not found: how WSJT-X picks `B90Ts`.~~ Found the same day, below.

**How WSJT-X's Q65 decoder handles spread, timing and frequency, read in its code (2026-11-20)** [S57]:

- **Spread: it tries several widths in turn and keeps the first decode that passes the CRC.** `lib/qra/q65/q65.f90`: `b90=1.72**ibw`, `b90ts=b90/baud`, then a decode at each `ibw` from `ibwa` to `ibwb`. `b90ts = b90/baud` is B90 times the symbol time of the current speed, which confirms the "keep spread × symbol time the same" reading above. Range, from `lib/q65_decode.f90`: Q65-60A `ibwa=1`, `ibwb=min(15,ibwa+6)` (B90 = 1.7 to 51 Hz); Deep widens it by 2 at each end (`ibwa=max(1,ibwa-2)`, `ibwb=min(15,ibwb+2)`).
- **Fit: it retries small shifts around the sync result, each with a full decode.** `lib/qra/q65/q65_loops.f90`: frequency shifts in half-tone steps (`a(1)=-(f0+0.5*baud*ndf)`), time shifts in 1/16-symbol steps (`jpk=jpk0 + nsps2*ndt/16`), and spread steps, with ndf² + ndt² + (spread step)² at most 4 or 5 (`maxdist`). Normal depth: shifts −1 to +1 each way; Deep: −2 to +2. It stops at the first decode that passes the CRC.
- **Sync under spread: it smooths the spectrum across frequency before sync, by a fixed amount per submode.** `q65_symspec`: `nsmo=int(0.5*mode_q65*mode_q65)` passes of a 1-2-1 filter (`smo121`: 0.5 × centre + 0.25 × each neighbour). That is 0 passes for submode A, 2 for B, 8 for C, 32 for D, 128 for E: the wider the tone spacing, the more smoothing. It does not depend on the measured spread. The code's own comment: "For large Doppler spreads, should we smooth the spectra here?"
- **A structural difference from JS8:** Q65's sync symbols all use the same tone, so spread smears them in frequency but never onto another sync tone. JS8's Costas sync uses all 8 tones, so under spread a sync symbol's power lands on neighbouring tones, as measured above (5 to 9 of 21 sync tones read right even at the true frequency and time).
- The Q65 guide says it decodes signals with spread "up to ten times the tone spacing" [S58, search summary only].
- **Theory, not read in full:** the classic result for Doppler-spread channels is a receiver that filters, squares and integrates over the spread. With orthogonal FSK, the optimum receiver gains "implicit diversity" when the signal is spread [S59; abstracts and search summaries only]. This fits a reach that grows with spread × symbol time.
- **No published method found for a sync search that adapts to the spread.** Two literature searches found joint timing and frequency estimators for unfaded or slowly fading FSK, and nothing for heavy spread [S59].

What this means for the two open items, as options, none built:

1. **Reach at Slow:** sourced as a width set by spread × symbol time. WSJT-X tries several widths instead of measuring one. Our measured weights and fixed reach 4 is our own variant of the same idea.
2. **Fit loss at Slow 10 Hz** (31 of 100 with the true values, 10 as found). There are two sourced approaches:
   - WSJT-X's retry of half-tone and 1/16-symbol shifts, each with a full decode.
   - Smoothing across frequency before sync. For JS8, that would mean scoring the sync tones with the wide reading's weights, not single bins.

   Each retry is another decode, so another chance for a wrong message. That is not measured.

### Fit smoothing and weak-sync rescue, tested (2026-11-20) [S57] [S60]

Reach 4 was built for Slow (3fbf2c3, above). This tests the second sourced idea, smoothing before sync, on the fit.

**Where the remaining Slow 10 Hz loss is.** Test copy at reach 4, `ehm` 0 dB, 100 signals:

| Fit | 6-sync-tone drop | Decoded |
|---|---|---|
| As found | on (now) | 10 |
| True frequency | on | 11 |
| True frequency and time | on | 10 |
| As found | off | 10 |
| True frequency, time as found | off | 24 |
| True frequency and time | off | 31 |

- **Two losses that hide each other.** The drop (`if (sync_matches <= 6)` in `attempt_decode`) counts sync tones in single bins, which spread defeats; it drops signals whose fit is right. And the frequency fit is off on the signals that pass. Fixing one alone gains nothing. Frequency is most of the fit loss (24 of 31).

**Test copy** (`patch_fsmo.py`, `patch_fgate.py`, `patch_wsync.py`; Claude's scratchpad, not in `scorecard\`):

- **Fit smoothing** (`JS8_FSMO` = passes, `JS8_FGATE` = gate). WSJT-X's 1-2-1 filter applied across frequency to the fit's sync score. In JS8 terms, the score at each trial is a weighted sum of `alignment_score` with the whole sync pattern shifted by −n to +n tones, weights from n passes of 1-2-1.
- **Gate.** The plain fit runs first. The smoothed fit is redone only when the spread share at the fitted point, the score 1 tone either side over the score at it, is at least the gate.
- **Weak-sync rescue** (`JS8_WSYNC`). A candidate with 6 or fewer single-bin sync tones is kept if the spread reading reads at least this many of the 21 sync tones right.

Slow, all six sets plus the fresh `/tmp/ehm2`, `/tmp/eld2` (seeds 121, 122), Normal mode, messages / chances:

| Setting (passes / gate / rescue) | All | `ehm` | Fresh 10 Hz | Crowded | Noise chances |
|---|---|---|---|---|---|
| Off | 2,373 / 1,189 | 28 | 51 | 1,173 | 11 |
| Smoothing only, 1 / always / off | 2,278 (six sets) | 37 | not run | 1,138 | 8 |
| Smoothing only, 1 / 0.3 / off | 2,330 (six sets) | 37 | 54 | 1,170 | 10 |
| Rescue only, off / off / 7 | not run | 31 | not run | not run | 12 |
| 1 / 0.3 / 7 | 2,429 / 1,277 | 50 | 86 | 1,170 | 10 |
| **2 / 0.3 / 7** | **2,446 / 1,255** | **58** | **97** | 1,168 | 10 |
| 3 / 0.3 / 7 | 2,445 / 1,262 | 58 | 99 | 1,166 | 12 |
| **2 / 0.5 / 7** | **2,435 / 1,304** | 51 | 90 | 1,173 | 11 |
| 1 / 0.3 / 9 | identical to 1 / 0.3 / 7 | | | | |

- Wrong messages: 0 in every row.
- **Each part alone gains little; together they nearly double Slow 10 Hz.**
- **Brand-new sets** (seeds 131, 132, made after picking 2 / 0.3 / 7): 49 → 74 of 800 (+51%), 0 wrong. Chances on those sets 108 → 171. Noise chances 11 → 10.
- Crowded Slow time: 1.044 → 1.073 s per file (+2.8%) at 2 / 0.3 / 7.

**Normal speed** (the test copy applies at every speed):

| Setting | Seven ordinary sets | Six 10 Hz sets | Noise chances |
|---|---|---|---|
| Off | 3,128 / 2,819 | 1,145 / 961 | 24 |
| 2 / 0.3 / 7 | **3,117** / 2,770 | **1,392** / 862 | 19 |
| 2 / 0.5 / 7 | 3,128 / 2,837 | 1,274 / 967 | 22 |
| 1 / 0.3 / 7 | 3,121 / 2,786 | not run | 22 |
| Rescue only, off / off / 7 | 3,128 / 2,820 | not run | 24 |

- **Gate 0.3 loses 11 ordinary messages at Normal** (0.35%), from the smoothing, not the rescue. Gate 0.5 loses none.
- Fresh high-latitude moderate (`itu_hm_g`) per level, off → gate 0.5 → gate 0.3: 0 dB 57 → 63 → 66, -4 dB 53 → 60 → 66, **-8 dB 50 → 53 → 62**, -12 dB 25 → 25 → 24 of 100. FT8's 50% point is -8.6 dB.
- Crowded Normal time (`crowd60`): 0.479 → 0.486 s (+1.5%) at gate 0.5, 0.488 s (+1.9%) at gate 0.3.
- **Fast and Turbo** (flat, fading, crowded, 10 Hz, 30 Hz, noise), 2 / 0.5 / 7: +2 messages each, same wrong messages as before (3 and 1, the known ones), chances unchanged. No gain there.
- Not built. The choice is between gate 0.5 (no losses anywhere: Normal 10 Hz +11%, Slow +62) and gate 0.3 (Normal 10 Hz +22%, Slow +73, but 11 ordinary messages lost at Normal and 5 of 1,800 crowded at Slow), or 0.3 at Slow and 0.5 at Normal.
- **What the research says about the gate:** nothing directly. No source gates smoothing on measured spread. WSJT-X smooths by a fixed amount per submode, and when unsure it retries with a full decode, keeping the first CRC-valid result [S57]. That points to a third design, tested below.

### Retry design against the gates, tested (2026-11-20) [S57] [S61]

**Retry design** (`patch_fretry.py`, `JS8_FRETRY`): the plain fit and decode run first, as now. Only if that candidate does not decode is it run once more with the smoothed fit (2 passes), as WSJT-X's loops do. "Retry every failure" retries any failed candidate; "retry at 0.3" only those with a spread share of 0.3 or more. Weak-sync rescue 7 in every setting except Off. All Normal mode.

Slow, six sets plus four fresh 10 Hz sets (seeds 121, 122, 131, 132; 131 and 132 brand-new for the gate test), messages / chances:

| Setting | All | Brand-new 10 Hz (131, 132) | Crowded | Noise chances |
|---|---|---|---|---|
| Off | 2,422 / 1,297 | 49 | 1,173 | 11 |
| Gate 0.5 | 2,503 / 1,462 | 68 | 1,173 | 11 |
| Gate 0.3 | 2,520 / 1,426 | 74 | 1,168 | 10 |
| Retry every failure | **2,581 / 2,823** | **89** | 1,188 | **21** |
| Retry at 0.3 | 2,563 / 2,059 | 89 | 1,175 | 18 |

Normal speed, seven ordinary sets and six 10 Hz sets, messages / chances:

| Setting | Ordinary | 10 Hz | Noise chances | Crowded time, times the original |
|---|---|---|---|---|
| Off | 3,128 / 2,819 | 1,145 / 961 | 24 | 1.90 |
| Gate 0.5 | 3,128 / 2,837 | 1,274 / 967 | 22 | 1.92 |
| Gate 0.3 | 3,117 / 2,770 | 1,392 / 862 | 19 | not run (1.91 to 1.94 in the earlier run) |
| Retry every failure | **3,305 / 5,460** | 1,469 / 1,787 | **45** | **about 2.9** |
| Retry at 0.3 | 3,134 / 3,034 | 1,451 / 1,591 | 35 | **2.02** |

- Wrong messages: 0 in every row.
- **Both retries lose nothing anywhere, and they gain the most.** Retry every failure also lifts ordinary Normal-speed sets by 5.7% (3,128 → 3,305), past today's Deep (3,241 on the same sets): a second fit gives every failed candidate another chance, not only spread ones.
- **The cost is wrong-message chances, roughly double.** Retry every failure: Normal ordinary 2,819 → 5,460, noise 24 → 45. Retry at 0.3: noise 24 → 35. By Rob's rule this goes behind Deep.
- **Time:** retry every failure takes the crowded Normal set to about 2.9 times the original (0.488 → 0.731 s per file, against the original's 0.250 s). Retry at 0.3 takes it to 2.02 times. Both are over Normal's 2-times limit. At Slow: +25% (1.027 → 1.280 s) and +9% (1.118 s), far inside Slow's 30 s cycle. Gate 0.5: 1.92 times at Normal, +3% at Slow.
- **What fits the rules (2-times limit at Normal; more chances only behind Deep):**
  - Normal mode: only gate 0.5. It loses nothing and stays inside the time limit.
  - Deep: a retry design. ~~Not measured in Deep yet.~~ Measured below.

**In Deep** (same test copy, same sets), messages / chances; crowded time per file:

| Setting | Normal-speed ordinary | Normal-speed 10 Hz | Slow, all ten sets | Noise chances (Normal speed) | Crowded time, Normal speed | Crowded time, Slow |
|---|---|---|---|---|---|---|
| Off | 3,241 / 4,368 | 1,153 / 1,509 | 2,450 / 2,130 | 43 | 0.815 s | 1.351 s |
| Gate 0.5 | 3,241 / 4,397 | 1,281 / 1,497 | 2,533 / 2,328 | 43 | 0.822 s (+1%) | 1.377 s |
| **Retry every failure** | **3,432 / 8,489** | **1,477 / 2,766** | **2,613 / 4,469** | **79** | **1.296 s (+59%)** | 1.844 s (+36%) |
| Retry at 0.3 | 3,247 / 4,835 | 1,457 / 2,438 | 2,593 / 3,325 | 66 | 0.881 s (+8%) | 1.543 s (+14%) |

- Wrong messages: 0 new; the 1 in each Normal-speed row is the known one from before (the same in Off).
- Original decoder in the same run: 0.260 s per crowded Normal-speed file. So Deep is 3.1 times the original now, 5.0 times with retry every failure. Deep has no written time limit.
- **Retry every failure in Deep:** +191 ordinary messages (+5.9%), 10 Hz +28%, Slow +163, nothing lost; 1.9 times the wrong-message chances (expected wrong on these sets 1.07 → 2.07).
- **Decided by Rob 2026-11-20:** Normal mode gets gate 0.5 (with the weak-sync rescue); Deep gets retry every failure (with the rescue), as tested: the plain fit first, the smoothed fit only for a candidate that fails, with no inline smoothing. Normal and Slow speeds only; Fast and Turbo unchanged (no gain measured there).

**Built 2026-11-20** (`kFitSmoothReach`, `kFitSmoothWeights`, `kFitSmoothShare`, `kWeakSyncMinSpread`, `smooth_fit_` in `src\decoder\legacy_decoder.cpp`: the fit in `attempt_decode`, the weak-sync check before the decode passes, Deep's second attempt in the candidate loop). Built version against the test numbers, messages / wrong / chances:

| Sets | Test | Built |
|---|---|---|
| Slow, ten sets, Normal | 2,503 / 0 / 1,462 (gate 0.5) | 2,503 / 0 / 1,462 |
| Normal-speed ordinary, Normal | 3,128 / 0 / 2,837 (gate 0.5) | 3,128 / 0 / 2,837 |
| Normal-speed 10 Hz, Normal | 1,274 / 0 / 967 (gate 0.5) | 1,274 / 0 / 967 |
| Slow, ten sets, Deep | 2,613 / 0 / 4,469 (retry every failure) | 2,613 / 0 / 4,469 |
| Normal-speed ordinary, Deep | 3,432 / 1 / 8,489 (retry every failure) | 3,432 / 1 / 8,489 |
| Normal-speed 10 Hz, Deep | 1,477 / 1 / 2,766 (retry every failure) | 1,477 / 1 / 2,766 |
| Fast, Deep (fading, crowded, 10 Hz, noise) | 2,513 / 3 / 2,333 (decoder before) | 2,513 / 3 / 2,333 |
| Turbo, Deep (same kinds of sets) | 1,904 / 1 / 2,203 (decoder before) | 1,904 / 1 / 2,203 |

- Identical everywhere. The wrong messages are the known ones, the same in the decoder before.
- Crowded time per file, decoder before → built (original 0.249 s): Normal speed, Normal mode 0.471 → 0.472 s (1.90 times the original); Deep 0.794 → 1.259 s (+59%, 5.06 times). Slow, Normal mode 1.019 → 1.034 s; Deep 1.304 → 1.784 s (+37%).
- Project tests: 19 of 19 pass.
- Not measured: Deep's time on a phone (phone testing is off until Rob says).
- A side note from the check: a counting copy made with `git archive` from Git Bash did not count wrong-message chances. The copy made from the file itself did. Messages matched either way.

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

### Today's decoder against the original JS8Core, every speed (2026-11-20) [S62]

Original JS8Core decoder (cba17f3, `scorecard\bin\dec-orig`) against ours at 798e6ce, Normal mode and Deep, on the same 44 test sets. Messages decoded, totals of the sets with signals (noise-only sets left out; every decoder gave 0 messages on them):

| Group | Sets | Original | Ours, Normal | Ours, Deep |
|---|---|---|---|---|
| Normal speed, ordinary | `ffade fflat ffade2 fflat2 crowd60 crowd2` | 2,592 | 4,302 (+66%) | 4,614 (+78%) |
| Normal speed, fading 0.1 to 1.5 Hz | `itu_mq itu_lq itu_mm itu_mv itu_lm` | 2,396 | 3,423 (+43%) | 3,531 (+47%) |
| Normal speed, 10 Hz fading | `itu_ld itu_hm` and the six 0 to -12 dB 10 Hz sets | 0 | 1,280 | 1,486 |
| Normal speed, 30 Hz fading | `itu_hd itu_hd_s` | 0 | 0 | 0 |
| Fast, all sets | `bflat bfade bcrowd bcrowd2 bhm_f bld_f bhd_f` | 3,983 | 6,530 (+64%) | 6,613 (+66%) |
| Turbo, all sets | `cflat cfade ccrowd ccrowd2 chm_f cld_f chd_f` | 2,778 | 4,355 (+57%) | 4,410 (+59%) |
| Slow, all sets | `eflat efade emm ehm ecrowd` | 1,667 | 2,345 (+41%) | 2,410 (+45%) |

- Wrong messages: original 0, ours 5 in Normal mode and 11 in Deep, all from the last try (the original has none). On the four noise-only sets (1,300 files) all three gave 0.
- Crowded time per file, original / ours / Deep: Normal speed (`crowd60`) 0.257 / 0.492 / 1.320 s (1.91 and 5.14 times); Fast (`bcrowd`) 0.213 / 0.332 / 0.501 s (1.56 and 2.35 times); Turbo (`ccrowd`) 0.118 / 0.173 / 0.254 s (1.47 and 2.15 times); Slow (`ecrowd`) 0.718 / 1.030 / 1.777 s (1.43 and 2.47 times). Normal mode is inside its 2-times limit at every speed.
- Every set, with files, signals, levels and wrong messages per decoder: `docs\decoder-vs-original.html`.

### Deep's retry: where the time goes, and repeats skipped (2026-11-20) [S63]

**Where Deep's crowded time goes** (timed copy of 798e6ce, `crowd60`, 30 files, seconds per file): first attempt at each candidate 0.675, retry with the smoothed fit 0.529, candidate search 0.038. The retry decoded 1.1 messages per file. Normal mode's first attempt takes 0.408; the gap is Deep's reading scale of 6.0 (section 4).

- About a third of retries land on exactly the first attempt's time and frequency: 32 of 104 per file on `crowd60`, 50 of 156 at Slow (`ecrowd`). None of them decoded (about 960 and 1,510 retries). They cost 0.202 s per file at Normal speed and 0.223 s at Slow.
- Cause: in Deep the retry changes only the fit (`fit(smooth_speed && smooth_fit_)`). The same fit gives the same readings, so the same failed result.
- No published source does this skip. WSJT-X Q65's retry loop (`lib/qra/q65/q65_loops.f90`, master) starts at the unshifted position and keeps no record of positions tried; whether that repeats its caller's decode is not visible in the files read. [S57]

**Built:** Deep's retry stops right after its fit if the fit lands on the first attempt's time and frequency. Normal mode unchanged. Against 798e6ce:

| Sets (Deep) | Messages | Wrong | Wrong-message chances | Noise chances |
|---|---|---|---|---|
| Normal-speed ordinary (7 sets) | 3,432 → 3,432 | 1 → 1 | 8,489 → 6,736 | 79 → 72 |
| ITU conditions (8 sets) | 3,540 → 3,540 | 2 → 2 | 7,910 → 6,351 | 79 → 72 |
| 10 Hz and 30 Hz, 0 to -12 dB (7 sets) | 1,477 → 1,477 | 1 → 1 | 2,807 → 2,482 | 79 → 72 |
| Slow (6 sets) | 2,410 → 2,410 | 0 → 0 | 3,423 → 2,546 | 35 → 29 |

- Line for line (message, SNR, frequency, time, quality) on 15 sets in Deep: 14 identical; `ecrowd` differs in one field of one record (`S-22_8.wav`: the same correct message, quality 0.6833 → 0.6333), because the spread last try's 32-use limit (`rescue_budget`) has uses left for later candidates.
- The fewer chances are mostly the repeats: an identical repeat of a failed last try gives the same failed CRC check, so it was never a new chance of a wrong message. The earlier "1.9 times the chances" for Deep's retry counted them; against Deep without the retry (4,368 on the ordinary sets) it is now 1.54 times.
- Normal mode, ordinary sets: 3,128 messages, 2,837 chances, identical to 798e6ce.
- Crowded time per file, 798e6ce → change: Normal speed Deep 1.279 → 1.103 s (−14%, 4.3 times the original's 0.257 s); Normal mode 0.480 → 0.477 s; Slow Deep 1.796 → 1.593 s (−11%). 19 of 19 project tests pass.
- Still open in Deep's time: retries on a different fit (0.33 s per file for 1.1 messages) and the first attempt at scale 6.0 (0.675 against 0.408 s). Next by the research: the give-up rule checked at set rounds [S21], then damping [S23].

### The give-up rule at set rounds, tested (2026-11-20) [S21] [S64]

**The paper's rule, read again from its text.** At three set rounds (theirs: 4, 5 and 6 of at most 15) the failed-check count is compared with three limits; above all three, decoding stops. Limits and rounds are "obtained empirically" per code: 120, 95 and 100 of 384 checks for their 2048-bit code. Result: 0.04 dB loss, up to 2.4 times fewer rounds. [S21, sections III and IV]

**Our decoding calls, logged** (134,512 calls, Deep, Normal and Slow speed):
- Calls that never succeed take 99% to 100% of all decoding rounds, about 47 rounds each; 2% to 3% of calls succeed.
- 393 calls (Normal speed) and 257 (Slow) succeeded after round 10. At rounds 4 to 10 some of them still had 34 to 43 failed checks of 87, inside the range where failing calls sit. The count does not separate them on our code.
- Rounds saved against successful calls stopped, one limit for all three rounds: at round 6, limit 30 saves 2% of rounds for 8 stopped successes; limit 20 saves 20% for 61.

**End to end** (Normal-speed sets, Deep, 3,432 messages without the rule): round 6 limit 30: 3,429; limit 25: 3,420; limit 20: 3,412. Round 10 limit 25: 3,430; limit 20: 3,417. Wrong-message chances unchanged (6,726 to 6,738). Crowded time at round 6 limit 20, the strongest: 1.125 → 1.021 s per file (−9%).

- **Result: not built.** Every setting loses messages; the most time it saves is 9%.

### Damping, tested (2026-11-20) [S23] [S65]

**Research.** Damping blends each round's messages with the last round's: new = γ × last + (1 − γ) × computed, for messages in both directions, starting from zero; "to mitigate oscillations and enhance convergence". γ = 0 is standard decoding. The paper learns γ per code and gives no value; no effect on round counts is stated. It cites Fossorier, Palanki and Yedidia 2003 for damping against oscillation (read later: [S66], next section). [S23, section II-A]

**Measured** (copy with γ settable, every decoding call, both modes). Messages / wrong-message chances:

| Sets | Normal mode, γ 0 | γ 0.1 | γ 0.2 | Deep, γ 0 | γ 0.1 | γ 0.2 |
|---|---|---|---|---|---|---|
| Normal-speed ordinary (7) | 3,128 / 2,837 | 3,152 / 2,842 | 3,157 / 2,826 | 3,432 / 6,736 | 3,453 / 6,709 | 3,455 / 6,690 |
| ITU conditions (8) | 3,429 / 2,454 | 3,453 / 2,440 | 3,446 / 2,451 | 3,540 / 6,351 | 3,561 / 6,329 | 3,559 / 6,331 |
| 10 Hz and 30 Hz, 0 to -12 dB (7) | 1,274 / 972 | 1,387 / 918 | 1,390 / 901 | 1,477 / 2,482 | 1,589 / 2,317 | 1,598 / 2,301 |
| Slow (6) | 2,345 / 1,125 | 2,364 / 1,126 | 2,356 / 1,123 | 2,410 / 2,546 | 2,426 / 2,553 | 2,421 / 2,536 |
| Fast (8) | 6,530 / 3,672 | 6,593 / 3,609 | 6,590 / 3,613 | 6,613 / 5,722 | 6,668 / 5,644 | 6,668 / 5,645 |
| Turbo (8) | 4,355 / 3,493 | 4,368 / 3,475 | 4,370 / 3,463 | 4,410 / 5,341 | 4,432 / 5,302 | 4,429 / 5,298 |

- γ 0.3: Normal-speed ordinary 3,138 (Normal mode), 3,434 (Deep). γ 0.5: 3,113 and 3,414, fewer than without damping.
- Wrong messages identical in every group; noise-only chances unchanged.
- **Fresh sets** (seeds 151 to 155, never used before), messages γ 0 / 0.1 / 0.2, Normal mode: fading 612 / 620 / 621, fade-free 731 / 727 / 728, 10 Hz 201 / 221 / 217, crowded 778 / 779 / 780, Fast fading 876 / 879 / 880. Deep: 648 / 657 / 657, 780 / 776 / 777, 240 / 258 / 256, 904 / 904 / 905, 885 / 887 / 888. The fade-free loss does not repeat on the other fade-free sets at γ 0.1 (`fflat` +1, `fflat2` +5, `eflat` 0, `bflat` +4); it is within the borderline variation noted above.
- Crowded time per file (`crowd60`, original 0.247 s): Normal mode 0.487 / 0.490 / 0.484 s (about 1.96 times the original at every γ); Deep 1.105 / 1.055 / 1.049 s (−5%). Fast and Turbo crowded time not measured.
- **Result:** damping at γ 0.1 or 0.2 adds messages at every speed and in both modes (+9% on 10 Hz fading, +0.4% to +1% elsewhere), with the same wrong messages, mostly fewer chances, and no extra time. It does not give the time saving it was tested for. γ 0.1 and 0.2 are level on these sets. Built at γ 0.1 (next sections).

### Damping, research after the test (2026-11-20) [S66] [S67] [S68] [S69] [S70]

Done to close three gaps: the paper [S23] cites for damping was not read, no published γ value was known, and nothing was known about damping alongside our other changes.

- **The cited paper** (Fossorier, Palanki, Yedidia, 2003 report and 2007 journal version) [S66]. Damping appears once, in the section on extending their method to Gaussian noise: g∘f = α·f(this round) + (1 − α)·f(last round), 0 < α ≤ 1. Their α weights the new value, so our γ 0.1 is their α 0.9. They list it as one of four ways to curb correlation fed back round to round on codes with short cycles: scaling, offsetting, damping, clipping. They give no α value and no damping result. Their codes are dense, highly redundant parity-check matrices (255 bits, 5,355 checks), not sparse codes like ours. [S66: 2003 section 6; 2007 section 5]
- **Stepanov and Chertkov** [S67]. Damping (their Δ; standard decoding is Δ = ∞, smaller Δ means more damping) on a 155-bit code under Gaussian noise. At their SNR 3 with a 32-round limit, Δ = 1 cut decoding errors "by factor of about 40". The error rate "depends non-monotonically on Δ". More damping needs more rounds before decodes arrive. No best value is stated. [S67, sections IV and V]
- **Pretti** [S68]. Damps each bit's total belief, not each message. The γ is the weight on the old value, as ours. On a 1,057-bit code over a bit-flip channel: best γ 0.83 for that form, 0.35 for a second form; that form fails above about 0.5. Larger γ "progressively slows down the algorithm dynamics". A rule of thumb from the code's column and row weights gives γ ≥ 0.64 for their code. [S68, sections 3.2, 4.3]
- **Against:** Garcia Satorras and Welling [S69]. On a 96-bit LDPC code, 20 rounds, Gaussian and bursty noise: "damping didn't lead to improvements". No numbers given. Their reason: LDPC graphs are sparse, so little information cycles back. [S69, section 5.2]
- **WSJT-X** [S70]. Its FT8 decoder (`lib/ft8/bpdecode174_91.f90`, `decode174_91.f90`, version 2.7.0) does not damp: every round's messages are computed fresh. Its early-stop rule is the same as ours (5 rounds without improvement, from round 10, more than 15 failed checks). It allows 30 rounds.

**What this settles:**
- **γ value.** No source gives one for a code like JS8's. Published best values range from about 0.35 to 0.83, on other codes, channels and damping forms. One source finds no gain on a short LDPC code. The sources agree on two things. The best value depends on the code and must be measured. Too much damping slows decoding and loses decodes. Our test fits both: γ 0.1 and 0.2 gained at every speed, 0.3 was about even, and 0.5 lost. γ 0.1 rests on our own measurement, repeated on fresh sets (seeds 151 to 155). The sources give no reason to doubt that.
- **Alongside our scaling.** [S66] treats scaling and damping as two answers to the same problem. That is scaling of the messages. Our 2.83 / 6.0 setting scales the tone readings that go in. No source tests damping together with input scaling. Our test measured exactly that combination: Normal mode at 2.83 and Deep at 6.0.
- **Why it may help JS8 when [S69] saw no gain** (not proven): our decoder feeds its beliefs back into the tone readings each round (iterative demapping). It also decodes fading signals. Both add the round-to-round feedback that [S66] says damping curbs. [S69] had neither.
- **Measured at the build** [S71]: crowded-band time at Fast and Turbo (below).

### Damping, built (2026-11-20) [S71]

`src/decoder/ldpc.cpp`, `belief_propagation`: every message in both directions keeps 0.1 of its value from the round before (`kDamping = 0.1f`), messages starting at zero. Every caller, every speed, both modes. `refine_reliabilities` (the last try's ordering rounds) is unchanged, as in the test.

**Check against the tested copy (γ 0.1 column above)**, messages / wrong / chances, Normal mode then Deep:

| Sets | Normal mode | Deep |
|---|---|---|
| Normal-speed ordinary (7) | 3,152 / 0 / 2,842 | 3,453 / 1 / 6,709 |
| ITU conditions (8) | 3,453 / 0 / 2,440 | 3,561 / 2 / 6,329 |
| 10 Hz and 30 Hz, 0 to -12 dB (7) | 1,387 / 0 / 918 | 1,589 / 1 / 2,317 |
| Slow (6) | 2,364 / 0 / 1,126 | 2,426 / 0 / 2,553 |
| Fast (8) | 6,593 / 3 / 3,609 | 6,668 / 5 / 5,644 |
| Turbo (8) | 4,368 / 2 / 3,475 | 4,432 / 2 / 5,302 |

Every number equals the tested copy's. 19 of 19 project tests pass.

**Crowded-band time**, seconds per file, same machine, one run (`.speed.py`), original / before damping / after:

| Set | Original | Normal mode before → after | Deep before → after |
|---|---|---|---|
| Fast `bcrowd` (80 files, 2,960 signals) | 0.216 | 0.337 → 0.336 | 0.508 → 0.483 (−5%) |
| Turbo `ccrowd` (80 files, 1,520 signals) | 0.122 | 0.176 → 0.176 | 0.256 → 0.247 (−4%) |

Messages in the timing run: Fast Normal mode 902 → 909, Deep 927 → 932; Turbo Normal mode 444 → 444, Deep 461 → 464. Wrong messages the same (Fast 2, Turbo 1, in every column).

### Today's decoder against the original, re-run after damping (2026-11-20) [S72]

Our decoder at `2428ff6` (Normal mode and Deep) on the same 44 sets as [S62]; the original's counts are [S62]'s (its code is unchanged). Signals decoded (rate ± 95% range):

| Speed | Signals | Original | Normal mode, 798e6ce → 2428ff6 | Deep, 798e6ce → 2428ff6 |
|---|---|---|---|---|
| Normal | 27,000 | 18.5% | 33.4% → 34.0% ±0.6 | 35.7% → 36.2% ±0.6 |
| Fast | 14,820 | 26.9% | 44.1% → 44.5% ±0.8 | 44.6% → 45.0% ±0.8 |
| Turbo | 10,140 | 27.4% | 42.9% → 43.1% ±1.0 | 43.5% → 43.7% ±1.0 |
| Slow | 4,600 | 36.2% | 51.0% → 51.4% ±1.4 | 52.4% → 52.7% ±1.4 |
| All | 56,560 | 23.7% | 39.3% → 39.8% ±0.4 | 40.8% → 41.2% ±0.4 |

- 10 Hz fading at Normal speed: 1,280 → 1,394 of 5,600 (Normal mode), 1,486 → 1,599 (Deep).
- Wrong messages: 5 of 22,497 decoded (Normal mode), 11 of 23,324 (Deep), the same counts as before; against the original's 0 of 13,416 the two-sided split test gives 16.5% (cannot tell apart) and 1.0% (higher). 0 wrong on 1,300 noise-only files.
- Crowded time, all three re-timed in one run (seconds per file, original / Normal mode / Deep): Normal speed 0.248 / 0.489 / 1.048; Fast 0.218 / 0.339 / 0.490; Turbo 0.121 / 0.178 / 0.247; Slow 0.714 / 1.037 / 1.532. Deep is 4.23 times the original at Normal speed, down from 5.14 at 798e6ce (the retry skip and damping).

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
- ~~The Fast and Turbo speeds: none of this research covers them.~~ Measured 2026-11-19 (section 5) on computer-made audio. Still open: real on-air audio at these speeds. Slow speed measured 2026-11-19 for the fading changes (section 5): no losses. 10 Hz at Slow decodes nothing; cause found (wide-reading reach); reach 4 built for Slow 2026-11-20. Still open: the time and frequency fit under spread.
- ~~How often the last-try decode lets a wrong message through near real signals.~~ Measured 2026-11-19 (section 4): about 1 in 4,096 per last-try result reaching the CRC check. Still open: real on-air audio and the other nine fading conditions. (The runner-up gap, now 0.0025, was measured on Fast and Turbo too; section 4.)
- ~~Normal's rule of 10 sync tones for the 3-tone last try (`kBlockOsdMinSyncMatches`) rests on the "9 sync tones" pattern, which turned out to be chance.~~ Re-measured 2026-11-19 (section 4): kept; 9 sync tones adds 7% to 10% more wrong-message chances for 0.15% to 0.4% more messages.
- How fast the decoder runs on a phone. Not tested (phone testing is off until Rob says).
- ~~Our decoder on the other nine ITU fading conditions.~~ Measured 2026-11-19 at Normal speed (section 5): gains hold up to 1.5 Hz spread. At 10 Hz nothing decoded, even at 0 dB, where WSJT-X's FT8 reaches -8.6 dB: the signals were found but their tones misread about half the time (likely reason FT8 copes: Gray-coded tones; not proven). **Fixed for 10 Hz, 5ad9bbc** (spread-aware reading and pass 11, Normal and Deep): about half decode at 0 to -4 dB, 28% at -8 dB. Still open: **30 Hz spread decodes nothing at Normal or Fast speed** (Turbo: 29 of 400, section 5; cause found 2026-11-20: the candidate search misses most of these signals; no published fix found); the gap to FT8 at 10 Hz (50% at -8.6 dB; ours about -4 to -5.3 dB after iterative demapping, **about -8 dB on the fresh high-latitude set after the wide reading**, 2026-11-20, section 5); ~~feeding the last try the iterated readings~~ (built 2026-11-20 for Deep, small gain, section 5). ~~Iterative demapping (researched, not built).~~ Built 2026-11-20 (section 5): +5.5% messages on ordinary Normal-speed sets, +19% on fresh 10 Hz sets, fewer wrong-message chances, no extra time at Normal speed. ~~Fast and Turbo not run on these conditions or on the fix.~~ Measured 2026-11-20 (section 5): Fast 10 Hz 139 → 353 of 800, Turbo unchanged at 10 Hz, ordinary audio +0.3% to +1.8% chances and time; kept at every speed.
- Whether retuning the standard decode's early-stop rule would keep the bigger scale's gains without its slowdown on a crowded band. [S19]
- What a wrong scale costs an LDPC sum-product decoder in published measurements: the main paper (Saeedi and Banihashemi 2007) could not be reached. [S18]

## 10. Literature search for new methods (2026-11-20)

Four rounds of searching, no date limit, about 3 hours: 45 searches and 9 sources read. Goal (Rob): any gain, even 1%. Reading only; tests of the leads follow in this section.

### Leads, in the order to test them

1. **Re-read the same audio with small changes** [S73]. After the normal decode, the untouched audio is decoded again after one small change: a delay of a few samples, faint added noise (3% to 5% of the audio level; 30% destroys the weak signals), or a frequency shift under one bin. Every new message is kept. Measured on a recorded on-air hour of 20 m FT8: the changed re-reads alone add 15.3%; a 104-message test recording goes from 85 to 101 with five re-reads. The author's reason: borderline signals flip between found and lost from changes smaller than the noise; a finer candidate search gained nothing, the re-read changes the tone reading of signals the decoder gave up on. Cost: three re-reads took 7.6 s per period. Each extra last try is another 1 in 4,096 chance of a wrong message for us (FT8's check is 4 times stronger), so this fits Deep only. GPL program: idea only.
2. **Hints for heartbeat and CQ frames** [S78]. WSJT-X credits its hints with "up to 4 dB" for its 8 hint types together, no figure per type. Ours cover 3 of them, for directed frames only: my call, a heard station to my call, a heard station to anyone (`src\decoder\legacy_decoder.cpp`, `build_hints`). Missing: WSJT-X type a1 (CQ with the rest unknown) and a7 (a station heard in the last period); in JS8 these are heartbeat and CQ frames from heard stations. Our 10-minute heard-station memory (`include\js8core\engine.hpp`, `kHintHeardMinutes`) is already longer than the 4 periods measured in [S73] (+6.0%).
3. **A finer frequency fit from the sync tones' phases** [S76]. LogLab fits frequency from the phases of the 21 sync tones to "a few hundredths of a hertz" and adds a 4-tone reading: 2,903 of 3,600 signals against WSJT-X's 2,777, and 79% between -22 and -20 dB against WSJT-X's 54%, 0 false decodes; simulated audio only, AGPL (idea only). Ours steps frequency by 0.5 Hz (`src\decoder\decoder_tables.hpp`, `kFreqSearchSteps`), worst error 0.25 Hz: at Normal speed that turns the phase 0.12 of a cycle (43°) across 3 tones (my arithmetic; LogLab's text says "a third of a cycle", which I cannot reconcile). Deciding check: decode the test sets at their true frequencies.
4. **A phase check against wrong decodes** [S76]. Every decode must show its tones adding in phase over every run of 4 tones to 30 times the noise power; noise "never passed 24 in 48,000 tries", real decodes down to -23 dB scored above 30. A test the 12-bit CRC does not make. Risk: fast fading scrambles phase; LogLab added a second test for echo paths.
5. **Update checks one at a time, busiest first** (node-wise residual belief propagation) [S77]. Better than the fixed order at every round count and fixes most decoder traps, on a 1,944-bit code only. A 2024 study says one-at-a-time orders lose some advantage on short codes [S74]. Ours updates every check at once each round (`src\decoder\ldpc.cpp`, `belief_propagation`).
6. **Try every sign of the 3 least reliable bits** (saturated BP, 8 decodes) [S74]. Of five ensemble methods, this and added-noise decoding are the two that work on any code; saturated BP beat added noise on every code. Gains only in figures, against the standard decode alone; we already run a last try, so the gain for us is unknown.
7. **Faster last-try methods** [S79]: skip hopeless guesses using a cheap probability first. Time only; could pay for the deeper search we skip. Search summary only.
8. **Match the reading to the fading speed** [S76]: 5 fading classes from the sync tones, reading chosen per class; 8,818 against WSJT-X's 8,747 over 32 simulated conditions, +24% to +31% CPU. Our spread-aware reading already switches on spread.
9. **Follow drift** [S75]: steady drift fitted with the sync tones, believed only if it explains 10% more sync power; at 0.5 Hz/s 307 against 151 of 400; simulated only. Our test audio has no drift.
10. **Change the order decoded signals are removed in** [S73] [S79]: one example of two signal pairs that decoded only under one order; one decoder offers it with no measurement.
11. **Mark unsure decodes instead of dropping them** [S78]: WSJT-X shows "?" on likely-but-not-certain decodes and keeps them off PSK Reporter. Needs a decode-output field and Easy JS8.

### Areas searched with nothing usable for us [S79]

- Learned (neural) decoders: a few scale settings match them [S23], and we have scaling and damping.
- Combining repeated messages across periods (Q65 style): needs the same message sent many times (often 20 to 30).
- Accepting last-try answers with a short check: the nearest paper uses a distance limit, the same kind as ours.
- Bit reading under fading, noise bursts, steady carriers, combining the 1-, 2- and 3-tone readings, the tone-reading window, underwater modems, military 8-tone HF links, joint decoding of overlapping signals, code-aided synchronisation: no measured result that applies, or patents only.
- The LQ mode paper (2026): a new protocol on FT8's code with a standard receiver [S80].
- No published JS8-specific decoder research.

### The fit at the true frequency and time, and a second time fit (tested 2026-11-20) [S81] [S82]

**Ceiling check (lead 3).** Decoding the four Normal-speed sets at their deciding levels (`fflat` -19 to -21, `ffade` -17 to -20, `fflat2`, `ffade2`; 4,200 signals) with each candidate snapped to its true frequency, and then to its true frequency and start time:

| Mode | Today | True frequency | True frequency and time |
|---|---|---|---|
| Normal | 2,036 | 2,192 (+7.7%) | 2,438 (+19.7%) |
| Deep | 2,150 | 2,274 (+5.8%) | 2,583 (+20.1%) |

Wrong messages 0 or 1 in every row.

**Where the loss is.** Normal mode, same sets: frequency steps of 0.1 Hz instead of 0.5 Hz: 2,033 (no gain, so frequency precision is not the loss and lead 3's sync-phase fit is not needed for it). A time window twice as wide: 2,219 (+9.0%). **Fitting time again after the frequency is corrected: 2,259 (+11.0%).** Our fit searches time at the uncorrected frequency, then frequency, and never searches time again. WSJT-X searches time a second time, over ±4 samples, after correcting the frequency (`lib/ft8/ft8b.f90`, lines 141 to 152) [S82]; JS8Core's fit lacks that step.

**Two versions tested on every speed, both modes** (messages, and wrong-message chances on the signal sets): "time again + frequency again" (full time window, then frequency again) and the **WSJT-X version** (time again over half the window, no second frequency search).

| Sets | Mode | Today | Time again + frequency again | WSJT-X version |
|---|---|---|---|---|
| Normal speed, 7 sets | Normal | 3,152 / 2,842 | 3,593 / 2,814 | **3,585 / 2,735** (+13.7%) |
| Normal speed, 7 sets | Deep | 3,453 / 6,709 | 4,009 / 6,736 | **3,995 / 6,578** (+15.7%) |
| Fast, 6 sets | Normal | 19,721 / 12,105 | — | **28,962 / 8,471** (+46.9%) |
| Fast, 6 sets | Deep | 20,005 / 19,231 | — | **29,491 / 12,679** (+47.4%) |
| Turbo, 6 sets | Normal | 10,036 / 10,912 | — | **14,153 / 9,142** (+41.0%) |
| Turbo, 6 sets | Deep | 10,306 / 16,599 | — | **14,529 / 13,381** (+41.0%) |
| Slow, 6 sets | Normal | 2,364 / 1,126 | — | **2,761 / 1,076** (+16.8%) |
| Slow, 6 sets | Deep | 2,426 / 2,553 | — | **2,875 / 2,507** (+18.5%) |
| 10 Hz fading, 6 sets | Normal | 1,387 / 913 | — | **1,537 / 876** (+10.8%) |
| 10 Hz fading, 6 sets | Deep | 1,589 / 2,279 | — | **1,764 / 2,291** (+11.0%) |

- Wrong messages stayed at the chance level in every run (0 to 5; Slow Deep 2 against 0.61 expected).
- Wrong-message chances on noise-only files rose: Normal speed 22 → 24 (Normal mode) and 72 → 84 (Deep); Slow 11 → 16 and 29 → 43; Fast 10 → 11 and 23 → 24; Turbo 0 → 2 and 0 → 4.
- Crowded-band time per file (`.speed.py`, one file at a time; original / today / WSJT-X version; Deep today / Deep WSJT-X version): Normal speed `crowd60` 0.256 / 0.504 / 0.518 s (2.02 times the original, from 1.97; the original itself measured 0.248 and 0.256 s in two runs) / 1.086 / 1.121 s; Fast `bcrowd` 0.223 / 0.350 / 0.357 / 0.498 / 0.473 s; Turbo `ccrowd` 0.123 / 0.179 / 0.178 / 0.249 / 0.239 s; Slow `ecrowd` 0.713 / 1.030 / 1.116 / 1.525 / 1.551 s.
- Remaining ceiling: with the WSJT-X version, 2,262 of 4,200 decode on the four sets; at the true frequency and time, 2,437 (+7.7% still left).
- Not built (test copies only).

### Why the time fit loses: candidate start times are early (tested 2026-11-20) [S81] [S83]

**Measured.** Candidate start time minus true start time, every candidate within 1.5 Hz of a true signal (`scorecard\.cand_bias.py`): median **-0.043 s** at Normal speed (`fflat2`, 1,325 candidates; `ffade2` -0.041 s), **-0.039 s** Fast (`bflat`), **-0.020 s** Turbo (`cflat`), **-0.107 s** Slow (`eflat`). The fit then searches only a quarter symbol each side (0.040, 0.025, 0.0125 and 0.080 s), so at Fast and Turbo most true start times lie outside it. Decoded messages report times within a few ms of the truth (median -0.003, -0.007, -0.003, -0.016 s), so the bias belongs to the candidate search alone.

**Cause.** Our candidate spectra fill the whole 2-symbol FFT span with audio under a Nuttall window that peaks one symbol in (`src\decoder\legacy_decoder.cpp`, `build_window` and `find_candidates`), so a tone scores best when the window starts 2 coarse steps (half a symbol) before it; the `+ 0.5` step in the candidate time takes back half a step, leaving 1.5 steps, less the rounding of `start_step`. That predicts -0.040, -0.0375, -0.019 and -0.100 s: measured -0.043, -0.039, -0.020, -0.107. WSJT-X fills its candidate FFT with one symbol of audio, zero-padded to two (`lib/ft8/sync8.f90`, lines 34 to 38) [S83], so its candidate time has no such lag.

**Fix tested** (`scorecard\.patch_candfix.py`, `JS8_CANDFIX`): candidate time = (best step + `start_step` + 2) x step - start time. Bias after: -0.002 s (`fflat2`). Normal mode, 7 Normal-speed sets: start-time fix alone 3,152 → 3,492 (+10.8%); second time fit alone 3,585; both 3,578 (the two repair the same loss at Normal speed). On top of the second time fit, the start-time fix adds Fast 28,962 → 32,687 (+12.9%, chances 8,471 → 6,390), Turbo 14,153 → 16,255 (+14.9%, chances 9,142 → 7,980), Slow 2,761 → 2,860 (+3.6%, chances same).

**Both together against today:**

| Sets | Normal mode | Deep |
|---|---|---|
| Normal speed, 7 sets | 3,152 → 3,578 (+13.5%); chances 2,842 → 2,708 | 3,453 → 4,020 (+16.4%); 6,709 → 6,615 |
| Fast, 6 sets | 19,721 → 32,687 (+65.7%); 12,105 → 6,390 | 20,005 → 33,248 (+66.2%); 19,231 → 9,444 |
| Turbo, 6 sets | 10,036 → 16,255 (+62.0%); 10,912 → 7,980 | 10,306 → 16,611 (+61.2%); 16,599 → 11,555 |
| Slow, 6 sets | 2,364 → 2,860 (+21.0%); 1,126 → 1,077 | 2,426 → 2,980 (+22.8%); 2,553 → 2,649 |
| 10 Hz fading, 6 sets | 1,387 → 1,556 (+12.2%); 913 → 879 | 1,589 → 1,789 (+12.6%); 2,279 → 2,309 |

- Wrong messages at the chance level, except Slow Deep: 2 against 0.65 expected, the second run in a row at 2 (earlier 2 against 0.61). Not yet explained.
- Chances on noise-only files up everywhere: Normal speed 22 → 28 (Normal mode), 72 → 87 (Deep); Fast 10 → 16, 23 → 39; Turbo 0 → 6, 0 → 11; Slow 11 → 16, 29 → 59.
- Crowded time per file, original / today / both fixes, Normal mode: Normal speed 0.249 / 0.489 / 0.509 s (2.04 times the original; the original measured 0.248 to 0.256 s over three runs, so 1.99 to 2.04 times); Fast 0.210 / 0.331 / 0.333 s; Turbo 0.118 / 0.174 / 0.168 s; Slow 0.697 / 1.005 / 1.101 s (+9.6%). Deep, today → both fixes: 1.046 → 1.083 s; 0.477 → 0.422 s; 0.243 → 0.220 s; 1.483 → 1.496 s.
- Not built (test copies only).

### Re-reading the same audio with small changes, tested (2026-11-20) [S73] [S81]

Test program copy (`scorecard\.patch_reread.py`, `JS8_REREAD`): after the normal decode, the file is decoded again after each change and only new messages are kept. Deep, 7 Normal-speed sets, on top of the second time fit (messages / chances on signal sets / noise chances / time for the run):

| Re-reads | Result |
|---|---|
| None | 3,995 / 6,578 / 84 / 34 s |
| Delay half a coarse step | 4,360 (+9.1%) / 13,543 / 178 / 61 s |
| Delay a quarter coarse step | 4,285 (+7.3%) / 13,300 / 174 / 63 s |
| Noise at 4% of the audio level | 4,084 (+2.2%) / 13,344 / 167 / 66 s |
| All three | 4,514 (+13.0%) / 26,855 / 365 / 121 s |

With both fit fixes, the half-step delay adds only 4,020 → 4,199 (+4.5%) for 6,615 → 13,348 chances and twice the time: about 1 expected wrong message per 110 messages gained, against Deep's 1 per 2,100 today. **Not worth building.** A wider fit window gained nothing once the second time fit was in (3,577 against 3,585, Normal mode). A finer candidate time grid was tried and decoded less (2,442), because the test copy shifted candidate times early by 0.03 s (the start-time rounding changes with the step); not retried.

### Hints for heartbeat and CQ frames, tested (2026-11-20) [S78] [S84]

**What was tested** (lead 2). One extra hint per heard station: frame type 0 (payload bits 0 to 2), the station's 50-bit callsign (bits 3 to 52), data flag clear (bit 72), first-frame flag set (bit 74). The CQ flag (bit 53) stays free, so one hint covers heartbeat and CQ. Every transmission's first frame carries the first-frame flag (`src\engine\engine.cpp`, `replace_unsent_text`). The directed-frame number rule in `keeps_hint` applies to directed hints only. Test copies on top of both fit fixes (`JS8_REFIT=2`, `JS8_CANDFIX=1`); the hints are switched on by `JS8_HBHINT`.

**Sets.** Generator copy with a new `heartbeat` mode: 4 signals per file, 300 of each kind: heartbeat from a heard station, CQ from a heard station, heartbeat from an unknown station, unknown directed frame. Own call N0OWN plus 16 heard calls, 50 files per level. Normal speed -19 to -24 dB, Fast -17 to -22, Turbo -14 to -19, Slow -22 to -27. Noise-only: 200 files per speed, plus 2,000 more at Normal speed.

**Messages from heard stations' heartbeats and CQs** (directed hints → plus heartbeat hints; the other two kinds unchanged in every run):

| Speed | Mode | Heartbeats | CQs | Both kinds |
|---|---|---|---|---|
| Normal | Normal | 107 → 146 | 104 → 148 | 211 → 294 (+39%) |
| Normal | Deep | 115 → 155 | 112 → 154 | 227 → 309 (+36%) |
| Fast | Normal | 98 → 119 | 101 → 131 | 199 → 250 (+26%) |
| Turbo | Normal | 70 → 110 | 70 → 102 | 140 → 212 (+51%) |
| Slow | Normal | 106 → 156 | 98 → 158 | 204 → 314 (+54%) |

- All messages on the Normal-speed set: 418 → 501 (+20%) in Normal mode, 459 → 541 (+18%) in Deep.
- Wrong messages: 0 in every run, signal and noise sets.
- Hint results reaching the CRC check on noise: Normal speed, 2,000 files, 2,834 → 20,818 (6.3 times); 3 passed the CRC (1 directed, 2 heartbeat) and the evidence limit (`kHintMaxDiscrepancy`, 0.12) rejected all 3. Each 200-file set: 0 passed, except Fast, where 1 passed and was rejected.
- Crowded-band time per file, own call plus 16 heard calls, first 5 files of each set, both fit fixes, directed hints → plus heartbeat hints: Normal speed 2.70 → 3.73 s (+38%), Fast 0.76 → 0.95 s (+26%), Turbo 0.50 → 0.63 s (+27%), Slow 1.57 → 1.73 s (+10%).
- Not built (test copies only).

**Found on the way: hints already cost most of the crowded-band time.** Every crowded-band time in this note was measured without hints. The app passes the own call plus up to 16 heard stations (`src\engine\callsign_hints.hpp`, `kMaxHeard`), so a crowded band fills the list. Today's decoder (test copy, no test setting on), same 5 files, no hints → own call plus 16 heard calls: Normal speed 0.64 → 2.92 s (4.6 times), Fast 0.42 → 1.69 s (4.0 times), Turbo 0.21 → 0.76 s (3.6 times), Slow 1.10 → 1.70 s (1.5 times). With both fit fixes on, the hinted times fall to 2.70, 0.76, 0.50 and 1.57 s. The hint retry runs 33 hints (16 + 1 + 16) on both kinds of evidence for every weak signal that failed. No hint-time measurement was found earlier in this note; the 2.04 times the original quoted above holds only with no stations heard.

### The two time-fit fixes, built (2026-11-21) [S85]

**Slow Deep's wrong messages, checked first.** The 2 wrong messages on the old Slow sets were `eflat S-25_7.wav` (pass 10, sync 9, 27 hard errors, ratio 0.0552, reported -60 dB) and `ecrowd S-23_3.wav` (pass 9, sync 14, 26 hard errors, ratio 0.0599, reported -30 dB): both last-try results. Today's decoder gave 0 on the same files. Fresh Slow sets three times the size (`eflat3`, `efade3`, `emm3`: 600 files each, -18 to -25 dB; `ecrowd3`: 90 files of 60 signals; `enoise3`: 300 files; seeds 131 to 135), Deep: today 7,111 messages, 0 wrong, 1.72 expected; both fixes 8,637 (+21.5%), 2 wrong, 1.63 expected. Over old and fresh sets together: 4 wrong against 2.28 expected, about 1 chance in 5. Chance level; nothing to fix.

**Built** (`4f21eb5`, `src\decoder\legacy_decoder.cpp`): candidate time = (best step + `start_step` + 2) × step - start time; inside the fit, after the frequency search, time is searched again at the found frequency over ± half a quarter symbol. No setting; every speed, both modes. Project tests: 19 of 19 passed. A counting copy of the built code gave the same messages, wrong messages and chances as the tested copy on every group in both modes (Normal speed 3,578 / 2,708 and Deep 4,020 / 6,615; Fast 32,687 / 6,390 and 33,248 / 9,444; Turbo 16,255 / 7,980 and 16,611 / 11,555; Slow 2,860 / 1,077 and 2,980 / 2,649; 10 Hz 1,556 / 879 and 1,789 / 2,309).

**Against the original, the 44 sets of [S62]** (signals decoded; `2428ff6` → `4f21eb5`):

| Speed | Signals | Original | Normal mode | Deep |
|---|---|---|---|---|
| Normal | 27,000 | 18.5% | 34.0% → 36.9% ±0.6 | 36.2% → 40.0% ±0.6 |
| Fast | 14,820 | 26.9% | 44.5% → 68.0% ±0.8 | 45.0% → 69.0% ±0.7 |
| Turbo | 10,140 | 27.4% | 43.1% → 62.1% ±0.9 | 43.7% → 63.0% ±0.9 |
| Slow | 4,600 | 36.2% | 51.4% → 62.2% ±1.4 | 52.7% → 64.8% ±1.4 |
| All | 56,560 | 23.7% | 39.8% → 51.6% ±0.4 | 41.2% → 53.7% ±0.4 |

- 10 Hz fading at Normal speed: 1,394 → 1,561 of 5,600 (Deep 1,599 → 1,796).
- Wrong messages: Normal mode 5 → 1 (of 29,200 decoded), Deep 11 → 5 (of 30,391); original 0 of 13,416. Neither is now told apart from the original (two-sided test: 100% and 33.2% chance of luck). Noise-only sets: 0 for all three.
- Crowded time per file, original / Normal mode / Deep: Normal speed 0.246 / 0.502 (2.04×) / 1.083 s (4.40×); Fast 0.215 / 0.339 (1.58×) / 0.431 s (2.00×); Turbo 0.121 / 0.170 (1.40×) / 0.224 s (1.85×); Slow 0.721 / 1.133 (1.57×) / 1.545 s (2.14×). Normal speed in Normal mode is 2.04 times the original, over the results page's limit of 2 times. All measured with callsign hints off.

### Harsher test audio: drift and clock offsets (2026-11-21) [S86]

**Question (Rob):** are the gains over the original an artefact of clean computer-made audio?

**Real-world sizes, researched.** Drift: WSJT-X's FT8 decoder has no drift correction; a WSJT-X developer put the harmless limit at about half a tone spacing per transmission, and a reply gave about 3 Hz over 12.6 s (0.24 Hz/s) as still fine (wsjt-devel, 2018); LogLab tested 0.5 Hz/s [S75]. Clock offset (DT): operator guides call ±1 s fine and adjust a clock beyond 1 s; most stations sit at +0.2 to +0.6 s; no published distribution found. Sound-card clock error, typically 50 to 100 ppm (baudline): at 100 ppm a 1,500 Hz tone moves 0.15 Hz and a 15 s cycle stretches 1.5 ms (my arithmetic), well inside the fit, so not modelled.

**Generator** (`scorecard\synth_gen.cpp`, git-excluded): `drift=<Hz/s>` gives each signal a steady drift at a random rate within ± that, centred on mid-frame; `clock=<s>` moves each signal's start by a random amount within ± that (a frame starting before the file loses its first samples). Both draw from their own random stream: `ecrowd3` and `ffade` remade byte for byte identical, truth files included. Check: 0 dB, no drift 40 of 40 decoded, 0.25 Hz/s 40 of 40, 2 Hz/s 16 of 40.

**Sets.** The 12 flat, 1 Hz fading and crowded sets of every speed (`fflat ffade crowd60 bflat bfade bcrowd cflat cfade ccrowd eflat efade ecrowd`, settings confirmed by remaking each truth file), remade from the same seeds with drift 0.25 Hz/s (`_d`), clock ±1 s (`_c`, start times -0.99 to +1.78 s) and both (`_dc`): same frames, frequencies and noise, only drift and timing differ. Normal mode; the base sets reproduce [S85]'s counts exactly.

**Messages, totals of the three sets per speed** (original / `2428ff6` / `4f21eb5`):

| Speed (signals) | Clean | Drift | Clock | Both |
|---|---|---|---|---|
| Normal (5,000) | 1,569 / 2,209 / 2,356 | 1,494 / 2,003 / 2,139 | 1,537 / 2,165 / 2,345 | 1,451 / 1,964 / 2,128 |
| Fast (6,960) | 2,331 / 3,368 / 4,838 | 2,231 / 3,284 / 4,758 | 2,061 / 3,114 / 4,697 | 2,012 / 3,046 / 4,623 |
| Turbo (5,520) | 1,743 / 2,581 / 3,522 | 1,721 / 2,552 / 3,552 | 1,251 / 1,930 / 3,062 | 1,229 / 1,916 / 3,030 |
| Slow (3,400) | 1,462 / 1,969 / 2,397 | 587 / 857 / 1,064 | 1,433 / 1,960 / 2,427 | 592 / 813 / 1,009 |

- **The gains over the original hold on harsher audio, and mostly grow.** `4f21eb5` against the original, clean → both: Normal speed 1.50 → 1.47 times, Fast 2.08 → 2.30, Turbo 2.02 → 2.47, Slow 1.64 → 1.70.
- **Clock offsets hurt the original most at Turbo** (-28%) and Fast (-12%); `4f21eb5` loses 13% and 3%. On clock-offset audio the start-time fix gains over `2428ff6`: Normal speed +8.3%, Fast +51%, Turbo +59%, Slow +24%.
- **Drift hurts Slow badly, every decoder:** the original -60%, `4f21eb5` -56%. A Slow frame lasts 25.3 s, so 0.25 Hz/s moves the tones up to 6.3 Hz across it, two Slow tone spacings (3.125 Hz). At Normal speed the same rate costs 9% (`4f21eb5`), at Fast and Turbo almost nothing (shorter frames, wider tones). Drift following (lead 9 [S75]) now has a measured target: Slow speed.
- Wrong messages over the 48 sets: original 0, `2428ff6` 7, `4f21eb5` 4 (of about 72,000 messages), at the chance level.
- Not covered: drift that wanders rather than runs steadily; real band noise and interference; real recordings.

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
| S40 | Fading-condition tests, 2026-11-19: sets `scorecard\itu_mq`, `itu_lq`, `itu_mm`, `itu_mv`, `itu_lm`, `itu_ld`, `itu_hm`, `itu_hd` (seeds 61 to 68, generator option `fade=`) and `noise4`; script `.refine_reach.py` (`SETS=`, `BIN=`); decoders `scorecard\bin\dec-orig` (original) and the counting build of a9b146b (`.patch_refine.py` + `.patch_lasttry.py`) | Run and counted |
| S9 | Our fading test, 2026-11-19: `scorecard\synth_gen.cpp` (fading option), sets `scorecard\ffade` and `scorecard\fflat` with their `truth.tsv`, results `scorecard\records-ffade.tsv` and `records-fflat.tsv`, decoder at commit b85d0af | Run and counted |
| S41 | Stage check, 2026-11-19: patch `scorecard\.patch_stagediag.py` (prints candidates, fitted frequency and sync count, and pass results; COPY only), script `.stagediag_run.py`; 10 files at 0 dB, seed 77, `fade=3,2` and `fade=10,3` | Run and counted |
| S42 | mfsk-core 0.4.3, `src/fec/qra/fast_fading.rs` (GPL-3.0-or-later Rust port of WSJT-X Q65 `q65_intrinsics_fastfading`, Palermo and Taylor). https://docs.rs/crate/mfsk-core/0.4.3/source/src/fec/qra/fast_fading.rs | Read as a description of the method; nothing copied |
| S43 | T. Ferrett, M. C. Valenti, "Noncoherent Analog Network Coding using LDPC-coded FSK", arXiv 1703.01432, 2017; and "Noncoherent LDPC-Coded Physical-Layer Network Coding using Multitone FSK", arXiv 1801.10370, 2018 | 2017 read in full 2026-11-20 (PDF text); 2018 read for mapping, iteration and results |
| S44 | A. Guillén i Fàbregas, A. Grant, "Parameter Free Iterative Decoding Metrics for Non-Coherent Orthogonal Modulation", draft 2006. https://www.upf.edu/documents/221459052/223029041/letter_ncfsk.pdf | Read in full 2026-11-20 |
| S45 | Spread-reading test, 2026-11-19: patch `scorecard\.patch_spread.py` (settable by `JS8_SPREAD`; COPY only) on top of `.patch_refine.py` and `.patch_lasttry.py`; scripts `.refine_reach.py`, `.level_count.py`; new sets `scorecard\itu_hm_s`, `itu_ld_s`, `itu_hd_s` (seeds 71 to 73, 25 files per level, 0 to -12 dB) | Run and counted |
| S46 | Decode-stage test, 2026-11-19: patch `scorecard\.patch_spreadmode.py` (`JS8_SPREADMODE` bits 1, 2, 4; COPY only) on top of `.patch_spread.py`; scripts `.refine_reach.py`, `.level_count.py`; sets `itu_hm_s`, `itu_ld_s`, `itu_hd_s`, `noise4` | Run and counted |
| S52 | mfsk-core 0.4.3, `src/q65/search.rs` and `src/q65/sync_pattern.rs` (GPL Rust port of WSJT-X Q65). https://docs.rs/crate/mfsk-core/0.4.3/source/src/q65/search.rs | Read for the search method; nothing copied |
| S53 | J. Taylor et al., "Quick-Start Guide to Q65". https://wsjt.sourceforge.io/Q65_Quick_Start.pdf; WSJT-X User Guide 2.6.0. https://wsjt.sourceforge.io/wsjtx-doc/wsjtx-main-2.6.0.pdf | Search summaries only (submode use for Doppler spread) |
| S51 | Wide spread reading test, 2026-11-20: patch `scorecard\.patch_wide.py` (`JS8_SPWIDE`, `JS8_SPWG`, `JS8_SPK`, `JS8_SPLOG`; COPY only) on top of `.patch_lasttry.py`; scripts `.refine_reach.py`, `.level_count.py`, `.speed.py`; new fresh sets `scorecard\itu_hm_g`, `itu_ld_g` (seeds 85, 86, 25 files per level, 0 to -12 dB); decoders `scorecard\bin\dec-widetest` (test copy) with wrappers `dec-w0`, `dec-w2`, and `dec-wide` (built version) with wrapper `dec-widedeep` | Run, counted and timed |
| S50 | Last-try ordering test, 2026-11-20: patch `scorecard\.patch_lt.py` (`JS8_LT` 0 to 3; COPY only) on top of `.patch_lasttry.py`; script `.refine_reach.py`, `.speed.py`; decoders `scorecard\bin\dec-ltdemap` (built version) and wrappers `dec-iddeep`, `dec-ltdeep` | Run, counted and timed |
| S49 | Iterative demapping test, 2026-11-20: patch `scorecard\.patch_id.py` (`JS8_ID` 0 to 4, `JS8_IDIT` round cap, `JS8_IDSYNC` sync minimum, `JS8_IDLOG`; COPY only) on top of `.patch_lasttry.py`; scripts `.refine_reach.py`, `.level_count.py`, `.speed.py`; decoders `scorecard\bin\dec-idtest` (test copy) with wrappers `dec-id0` to `dec-id4`, `dec-id2s10`, `dec-id2s12`, `dec-id2i30`, and `dec-iddemap` (built version) | Run, counted and timed |
| S48 | Fast-fading fix at Fast and Turbo, 2026-11-20: counting copies (`.patch_lasttry.py`) of a962669 (before) and 5ad9bbc (after); scripts `.refine_reach.py`, `.level_count.py` (now honours `MODE`; before this it always decoded as Normal), `.speed.py`; generator compiled fresh from `scorecard\synth_gen.cpp` (the copy in `scorecard\bin` ignores `files=` and `levels=`); new sets `scorecard\bhm_f`, `bld_f`, `bhd_f` (Fast; seeds 91, 92, 95) and `chm_f`, `cld_f`, `chd_f` (Turbo; seeds 93, 94, 96), 25 files per level, 0 to -12 dB | Run, counted and timed |
| S54 | Slow-speed test, 2026-11-19: generator `scorecard\synth_gen.cpp` (new `speed=slow`: 3,840 samples per tone, 30 s, start 0.5 s, Slow sync pattern); sets `scorecard\eflat`, `efade`, `emm`, `ehm`, `ecrowd`, `enoise` (seeds 111 to 116); counting copies (`.patch_lasttry.py`) of 5ad9bbc (before) and 847f8aa (after); scripts `.refine_reach.py`, `.speed.py` (`MODE=E`); wrappers `scorecard\bin\dec-sl0`, `dec-sl1`, `dec-sl0d`, `dec-sl1d` | Run, counted and timed |
| S55 | Slow 10 Hz stage check, 2026-11-19: test-container copies only (nothing in the repo): 847f8aa with `scorecard\.patch_stagediag.py`; a true-frequency-and-time copy (`JS8_TRUTH`, `JS8_ORACLE`, `JS8_OTIME`, `JS8_NOGATE`; patch kept in Claude's scratchpad, not in `scorecard\`); counting copies (`.patch_lasttry.py`) with `kWideReach` 2, 4, 5, 6; fresh sets `/tmp/ehm2`, `/tmp/eld2` (seeds 121, 122, deleted with the container); scripts `.refine_reach.py`, `.level_count.py`, `.speed.py` (`MODE=E`); wrappers `scorecard\bin\dec-r2`, `dec-r4` | Run, counted and timed |
| S56 | WSJT-X `lib/qra/q65/q65.c`, `fadengauss.c`, `fadenlorentz.c`, `q65_subs.c`, `lib/q65_decode.f90`, GitHub mirror master (https://github.com/WSJTX/wsjtx/tree/master/lib/qra/q65); mfsk-core 0.4.3 `src/fec/qra/fading_tables.rs`, `src/fec/qra/q65.rs`, `src/q65/rx.rs` (end of the file as a summary only), `src/q65/protocol.rs`, `src/q65/search.rs` (https://docs.rs/crate/mfsk-core/0.4.3/source/) | Read for the fast-fading window width; research only, nothing copied |
| S57 | WSJT-X `lib/qra/q65/q65.f90` (subroutines `q65_dec0`, `q65_symspec`, `q65_dec_q3`, `q65_dec_q012`, `q65_ccf_22`), `lib/qra/q65/q65_loops.f90`, `lib/q65_decode.f90`, `lib/smo121.f90`, GitHub mirror master (https://github.com/WSJTX/wsjtx) | Read; research only, nothing copied |
| S58 | J. Taylor et al., "Quick-Start Guide to Q65". https://wsjt.sourceforge.io/Q65_Quick_Start.pdf | Search summary only |
| S59 | G. Jourdain, G. Tziritas, Signal Processing 1984 (https://www.csd.uoc.gr/~tziritas/papers/SP-1984.pdf) and ICASSP 1982 (https://www.csd.uoc.gr/~tziritas/papers/ICASSP-1982.pdf); R. Kurth, MIT 1969 (filter-squarer-integrator receiver for Doppler-spread channels); R. S. Kennedy, "Fading Dispersive Communication Channels" (not found); searches on noncoherent MFSK timing and frequency estimation under Doppler spread, 2026-11-20 | Search summaries and abstracts only |
| S60 | Fit smoothing and weak-sync rescue test, 2026-11-20: test-container copies of 3fbf2c3 + `.patch_lasttry.py` with `patch_fsmo.py` (`JS8_FSMO`, `JS8_FSMOSPEED`), `patch_fgate.py` (`JS8_FGATE`, `JS8_FLOG`), `patch_wsync.py` (`JS8_WSYNC`, `JS8_WLOG`) and `patch_oracle.py` (true frequency and time), kept in Claude's scratchpad; sets as in S54 plus `/tmp/ehm2`, `/tmp/eld2`, `/tmp/ehm3`, `/tmp/eld3` (seeds 121, 122, 131, 132; deleted with the container), the Normal-speed sets, `itu_*_s/_f/_g`, Fast and Turbo sets; scripts `.refine_reach.py`, `.level_count.py`, `.speed.py`; wrappers `scorecard\bin\dec-cb0`, `dec-cb1`, `dec-cb5` | Run, counted and timed |
| S61 | Retry design test, 2026-11-20: test-container copy of 3fbf2c3 + `.patch_lasttry.py` + `patch_fsmo.py`, `patch_fgate.py`, `patch_wsync.py`, `patch_fretry.py` (`JS8_FRETRY`; Claude's scratchpad); sets as in S60; original decoder `scorecard\bin\dec-orig` for the time ratio; wrappers `scorecard\bin\dec-rt0`, `dec-rt5`, `dec-rta`, `dec-rt3` | Run, counted and timed |
| S62 | Original-vs-ours test, 2026-11-20: `scorecard\bin\dec-orig` (cba17f3) against 798e6ce (Normal and `--deep`) on 44 sets, every speed; scripts `scorecard\.level_count.py` (counts), `.speed.py` (crowded time, one file at a time); results page `docs\decoder-vs-original.html` | Run, counted and timed |
| S63 | Deep retry timing and skip test, 2026-11-20: timed copy (`patch_pt2.py`, `patch_same.py`, `patch_same2.py`, Claude's scratchpad; time per pass, first attempt against retry, retries landing on the first attempt's fit); counting copies (`.patch_lasttry.py`) of 798e6ce and the change; `.refine_reach.py`, `.cmp_exact.py`, `.speed.py` on the Normal- and Slow-speed sets | Run, counted and timed |
| S64 | Give-up rule test, 2026-11-20: logging copy (`patch_bplog.py`, Claude's scratchpad: failed checks per round for every decoding call, 134,512 calls, Deep, `crowd60 ffade2 fflat2 ecrowd efade`); rule copy (`patch_gu.py`: `JS8_GU_R`, `JS8_GU_T`) + `.patch_lasttry.py`; `.refine_reach.py`, `.speed.py`. Paper text extracted locally from the saved PDF of [S21] | Run, counted and timed |
| S65 | Damping test, 2026-11-20: copy with `patch_damp.py` (Claude's scratchpad; `JS8_DAMP` = γ, both message directions) + `.patch_lasttry.py`; every Normal-, Slow-, Fast- and Turbo-speed set; fresh sets made for this test, seeds 151 to 155 (`/tmp` in the test container, not kept: fading 1 Hz/2 ms, fade-free, 10 Hz/3 ms, 60 signals per file, Fast fading); `.refine_reach.py`, `.speed.py`. Damping form read from [S23] section II-A (ar5iv HTML) | Run, counted and timed |
| S66 | M. Fossorier, R. Palanki, J. Yedidia, "Iterative Decoding of Multi-step Majority Logic Decodable Codes", MERL TR2003-107 (Turbo Codes symposium 2003), https://www.merl.com/publications/docs/TR2003-107.pdf ; revised as R. Palanki, M. Fossorier, J. Yedidia, IEEE Transactions on Communications, vol. 55, no. 6, June 2007, pages 1099 to 1102, MERL TR2007-038, https://www.merl.com/publications/docs/TR2007-038.pdf | Read in full (both) |
| S67 | M. G. Stepanov, M. Chertkov, "Improving Convergence of Belief Propagation Decoding", arXiv cs/0607112, 2006. https://ar5iv.arxiv.org/html/cs/0607112 | Text read in full; figures are images, captions and text only |
| S68 | M. Pretti, "Lowering the error floor of Gallager codes: a statistical-mechanical view", arXiv 1407.0521, 2014. https://ar5iv.labs.arxiv.org/html/1407.0521 | Text read in full |
| S69 | V. Garcia Satorras, M. Welling, "Neural Enhanced Belief Propagation on Factor Graphs", AISTATS 2021, PMLR vol. 130. https://proceedings.mlr.press/v130/garcia-satorras21a/garcia-satorras21a.pdf | Read in full |
| S70 | WSJT-X 2.7.0 (Debian wsjtx 2.7.0+repack-1), `lib/ft8/bpdecode174_91.f90` and `lib/ft8/decode174_91.f90`. https://sources.debian.org/src/wsjtx/2.7.0%2Brepack-1/lib/ft8/bpdecode174_91.f90 | Both files read in full; `platanh` and `osd174_91` not read |
| S71 | Damping build check, 2026-11-20: working tree with `kDamping = 0.1f` in `belief_propagation` + `.patch_lasttry.py` (counting), the same without the patch (tests, timing), and HEAD 7850950 (before damping, timing); `.refine_reach.py` on all six groups in both modes, `.speed.py` on `bcrowd` (`MODE=B`) and `ccrowd` (`MODE=C`) with `bin/dec-orig`; ctest 19 tests | Run, counted and timed |
| S72 | Original-vs-ours re-run, 2026-11-20: 2428ff6 (Normal and `--deep`) on the 44 sets of [S62] with `.level_count.py` (via a copy of the S62 script without the original); crowded time `.speed.py` on `crowd60`, `bcrowd`, `ccrowd`, `ecrowd` with `bin/dec-orig` | Run, counted and timed |
| S47 | Built spread reading, 2026-11-19: patch `scorecard\.patch_spreadgate.py` (`JS8_SPMIN`, `JS8_SPSYNC`; COPY only), `.patch_lasttry.py` (now also for 11 passes); scripts `.refine_reach.py`, `.speed.py`; fresh sets `scorecard\itu_hm_f`, `itu_ld_f` (seeds 81, 82); decoders `scorecard\bin\dec-gap25` (before), `dec-spread` (first version), `dec-spread2` (built) | Run, counted and timed |
| S73 | T. Sokcevic (CE3TSK), "Deterministic Ensemble Decoding for FT8 and FT4: Re-Reading the Same Audio, and the Runtime Split That Pays for It", August 2026. https://ce3tsk.com/download/JTDX_CONTEST_DECODER_PAPER.pdf (derivative of JTDX, GPL) | Read in full (text extracted locally with pdftotext) |
| S74 | F. Krieg, J. Clausius, M. Geiselhart, S. ten Brink, "A Comparative Study of Ensemble Decoding Methods for Short Length LDPC Codes", arXiv 2410.23980, 2024. https://arxiv.org/html/2410.23980 | Text read in full; gains are in figures only |
| S75 | LogLab (K9KSU), pull request #71, "FT8 decoder: follow drift, validate by differential coherence, gate deep decodes by evidence". https://github.com/ckarc/LogLab/pull/71 (AGPL-3.0) | Description and commit messages read; code and linked write-up (404) not read |
| S76 | LogLab pull requests #68, "FT8 decoder stage 3, a coherent lock and a phase-coherence check", and #73, "classify the channel, combine symbols to suit it, and lock through fading". https://github.com/ckarc/LogLab/pull/68 , https://github.com/ckarc/LogLab/pull/73 (AGPL-3.0) | Descriptions and commit messages read; code not read |
| S77 | A. I. Vila Casado, M. Griot, R. D. Wesel, "Informed Dynamic Scheduling for Belief-Propagation Decoding of LDPC Codes", arXiv cs/0702111, 2007. https://ar5iv.labs.arxiv.org/html/cs/0702111 | Text read in full; figures not read |
| S78 | WSJT-X user guide, `doc/user_guide/en/decoder_notes.adoc` (AP decoding), GitHub mirror master. https://raw.githubusercontent.com/WSJTX/wsjtx/master/doc/user_guide/en/decoder_notes.adoc | Read in full |
| S79 | Search summaries only, 2026-11-20: S. Franke, J. Taylor, JT65 soft-decision decoder, QEX May/June 2016; C. Yue et al., "A Revisit to Ordered Statistics Decoding", arXiv 2004.04913; C. Herzet, N. Noels et al., "Code-Aided Turbo Synchronization", Proc. IEEE 2007; T. Hehn et al., multiple-bases BP, arXiv 0905.0079; BP-LED, arXiv 1705.09507; A. Buchberger et al., pruned neural BP, arXiv 2001.07464; A. Savard group, LDPC under impulsive noise (WCNC 2019, PIMRC 2018); ft8coder 0.5.0 documentation (docs.rs); Rhizomatica hermes-radio-daemon pull request #74; arXiv 2003.11233 (CRC-aided hybrid decoding, OSD distance threshold); MIL-STD-188-141 ALE summaries | Search summaries only |
| S80 | L. Quesada, "The LQ Digital Mode Family: Protocol Architecture and Reference Specification for Weak-Signal Communications", arXiv 2609.06427v4, 2026 | Receiver sections searched in the extracted text |
| S81 | Fit tests, 2026-11-20: copies of 4124f83 with `scorecard\.patch_oracle2.py` (`JS8_TRUTH`, `JS8_ORACLE`, `JS8_OTIME`: true frequency and start time) and `scorecard\.patch_fitdiag.py` (`JS8_FSTEP` frequency step, `JS8_TWIN` time window, `JS8_REFIT` 1 = time again then frequency again, 2 = WSJT-X version), plus `.patch_lasttry.py` for counting; `scorecard\.orpar.py` (true-signal runs), `.refine_reach.py` (`SETS=`, `MODE=`) on the Normal-speed, Fast, Turbo, Slow and 10 Hz sets, `.speed.py` on `crowd60`, `bcrowd`, `ccrowd`, `ecrowd` with `bin/dec-orig` | Run, counted and timed |
| S82 | WSJT-X `lib/ft8/ft8b.f90`, GitHub mirror master (https://raw.githubusercontent.com/WSJTX/wsjtx/master/lib/ft8/ft8b.f90), lines 102 to 152 (time search, frequency search, second time search) | Read; research only, nothing copied |
| S83 | WSJT-X `lib/ft8/sync8.f90`, GitHub mirror master (https://raw.githubusercontent.com/WSJTX/wsjtx/master/lib/ft8/sync8.f90), lines 28 to 50 and 123 (candidate spectra: one symbol of audio in a two-symbol FFT; candidate time) | Read; research only, nothing copied |
| S84 | Heartbeat-hint test, 2026-11-20: generator copy `scorecard\.patch_hbgen.py` (`heartbeat` mode; seeds 9101 Normal speed, 9121 Fast, 9122 Turbo, 9123 Slow; noise 9102, 9103 (2,000 files), 9211, 9212, 9213); decoder copy of both fit fixes plus `scorecard\.patch_hbhint.py` (`JS8_HBHINT`; prints `HREACH`, `HOK`); runner `scorecard\.hb_run.py` (`MODE=`, `SETTINGS=`, `NOISE_ONLY=`); crowded time on the first 5 files of `crowd60`, `bcrowd`, `ccrowd`, `ecrowd` with wrappers `scorecard\bin\dec-hb1`, `dec-hb2`, `dec-hb1d`, `dec-hb2d` | Run, counted and timed |
| S85 | Time-fit build check, 2026-11-21: `scorecard\.wrong_list.py` (each wrong message with its result line) on the Slow sets and fresh `eflat3`, `efade3`, `emm3`, `ecrowd3`, `enoise3` (seeds 131 to 135) with `.refine_reach.py`; 4f21eb5's working tree built and ctest (19 tests); counting copy (`.patch_lasttry.py`) on all five groups in both modes; `scorecard\.vs44.py` (the 44 sets of [S62], both modes); `.speed.py` on `crowd60`, `bcrowd`, `ccrowd`, `ecrowd` with `bin/dec-orig` | Run, counted and timed |
