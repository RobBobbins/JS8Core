// Callsign hints: the engine's heard-station list, and weak directed frames
// to a known callsign decoding more often with hints, with no wrong frame on
// unknown stations or on noise. Synthetic Normal-speed audio, fixed seeds.
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <random>
#include <set>
#include <string>
#include <vector>

#include "js8core/decoder.hpp"
#include "js8core/protocol/costas.hpp"
#include "js8core/protocol/varicode.hpp"
#include "../src/engine/callsign_hints.hpp"

using namespace js8core;
namespace varicode = protocol::varicode;

namespace {

constexpr int kRate = 12000;
constexpr int kSymbolSamples = 1920;
constexpr double kNoiseSigma = 300.0;
constexpr double kPi = 3.14159265358979323846;

// Directed frame as sent: type 3, from, to, command 31 (free text), number 0.
std::string directed_frame(std::string const& from, std::string const& to) {
  std::uint64_t const value =
      (std::uint64_t{3} << 61) |
      (std::uint64_t{varicode::pack_callsign(from, nullptr)} << 33) |
      (std::uint64_t{varicode::pack_callsign(to, nullptr)} << 5) | 31u;
  return varicode::pack72bits(value, 0);
}

struct Signal {
  std::string frame;
  double frequency;
};

// Gaussian noise from mt19937 (Box-Muller), the same on every standard library.
class Noise {
 public:
  explicit Noise(unsigned seed) : rng_(seed) {}
  double operator()() {
    double const u1 = (rng_() + 1.0) / 4294967297.0;
    double const u2 = (rng_() + 1.0) / 4294967297.0;
    return kNoiseSigma * std::sqrt(-2.0 * std::log(u1)) * std::cos(2.0 * kPi * u2);
  }

 private:
  std::mt19937 rng_;
};

// 15 seconds of audio with the signals at `snr` dB in 2500 Hz, decoded once
// without and once with hints. Returns the decoded frame texts of each run.
std::pair<std::set<std::string>, std::set<std::string>> decode_both(
    std::vector<Signal> const& signals, double snr, unsigned seed,
    std::uint32_t own, std::vector<std::uint32_t> const& heard) {
  Noise noise(seed);
  std::vector<double> mix(15 * kRate);
  for (auto& v : mix) v = noise();
  double const amp = std::sqrt(2.0 * kNoiseSigma * kNoiseSigma * (2500.0 / 6000.0) *
                               std::pow(10.0, snr / 10.0));
  auto const& costas = protocol::costas(protocol::CostasType::Original);
  for (auto const& signal : signals) {
    int tones[79];
    legacy_encode(3, costas, signal.frame.c_str(), tones);
    double phase = 0.0;
    int const start = kRate / 2 + 2400;
    for (int symbol = 0; symbol < 79; ++symbol) {
      double const step = 2.0 * kPi * (signal.frequency + tones[symbol] * 6.25) / kRate;
      for (int i = 0; i < kSymbolSamples; ++i) {
        mix[static_cast<std::size_t>(start + symbol * kSymbolSamples + i)] += amp * std::sin(phase);
        phase += step;
      }
    }
  }

  DecodeState state;
  state.samples.assign(static_cast<std::size_t>(kJs8NtMax * kJs8RxSampleRate), 0);
  for (std::size_t i = 0; i < mix.size(); ++i) {
    state.samples[i] = static_cast<std::int16_t>(std::lround(std::fmax(-32768.0, std::fmin(32767.0, mix[i]))));
  }
  state.params.nfa = 200;
  state.params.nfb = 2500;
  state.params.nfqso = 1500;
  state.params.nsubmodes = 1 << 0;
  state.params.kposA = 0;
  state.params.kszA = 15 * kRate;

  auto run = [&] {
    std::set<std::string> texts;
    legacy_decode(state, [&](events::Variant const& event) {
      if (auto const* decoded = std::get_if<events::Decoded>(&event)) texts.insert(decoded->data);
    });
    return texts;
  };
  auto const plain = run();
  state.hint_own_call = own;
  state.hint_heard_calls = heard;
  auto const hinted = run();
  return {plain, hinted};
}

// The engine's heard-station list and own callsign.
int test_heard_list() {
  int failures = 0;
  auto check = [&](bool ok, char const* what) {
    if (!ok) {
      ++failures;
      std::printf("FAIL: %s\n", what);
    }
  };
  auto decoded = [](std::string frame, int type) {
    events::Decoded d;
    d.data = std::move(frame);
    d.type = type;
    return d;
  };
  auto packed = [](char const* call) { return varicode::pack_callsign(call, nullptr); };
  using Clock = CallsignHints::Clock;
  Clock::time_point const t0{};

  CallsignHints hints;
  DecodeState task;
  hints.set_own("N0OWN");
  hints.fill(task, t0);
  check(task.hint_own_call == packed("N0OWN"), "own callsign packed");
  check(task.hint_heard_calls.empty(), "nothing heard yet");
  hints.set_own("VE3/N0OWN");
  hints.fill(task, t0);
  check(task.hint_own_call == 0, "compound own callsign turns own hints off");
  hints.set_own("");
  hints.fill(task, t0);
  check(task.hint_own_call == 0, "empty own callsign turns own hints off");

  hints.note(decoded(directed_frame("W2XYZ", "N0OWN"), 3), t0);
  hints.note(decoded(varicode::pack_compound_frame("K1ABC", 0, 0, 0), 3), t0);
  hints.note(decoded(directed_frame("G4DEF", "N0OWN"), 7), t0);  // data flag
  hints.note(decoded(directed_frame("<....>", "N0OWN"), 3), t0);  // placeholder
  hints.fill(task, t0);
  check(task.hint_heard_calls.size() == 2, "directed and heartbeat senders kept, data and placeholder ignored");
  check(task.hint_heard_calls.size() == 2 && task.hint_heard_calls[0] == packed("K1ABC") &&
            task.hint_heard_calls[1] == packed("W2XYZ"),
        "newest first");

  hints.note(decoded(directed_frame("W2XYZ", "K1ABC"), 1), t0 + std::chrono::minutes(1));
  hints.fill(task, t0 + std::chrono::minutes(1));
  check(task.hint_heard_calls.size() == 2 && task.hint_heard_calls[0] == packed("W2XYZ"),
        "heard again moves to the front without a duplicate");

  hints.fill(task, t0 + std::chrono::minutes(Js8Engine::kHintHeardMinutes) + std::chrono::seconds(30));
  check(task.hint_heard_calls.size() == 1 && task.hint_heard_calls[0] == packed("W2XYZ"),
        "stations older than the window expire");

  for (int i = 0; i < 20; ++i) {
    std::string call = "K" + std::to_string(i % 10) + "A" + std::string(1, static_cast<char>('A' + i));
    hints.note(decoded(directed_frame(call, "N0OWN"), 3), t0 + std::chrono::minutes(2));
  }
  hints.fill(task, t0 + std::chrono::minutes(2));
  check(task.hint_heard_calls.size() == CallsignHints::kMaxHeard, "at most kMaxHeard stations");
  return failures;
}

}  // namespace

int main() {
  int const list_failures = test_heard_list();

  std::uint32_t const own = varicode::pack_callsign("N0OWN", nullptr);
  std::vector<std::uint32_t> heard;
  for (char const* call : {"K1ABC", "W2XYZ", "VE3QRS", "G4DEF", "JA1GHI", "DL2JKL"}) {
    heard.push_back(varicode::pack_callsign(call, nullptr));
  }

  int plain_hits = 0;
  int hinted_hits = 0;
  int wrong = 0;
  int lost = 0;
  int trials = 0;
  for (double snr : {-20.0, -21.0}) {
    for (unsigned seed = 1; seed <= 8; ++seed) {
      // A heard station calling me, and an unknown station calling another.
      std::vector<Signal> const signals = {
          {directed_frame("W2XYZ", "N0OWN"), 900.0},
          {directed_frame("KA9ZZZ", "WB8QQQ"), 1700.0},
      };
      auto const [plain, hinted] = decode_both(signals, snr, seed, own, heard);
      ++trials;
      plain_hits += static_cast<int>(plain.count(signals[0].frame));
      hinted_hits += static_cast<int>(hinted.count(signals[0].frame));
      for (auto const& text : hinted) {
        if (text != signals[0].frame && text != signals[1].frame) ++wrong;
      }
      for (auto const& text : plain) {
        if (!hinted.count(text)) ++lost;
      }
    }
  }

  int noise_decodes = 0;
  for (unsigned seed = 100; seed < 106; ++seed) {
    auto const [plain, hinted] = decode_both({}, 0.0, seed, own, heard);
    noise_decodes += static_cast<int>(hinted.size());
  }

  std::printf("frames to own call decoded: %d without hints, %d with hints, of %d\n",
              plain_hits, hinted_hits, trials);
  std::printf("wrong frames with hints: %d; frames lost with hints: %d; noise decodes: %d\n",
              wrong, lost, noise_decodes);
  bool const ok = list_failures == 0 && hinted_hits > plain_hits && wrong == 0 && lost == 0 &&
                  noise_decodes == 0;
  std::printf("%s\n", ok ? "PASS" : "FAIL");
  return ok ? 0 : 1;
}
