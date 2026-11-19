#include "ldpc.hpp"

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <numeric>
#include <string>

#include "decoder_tables.hpp"

namespace js8core::decoder {
namespace {

constexpr std::uint8_t kInvalidWord = 0xFF;

constexpr std::array<std::uint8_t, 256> build_word_table() {
  std::array<std::uint8_t, 256> table{};
  // Manual fill: std::array::fill is not constexpr in older NDK libc++.
  for (std::size_t i = 0; i < table.size(); ++i) table[i] = kInvalidWord;
  for (std::size_t i = 0; i < data::kAlphabet.size(); ++i) {
    table[static_cast<std::uint8_t>(data::kAlphabet[i])] =
        static_cast<std::uint8_t>(i);
  }
  return table;
}

constexpr auto kWordTable = build_word_table();

constexpr int hex_digit(char c) {
  if (c >= '0' && c <= '9') return c - '0';
  if (c >= 'a' && c <= 'f') return c - 'a' + 10;
  if (c >= 'A' && c <= 'F') return c - 'A' + 10;
  return 0;
}

// The generator parity rows are stored as hex text. Expand them into a packed
// bit matrix once at compile time.
constexpr std::size_t kParityWords =
    (data::kPayloadBits * data::kPayloadBits + 63) / 64;

constexpr std::array<std::uint64_t, kParityWords> build_parity_matrix() {
  std::array<std::uint64_t, kParityWords> matrix{};
  for (std::size_t row = 0; row < data::kPayloadBits; ++row) {
    std::size_t column = 0;
    for (char const c : data::kParityRows[row]) {
      int const value = hex_digit(c);
      for (int const mask : {8, 4, 2, 1}) {
        if (column >= data::kPayloadBits) break;
        if ((value & mask) != 0) {
          std::size_t const index = row * data::kPayloadBits + column;
          matrix[index / 64] |= (std::uint64_t{1} << (index % 64));
        }
        ++column;
      }
    }
  }
  return matrix;
}

constexpr auto kParityMatrix = build_parity_matrix();

constexpr int kMaxCheckDegree = 7;
constexpr int kMaxBitDegree = 3;

}  // namespace

std::uint8_t word_for(char c) {
  return kWordTable[static_cast<std::uint8_t>(c)];
}

std::uint16_t crc12(std::uint8_t const* data, std::size_t size) {
  std::uint16_t remainder = 0;
  for (std::size_t i = 0; i < size; ++i) {
    for (int bit = 7; bit >= 0; --bit) {
      bool const feedback = (remainder & 0x0800u) != 0;
      remainder = static_cast<std::uint16_t>(
          ((remainder << 1) | ((data[i] >> bit) & 1u)) & 0x0FFFu);
      if (feedback) remainder ^= 0x0C06u;
    }
  }
  return static_cast<std::uint16_t>(remainder ^ 42u);
}

bool crc_ok(std::array<std::int8_t, 87> const& payload) {
  std::array<std::uint8_t, 11> bytes{};
  for (std::size_t i = 0; i < payload.size(); ++i) {
    if (payload[i] != 0) {
      bytes[i / 8] |= static_cast<std::uint8_t>(1u << (7 - (i % 8)));
    }
  }

  std::uint16_t const received =
      static_cast<std::uint16_t>(((bytes[9] & 0x1Fu) << 7) | (bytes[10] >> 1));

  bytes[9] &= 0xE0u;
  bytes[10] = 0;

  return received == crc12(bytes.data(), bytes.size());
}

std::string decode_message(std::array<std::int8_t, 87> const& payload) {
  if (!crc_ok(payload)) return {};

  std::string text;
  text.reserve(data::kFrameCharacters);
  for (int word = 0; word < data::kFrameCharacters; ++word) {
    std::uint8_t value = 0;
    for (int bit = 0; bit < 6; ++bit) {
      value = static_cast<std::uint8_t>(
          (value << 1) | (payload[word * 6 + bit] != 0 ? 1u : 0u));
    }
    text.push_back(data::kAlphabet[value]);
  }
  return text;
}

int belief_propagation(std::array<float, 174> const& llr,
                       std::array<std::int8_t, 87>& payload,
                       std::array<std::int8_t, 174>& codeword,
                       int max_iterations, bool early_abort) {
  using namespace data;

  // Messages travelling from bits to checks, and from checks to bits.
  std::array<std::array<float, kMaxBitDegree>, kCodeLength> bit_to_check{};
  std::array<std::array<float, kMaxCheckDegree>, kParityBits> check_to_bit{};
  std::array<std::array<float, kMaxCheckDegree>, kParityBits> check_tanh{};
  std::array<float, kCodeLength> posterior{};
  std::array<int, kParityBits> syndrome{};

  int stalled = 0;
  int previous_failures = 0;

  for (int iteration = 0; iteration <= max_iterations; ++iteration) {
    // Posterior log-likelihood ratio for every coded bit.
    for (int bit = 0; bit < kCodeLength; ++bit) {
      float value = llr[static_cast<std::size_t>(bit)];
      for (int k = 0; k < kMaxBitDegree; ++k) {
        value += bit_to_check[static_cast<std::size_t>(bit)]
                              [static_cast<std::size_t>(k)];
      }
      posterior[static_cast<std::size_t>(bit)] = value;
    }

    for (int bit = 0; bit < kCodeLength; ++bit) {
      codeword[static_cast<std::size_t>(bit)] =
          posterior[static_cast<std::size_t>(bit)] > 0.0f ? 1 : 0;
    }

    int failed_checks = 0;
    for (int check = 0; check < kParityBits; ++check) {
      CheckNode const& node = kCheckNodes[static_cast<std::size_t>(check)];
      int parity = 0;
      for (int j = 0; j < node.count; ++j) {
        parity += codeword[static_cast<std::size_t>(node.bits[j])];
      }
      syndrome[static_cast<std::size_t>(check)] = parity;
      if ((parity & 1) != 0) ++failed_checks;
    }

    if (failed_checks == 0) {
      std::copy(codeword.begin() + kParityBits, codeword.end(),
                payload.begin());

      int errors = 0;
      for (int bit = 0; bit < kCodeLength; ++bit) {
        float const sign = (2 * codeword[static_cast<std::size_t>(bit)] - 1);
        if (sign * llr[static_cast<std::size_t>(bit)] < 0.0f) ++errors;
      }
      return errors;
    }

    if (iteration > 0) {
      int const delta = failed_checks - previous_failures;
      stalled = delta < 0 ? 0 : stalled + 1;
      if (early_abort && stalled >= 5 && iteration >= 10 && failed_checks > 15) return -1;
    }
    previous_failures = failed_checks;

    // Bit-to-check messages: posterior minus the incoming check message.
    for (int check = 0; check < kParityBits; ++check) {
      CheckNode const& node = kCheckNodes[static_cast<std::size_t>(check)];
      for (int j = 0; j < node.count; ++j) {
        int const bit = node.bits[j];
        float message = posterior[static_cast<std::size_t>(bit)];
        for (int k = 0; k < kMaxBitDegree; ++k) {
          if (kBitChecks[static_cast<std::size_t>(bit)]
                       [static_cast<std::size_t>(k)] == check) {
            message -= bit_to_check[static_cast<std::size_t>(bit)]
                                    [static_cast<std::size_t>(k)];
          }
        }
        check_to_bit[static_cast<std::size_t>(check)]
                    [static_cast<std::size_t>(j)] = message;
      }
    }

    for (int check = 0; check < kParityBits; ++check) {
      CheckNode const& node = kCheckNodes[static_cast<std::size_t>(check)];
      for (int j = 0; j < node.count; ++j) {
        check_tanh[static_cast<std::size_t>(check)]
                  [static_cast<std::size_t>(j)] = std::tanh(
                      -check_to_bit[static_cast<std::size_t>(check)]
                                   [static_cast<std::size_t>(j)] /
                      2.0f);
      }
    }

    // Check-to-bit messages: product of the other incoming tanh terms.
    for (int bit = 0; bit < kCodeLength; ++bit) {
      for (int k = 0; k < kMaxBitDegree; ++k) {
        int const check =
            kBitChecks[static_cast<std::size_t>(bit)]
                      [static_cast<std::size_t>(k)];
        CheckNode const& node = kCheckNodes[static_cast<std::size_t>(check)];
        float product = 1.0f;
        for (int j = 0; j < node.count; ++j) {
          if (node.bits[j] != bit) {
            product *= check_tanh[static_cast<std::size_t>(check)]
                                 [static_cast<std::size_t>(j)];
          }
        }
        // Saturated LLRs can yield tanh == +/-1 exactly. atanh(+/-1)
        // produces infinities which poison subsequent message passes.
        constexpr float kTanhLimit = 1.0f - 1.0e-6f;
        product = std::clamp(product, -kTanhLimit, kTanhLimit);
        bit_to_check[static_cast<std::size_t>(bit)]
                    [static_cast<std::size_t>(k)] =
            2.0f * std::atanh(-product);
      }
    }
  }

  return -1;
}

bool parity_bit(std::size_t row, std::size_t column) {
  std::size_t const index = row * data::kPayloadBits + column;
  return ((kParityMatrix[index / 64] >> (index % 64)) & 1u) != 0;
}

// Ordered statistics decoding (see Lin and Costello, Error Control Coding,
// 2nd ed., 2004, as cited by Franke, Somerville and Taylor, QEX July/August
// 2020). The generator rows
// are reduced over the most reliable independent positions; candidate
// codewords re-encode the hard decisions there with up to `order` flips, and
// the candidate with the smallest discrepancy (sum of |llr| over bits that
// disagree with the hard decisions) wins.
int ordered_statistics_decode(std::array<float, 174> const& llr,
                              std::array<std::int8_t, 87>& payload,
                              std::array<std::int8_t, 174>& codeword,
                              int order, float* discrepancy) {
  using namespace data;
  using Row = std::bitset<kCodeLength>;

  // Generator row r: message bit r contributes to parity bit p when
  // parity_bit(p, r), and to its own systematic position 87 + r.
  static std::array<Row, kPayloadBits> const generator = [] {
    std::array<Row, kPayloadBits> rows{};
    for (std::size_t r = 0; r < kPayloadBits; ++r) {
      for (std::size_t p = 0; p < kPayloadBits; ++p) {
        if (parity_bit(p, r)) rows[r].set(p);
      }
      rows[r].set(kPayloadBits + r);
    }
    return rows;
  }();

  std::array<int, kCodeLength> by_reliability{};
  std::iota(by_reliability.begin(), by_reliability.end(), 0);
  std::stable_sort(by_reliability.begin(), by_reliability.end(), [&llr](int a, int b) {
    return std::abs(llr[static_cast<std::size_t>(a)]) > std::abs(llr[static_cast<std::size_t>(b)]);
  });

  Row hard;
  for (std::size_t k = 0; k < kCodeLength; ++k) {
    if (llr[k] > 0.0f) hard.set(k);
  }

  // Gauss-Jordan elimination, taking pivot positions in reliability order.
  std::array<Row, kPayloadBits> rows = generator;
  std::array<int, kPayloadBits> pivot{};
  std::size_t rank = 0;
  for (int position : by_reliability) {
    if (rank == kPayloadBits) break;
    std::size_t const column = static_cast<std::size_t>(position);
    std::size_t found = rank;
    while (found < kPayloadBits && !rows[found].test(column)) ++found;
    if (found == kPayloadBits) continue;
    std::swap(rows[rank], rows[found]);
    for (std::size_t r = 0; r < kPayloadBits; ++r) {
      if (r != rank && rows[r].test(column)) rows[r] ^= rows[rank];
    }
    pivot[rank] = position;
    ++rank;
  }
  if (rank != kPayloadBits) return -1;

  // Order 0: re-encode the hard decisions on the pivot positions.
  Row base;
  for (std::size_t r = 0; r < kPayloadBits; ++r) {
    if (hard.test(static_cast<std::size_t>(pivot[r]))) base ^= rows[r];
  }

  auto cost = [&llr, &hard](Row const& candidate) {
    Row const diff = candidate ^ hard;
    float total = 0.0f;
    for (std::size_t k = 0; k < kCodeLength; ++k) {
      if (diff.test(k)) total += std::abs(llr[k]);
    }
    return total;
  };

  Row best = base;
  float best_cost = cost(base);
  auto consider = [&](Row const& candidate, float c) {
    if (c < best_cost) {
      best_cost = c;
      best = candidate;
    }
  };
  if (order >= 1) {
    for (std::size_t a = 0; a < kPayloadBits; ++a) {
      Row const one = base ^ rows[a];
      float const c1 = cost(one);
      consider(one, c1);
      if (order < 2) continue;
      for (std::size_t b = a + 1; b < kPayloadBits; ++b) {
        Row const two = one ^ rows[b];
        float const c2 = cost(two);
        consider(two, c2);
      }
    }
  }

  int errors = 0;
  for (std::size_t k = 0; k < kCodeLength; ++k) {
    codeword[k] = best.test(k) ? 1 : 0;
    if (best.test(k) != hard.test(k)) ++errors;
  }
  std::copy(codeword.begin() + kParityBits, codeword.end(), payload.begin());
  if (discrepancy) *discrepancy = best_cost;
  return errors;
}

}  // namespace js8core::decoder
