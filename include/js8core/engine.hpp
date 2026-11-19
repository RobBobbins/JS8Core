#pragma once

#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

#include "js8core/audio.hpp"
#include "js8core/clock.hpp"
#include "js8core/logger.hpp"
#include "js8core/network.hpp"
#include "js8core/protocol/varicode.hpp"
#include "js8core/rig.hpp"
#include "js8core/storage.hpp"
#include "js8core/types.hpp"

namespace js8core {

// Events reported by the engine. Consumers receive these through
// EngineCallbacks::on_event. The variant order is part of the public interface
// and must not change.
namespace events {

// A decode pass has begun for the given submode mask.
struct DecodeStarted {
  int submodes = 0;
};

// The decoder is about to scan a window of the capture ring.
struct SyncStart {
  int position = 0;
  int size = 0;
};

// Progress information from the synchronization search.
struct SyncState {
  enum class Kind { Candidate, Decoded } kind = Kind::Candidate;
  int mode = 0;
  float frequency = 0.0f;
  float dt = 0.0f;
  union {
    int candidate;
    float decoded;
  } sync {0};
};

// A fully decoded frame.
struct Decoded {
  int utc = 0;
  int snr = 0;
  float xdt = 0.0f;
  float frequency = 0.0f;
  std::string data;
  int type = 0;
  float quality = 0.0f;
  int mode = 0;
  // Candidate cycle-clock correction calculated from the received frame.
  int drift_ms = 0;
  // Ring alignment (ms) at capture time. The signal is on time with
  // capture_drift_ms - 1000 * xdt.
  std::int64_t capture_drift_ms = 0;
};

// Summary emitted after a decode pass.
struct DecodeFinished {
  std::size_t decoded = 0;
};

// A UI-oriented spectrum snapshot.
struct Spectrum {
  std::vector<float> bins;
  float bin_hz = 0.0f;
  float power_db = 0.0f;
  float peak_db = 0.0f;
};

using Variant = std::variant<DecodeStarted, SyncStart, SyncState, Decoded, DecodeFinished, Spectrum>;

}  // namespace events

// Transmit output channel layout. Mono opens a 1-channel output; the others
// open 2 channels. Left and Right put the signal on one channel and silence on
// the other. Any other value opens 2 channels and behaves like Both.
inline constexpr int kTxChannelMono = 0;
inline constexpr int kTxChannelLeft = 1;
inline constexpr int kTxChannelRight = 2;
inline constexpr int kTxChannelBoth = 3;

struct EngineConfig {
  int sample_rate_hz = 0;
  int submodes = 0;
  int tx_output_rate_hz = 48000;
  float tx_output_gain = 1.0f;
  bool tx_output_gain_boost_enabled = false;
  int tx_output_channel = kTxChannelMono;
};

// One frame of the message being transmitted: the 12-character packed payload
// and its frame-type flags (1 = first frame, 2 = last frame, 4 = fast data).
struct TxFrameInfo {
  std::string frame;
  int bits = 0;
};

// Received audio just written into the 60-second capture ring.
struct CaptureTap {
  // Mono, signed 16-bit, at the ring rate (12000 Hz).
  std::span<const std::int16_t> samples;
  // Ring position of samples[0]. Sample n sits at (ring_index + n) % ring_size.
  std::size_t ring_index = 0;
  // Ring length in samples. Position 0 is the start of a UTC minute.
  std::size_t ring_size = 0;
};

// Request to encode and transmit a user message.
struct TxMessageRequest {
  std::string text;
  std::string my_call;
  std::string my_grid;
  std::string selected_call;
  int submode = 0;
  double audio_frequency_hz = 0.0;
  double tx_delay_s = 0.0;
  bool force_identify = false;
  bool force_data = false;
};

// Request to transmit a single already-built 12-character frame.
struct TxFrameRequest {
  std::string frame;
  int bits = 0;
  int submode = 0;
  double audio_frequency_hz = 0.0;
  double tx_delay_s = 0.0;
};

struct EngineCallbacks {
  std::function<void(events::Variant const&)> on_event;
  std::function<void(std::string_view message)> on_error;
  std::function<void(LogLevel level, std::string_view message)> on_log;
  // Optional. Called on the capture thread for every captured buffer, after
  // its samples (and any queued test audio) are in the ring and before decode
  // scheduling. Return quickly and copy what you need. Do not call
  // submit_capture() or queue_test_audio() from inside it.
  std::function<void(CaptureTap const&)> on_capture;
};

// Platform services supplied by the host application. Any of these may be
// null when the corresponding capability is not needed.
struct EngineDependencies {
  AudioInput* audio_in = nullptr;
  AudioOutput* audio_out = nullptr;
  RigControl* rig = nullptr;
  Scheduler* scheduler = nullptr;
  Storage* storage = nullptr;
  Logger* logger = nullptr;
  UdpChannel* udp = nullptr;
};

// The JS8 protocol engine: capture conditioning, slot scheduling, decoding,
// transmission, and spectrum reporting.
class Js8Engine {
public:
  virtual ~Js8Engine() = default;

  virtual bool start() = 0;
  virtual void stop() = 0;

  // Feed captured audio directly. Adapters that manage their own capture can
  // use this instead of an AudioInput dependency.
  virtual bool submit_capture(AudioInputBuffer const& buffer) = 0;

  virtual bool transmit_message(TxMessageRequest const& request) = 0;
  virtual bool transmit_frame(TxFrameRequest const& request) = 0;
  virtual bool start_tune(double audio_frequency_hz, int submode, double tx_delay_s) = 0;
  virtual void stop_transmit() = 0;
  virtual bool is_transmitting() const = 0;
  virtual bool is_transmitting_audio() const = 0;
  virtual int tx_milliseconds_until_audio() const = 0;
  virtual void set_tx_ready(bool ready) = 0;
  virtual void set_tx_boost_enabled(bool enabled) = 0;
  virtual void set_vox_lead_ms(int milliseconds) = 0;
  virtual void set_submodes(int submodes) = 0;
  virtual void set_sync_stats(bool enabled) = 0;
  // Deep decoding (default off): also run the decoder steps that find more
  // weak signals at a measured higher risk of wrong messages, and take more
  // time. Applies from the next decode.
  virtual void set_deep_decoding(bool enabled) = 0;

  // Callsign hints. A weak directed frame that fails normal decoding is tried
  // again with its callsigns assumed: to this callsign, and from a station
  // heard in the last kHintHeardMinutes. The engine collects heard stations
  // from decoded frames by itself. Empty, or a callsign that does not fit a
  // directed frame (a compound callsign such as "VE3/N0OWN"), turns the
  // own-callsign hints off. Applies from the next decode.
  static constexpr int kHintHeardMinutes = 10;
  virtual void set_my_call(std::string const& callsign) = 0;

  // Positive means the engine clock is ahead of the system clock. The new
  // value takes effect at the next captured buffer.
  virtual void set_time_drift_ms(std::int64_t drift_ms) = 0;
  virtual std::int64_t time_drift_ms() const = 0;

  // Transmit level (linear, clamped to 0.0 to 1.0) and channel layout
  // (kTxChannel*). The level and Left/Right/Both apply from the next output
  // buffer. A change between Mono and a 2-channel layout applies the next time
  // the output is opened (after stop_transmit()).
  virtual void set_tx_output(float gain, int channel) = 0;

  // Number of frames of the current message handed to the tone generator,
  // including the one waiting for its period or playing. 0 for tune, for
  // transmit_frame(), after stop_transmit() and before any message. Kept after
  // the message finishes on its own, until the next transmit or stop.
  virtual int tx_frames_started() const = 0;

  // Every frame of the current message, in order. Same lifetime as
  // tx_frames_started(): empty for tune, transmit_frame(), after
  // stop_transmit() and before any message.
  virtual std::vector<TxFrameInfo> tx_message_frames() const = 0;

  // Tone numbers (0 to 7) of the frame last loaded into the tone generator.
  // Empty for tune, before the first frame and after stop_transmit().
  virtual std::vector<int> tx_current_tones() const = 0;

  // Typeahead. Replaces the frames of the current message not yet started with
  // request.text, sent as data frames. frames_started_seen must equal
  // tx_frames_started(); otherwise a frame has started since and this returns
  // false. Also false when no message is being sent or while tuning. Empty
  // text ends the message after the frame already started.
  virtual bool replace_unsent_text(TxMessageRequest const& request,
                                   int frames_started_seen) = 0;

  // Test use. Queues mono 16-bit samples at the ring rate (12000 Hz). Each
  // captured buffer then has its ring samples overwritten (not mixed) by the
  // same number of queued samples, so test audio plays in real time and
  // reaches the decoder, spectrum and on_capture as if received. The queue
  // has no size limit and is not cleared by stop().
  virtual void queue_test_audio(std::span<const std::int16_t> samples) = 0;
};

std::unique_ptr<Js8Engine> make_engine(EngineConfig const& config,
                                       EngineCallbacks callbacks,
                                       EngineDependencies deps);

}  // namespace js8core
