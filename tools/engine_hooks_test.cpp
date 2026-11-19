// Tests for the host hooks: capture tap, test audio, TX output channel and
// level, frames-started counter, frame list, current tones and typeahead.
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <span>
#include <string>
#include <vector>

#include "js8core/engine.hpp"

using namespace js8core;

namespace {

int failures = 0;

void check(bool ok, char const* what) {
  if (!ok) {
    ++failures;
    std::fprintf(stderr, "FAIL: %s\n", what);
  }
}

// Output whose buffers the test pulls by hand, so TX runs faster than real time.
class ManualOutput final : public AudioOutput {
 public:
  bool start(AudioStreamParams const& params, AudioOutputFill fill,
             AudioErrorHandler) override {
    format_ = params.format;
    if (format_.sample_rate == 0) format_.sample_rate = 12000;
    fill_ = std::move(fill);
    started_ = true;
    ++opens_;
    return true;
  }
  void stop() override {
    started_ = false;
    fill_ = nullptr;
  }

  // Pulls one buffer; returns interleaved samples.
  std::vector<std::int16_t> pull(std::size_t frames = 1200) {
    std::vector<std::int16_t> out(frames * static_cast<std::size_t>(format_.channels), 0);
    if (!fill_) return out;
    std::vector<std::byte> bytes(out.size() * sizeof(std::int16_t));
    AudioOutputBuffer buffer{std::span<std::byte>(bytes), format_, SteadyTimePoint{}};
    fill_(buffer);
    std::memcpy(out.data(), bytes.data(), bytes.size());
    return out;
  }

  int channels() const { return format_.channels; }
  bool started() const { return started_; }
  int opens() const { return opens_; }

 private:
  AudioFormat format_{};
  AudioOutputFill fill_;
  bool started_ = false;
  int opens_ = 0;
};

struct Rig {
  ManualOutput output;
  std::vector<CaptureTap> taps;
  std::vector<std::vector<std::int16_t>> tap_samples;
  std::unique_ptr<Js8Engine> engine;

  explicit Rig(int channel = kTxChannelMono, bool tap = true) {
    EngineConfig config;
    config.sample_rate_hz = 12000;
    config.tx_output_rate_hz = 12000;
    config.tx_output_channel = channel;
    EngineCallbacks callbacks;
    if (tap) {
      callbacks.on_capture = [this](CaptureTap const& t) {
        taps.push_back(t);
        tap_samples.emplace_back(t.samples.begin(), t.samples.end());
      };
    }
    EngineDependencies deps;
    deps.audio_out = &output;
    engine = make_engine(config, callbacks, deps);
  }
  ~Rig() { engine->stop(); }

  bool submit(std::vector<std::int16_t> const& interleaved, int channels = 1,
              SampleType type = SampleType::Int16) {
    AudioInputBuffer buffer;
    buffer.data = std::as_bytes(std::span<const std::int16_t>(interleaved));
    buffer.format = AudioFormat{12000, channels, type};
    return engine->submit_capture(buffer);
  }
};

TxMessageRequest message(std::string text) {
  TxMessageRequest request;
  request.text = std::move(text);
  request.my_call = "K1ABC";
  request.my_grid = "FN42";
  request.submode = 2;  // Turbo: 6 s periods keep the test short.
  request.audio_frequency_hz = 1500.0;
  return request;
}

bool any_nonzero(std::vector<std::int16_t> const& v, int channels, int ch) {
  for (std::size_t i = static_cast<std::size_t>(ch); i < v.size();
       i += static_cast<std::size_t>(channels)) {
    if (v[i] != 0) return true;
  }
  return false;
}

int peak(std::vector<std::int16_t> const& v) {
  int p = 0;
  for (auto s : v) p = std::max(p, std::abs(static_cast<int>(s)));
  return p;
}

// Pulls output until audio is playing (or gives up after ~20 s of samples).
std::vector<std::int16_t> pull_until_audio(Rig& rig) {
  for (int i = 0; i < 200; ++i) {
    auto buffer = rig.output.pull();
    if (peak(buffer) > 0) return buffer;
  }
  return {};
}

void test_capture_tap() {
  Rig rig;
  std::vector<std::int16_t> ones(1200, 1000);
  check(rig.submit(ones), "tap: first buffer accepted");
  check(rig.taps.size() == 1, "tap: one call per buffer");
  if (rig.taps.size() != 1) return;
  check(rig.tap_samples[0].size() == 1200, "tap: sample count matches buffer");
  check(rig.taps[0].ring_size == 720000, "tap: ring size is 60 s at 12 kHz");
  check(rig.taps[0].ring_index < 720000, "tap: ring index inside ring");

  check(rig.submit(ones), "tap: second buffer accepted");
  check(rig.taps.size() == 2, "tap: second call");
  if (rig.taps.size() == 2) {
    check(rig.taps[1].ring_index == (rig.taps[0].ring_index + 1200) % 720000,
          "tap: ring index advances by the buffer length");
  }

  // Stereo: first channel only (+2000 left, -2000 right).
  std::vector<std::int16_t> stereo(2400);
  for (std::size_t i = 0; i < stereo.size(); i += 2) {
    stereo[i] = 2000;
    stereo[i + 1] = -2000;
  }
  for (int i = 0; i < 3; ++i) rig.submit(stereo, 2);
  check(rig.taps.size() == 5, "tap: stereo buffers delivered");
  if (rig.taps.size() == 5) {
    auto const& last = rig.tap_samples.back();
    check(last.size() == 1200, "tap: stereo gives mono samples");
    long sum = 0;
    for (auto s : last) sum += s;
    check(sum / 1200 > 1000, "tap: stereo gives the first channel");
  }

  std::size_t const before = rig.taps.size();
  rig.submit({});
  check(rig.taps.size() == before, "tap: no call for an empty buffer");
  check(!rig.submit(ones, 1, SampleType::Float32), "tap: wrong sample type refused");
  check(rig.taps.size() == before, "tap: no call for a refused buffer");

  Rig quiet(kTxChannelMono, false);
  check(quiet.submit(ones), "tap: no listener, capture still works");
}

void test_test_audio() {
  Rig rig;
  std::vector<std::int16_t> test(1500, 1234);
  rig.engine->queue_test_audio(test);
  std::vector<std::int16_t> zeros(1200, 0);
  rig.submit(zeros);
  rig.submit(zeros);
  check(rig.tap_samples.size() == 2, "test audio: two buffers tapped");
  if (rig.tap_samples.size() != 2) return;
  check(std::all_of(rig.tap_samples[0].begin(), rig.tap_samples[0].end(),
                    [](std::int16_t s) { return s == 1234; }),
        "test audio: first buffer fully replaced");
  check(std::all_of(rig.tap_samples[1].begin(), rig.tap_samples[1].begin() + 300,
                    [](std::int16_t s) { return s == 1234; }),
        "test audio: rest of the queue used next buffer");
  check(std::all_of(rig.tap_samples[1].begin() + 300, rig.tap_samples[1].end(),
                    [](std::int16_t s) { return s == 0; }),
        "test audio: real audio after the queue runs out");

  // The queue survives stop and plays after the next start.
  std::vector<std::int16_t> more(100, -777);
  rig.engine->queue_test_audio(more);
  rig.engine->start();
  rig.engine->stop();
  rig.engine->start();
  rig.submit(zeros);
  check(rig.tap_samples.size() == 3 &&
            std::all_of(rig.tap_samples[2].begin(), rig.tap_samples[2].begin() + 100,
                        [](std::int16_t s) { return s == -777; }),
        "test audio: queue kept across stop and start");
}

void test_frames_and_tones() {
  Rig rig;
  rig.engine->start();
  check(rig.engine->tx_frames_started() == 0, "frames: 0 before any message");
  check(rig.engine->tx_message_frames().empty(), "frames: list empty before any message");
  check(rig.engine->tx_current_tones().empty(), "tones: empty before any frame");

  check(rig.engine->transmit_message(
            message("HELLO THIS IS A LONGER MESSAGE TO MAKE SEVERAL FRAMES")),
        "frames: message accepted");
  auto const list = rig.engine->tx_message_frames();
  int const total = static_cast<int>(list.size());
  check(total >= 3, "frames: message has several frames");
  check(rig.engine->tx_frames_started() == 1, "frames: counter is 1 at once");
  if (total >= 3) {
    check((list.front().bits & 1) != 0, "frames: first frame flagged first");
    check((list.back().bits & 2) != 0, "frames: last frame flagged last");
    check(std::all_of(list.begin(), list.end(),
                      [](TxFrameInfo const& f) { return f.frame.size() == 12; }),
          "frames: every frame is 12 characters");
  }
  auto tones = rig.engine->tx_current_tones();
  check(tones.size() == 79, "tones: 79 symbols");
  check(std::all_of(tones.begin(), tones.end(), [](int t) { return t >= 0 && t <= 7; }),
        "tones: values 0 to 7");

  // Run the whole message through the output.
  int last_seen = 1;
  bool monotonic = true;
  for (int i = 0; i < 4000 && rig.engine->is_transmitting(); ++i) {
    rig.output.pull();
    int const now = rig.engine->tx_frames_started();
    if (now < last_seen || now > last_seen + 1) monotonic = false;
    last_seen = now;
  }
  check(!rig.engine->is_transmitting(), "frames: message finished");
  check(monotonic, "frames: counter steps up one at a time");
  check(rig.engine->tx_frames_started() == total, "frames: counter ends at the frame count");
  check(static_cast<int>(rig.engine->tx_message_frames().size()) == total,
        "frames: list kept after the message finishes");

  rig.engine->stop_transmit();
  check(rig.engine->tx_frames_started() == 0, "frames: counter 0 after stop");
  check(rig.engine->tx_message_frames().empty(), "frames: list empty after stop");
  check(rig.engine->tx_current_tones().empty(), "tones: empty after stop");

  check(rig.engine->start_tune(1500.0, 2, 0.0), "tune accepted");
  check(rig.engine->tx_frames_started() == 0, "frames: counter 0 while tuning");
  check(rig.engine->tx_message_frames().empty(), "frames: list empty while tuning");
  check(rig.engine->tx_current_tones().empty(), "tones: empty while tuning");
  rig.engine->stop_transmit();

  if (total >= 1) {
    TxFrameRequest raw;
    raw.frame = list.front().frame;
    raw.bits = list.front().bits;
    raw.submode = 2;
    raw.audio_frequency_hz = 1500.0;
    check(rig.engine->transmit_frame(raw), "raw frame accepted");
    check(rig.engine->tx_frames_started() == 0, "frames: counter 0 for a raw frame");
    check(rig.engine->tx_message_frames().empty(), "frames: list empty for a raw frame");
    check(rig.engine->tx_current_tones().size() == 79, "tones: set for a raw frame");
    rig.engine->stop_transmit();
  }

  // A new message replaces the old one.
  rig.engine->transmit_message(message("FIRST MESSAGE WITH SOME LENGTH TO IT"));
  auto const long_size = rig.engine->tx_message_frames().size();
  rig.engine->transmit_message(message("HI"));
  check(rig.engine->tx_frames_started() == 1, "frames: counter restarts for a new message");
  check(rig.engine->tx_message_frames().size() < long_size,
        "frames: list replaced by new message");
  rig.engine->stop_transmit();
}

void test_typeahead() {
  Rig rig;
  rig.engine->start();
  auto request = message("THE QUICK BROWN FOX JUMPS OVER THE LAZY DOG AGAIN AND AGAIN");
  check(rig.engine->transmit_message(request), "typeahead: message accepted");
  int const started = rig.engine->tx_frames_started();

  auto replacement = message("NEW WORDS HERE");
  check(!rig.engine->replace_unsent_text(replacement, started + 1),
        "typeahead: refused when the counter does not match");
  check(rig.engine->replace_unsent_text(replacement, started),
        "typeahead: accepted when the counter matches");
  auto const list = rig.engine->tx_message_frames();
  check(static_cast<int>(list.size()) > started, "typeahead: new frames added");
  bool no_first = true;
  for (std::size_t i = static_cast<std::size_t>(started); i < list.size(); ++i) {
    if (list[i].bits & 1) no_first = false;
  }
  check(no_first, "typeahead: new frames never flagged first");
  check(!list.empty() && (list.back().bits & 2) != 0, "typeahead: new last frame flagged last");

  // Empty text: message ends after the frame already started.
  check(rig.engine->replace_unsent_text(message(""), started),
        "typeahead: empty text accepted");
  check(static_cast<int>(rig.engine->tx_message_frames().size()) == started,
        "typeahead: empty text leaves only started frames");
  for (int i = 0; i < 4000 && rig.engine->is_transmitting(); ++i) rig.output.pull();
  check(!rig.engine->is_transmitting(), "typeahead: message ends after current frame");
  check(rig.engine->tx_frames_started() == started, "typeahead: no further frame started");
  check(!rig.engine->replace_unsent_text(replacement, started),
        "typeahead: refused after the message finished");

  rig.engine->stop_transmit();
  check(!rig.engine->replace_unsent_text(replacement, 0), "typeahead: refused when idle");
  rig.engine->start_tune(1500.0, 2, 0.0);
  check(!rig.engine->replace_unsent_text(replacement, 0), "typeahead: refused while tuning");
  rig.engine->stop_transmit();
}

void test_output_channel() {
  {
    Rig rig;  // Default: Mono.
    rig.engine->start();
    rig.engine->transmit_message(message("HI"));
    check(rig.output.channels() == 1, "channel: Mono opens 1 channel");
    check(peak(pull_until_audio(rig)) > 0, "channel: Mono plays");
    rig.engine->stop_transmit();
  }
  {
    Rig rig(kTxChannelLeft);
    rig.engine->start();
    rig.engine->transmit_message(message("HI"));
    check(rig.output.channels() == 2, "channel: Left opens 2 channels");
    auto b = pull_until_audio(rig);
    check(any_nonzero(b, 2, 0) && !any_nonzero(b, 2, 1), "channel: Left only on first");

    rig.engine->set_tx_output(1.0f, kTxChannelRight);
    b = rig.output.pull();
    check(!any_nonzero(b, 2, 0) && any_nonzero(b, 2, 1),
          "channel: switch to Right applies on next buffer");

    rig.engine->set_tx_output(1.0f, kTxChannelBoth);
    b = rig.output.pull();
    bool same = true;
    for (std::size_t i = 0; i < b.size(); i += 2) same = same && b[i] == b[i + 1];
    check(same && any_nonzero(b, 2, 0), "channel: Both puts the signal on both");

    int const full = peak(b);
    rig.engine->set_tx_output(0.5f, kTxChannelBoth);
    int const half = peak(rig.output.pull());
    check(half > full * 4 / 10 && half < full * 6 / 10, "level: 0.5 halves the output");

    rig.engine->set_tx_output(0.0f, kTxChannelBoth);
    check(peak(rig.output.pull()) == 0, "level: 0 gives silence");

    rig.engine->set_tx_output(5.0f, kTxChannelMono);
    b = rig.output.pull();
    check(rig.output.channels() == 2 && rig.output.opens() == 1,
          "channel: Mono mid-transmit keeps the open stream");
    check(peak(b) <= full + 1, "level: above 1 is clamped");
    rig.engine->stop_transmit();

    rig.engine->transmit_message(message("HI"));
    check(rig.output.channels() == 1, "channel: Mono applies at the next open");
    rig.engine->stop_transmit();
  }
  {
    Rig rig(7);
    rig.engine->start();
    rig.engine->transmit_message(message("HI"));
    check(rig.output.channels() == 2, "channel: unknown value opens 2 channels");
    auto b = pull_until_audio(rig);
    check(any_nonzero(b, 2, 0) && any_nonzero(b, 2, 1), "channel: unknown value acts as Both");
    rig.engine->stop_transmit();
  }
}

}  // namespace

int main() {
  test_capture_tap();
  test_test_audio();
  test_frames_and_tones();
  test_typeahead();
  test_output_channel();
  if (failures) {
    std::fprintf(stderr, "%d check(s) failed\n", failures);
    return 1;
  }
  std::printf("engine hooks: all checks passed\n");
  return 0;
}
