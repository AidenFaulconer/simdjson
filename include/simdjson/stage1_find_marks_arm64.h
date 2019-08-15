#ifndef SIMDJSON_STAGE1_FIND_MARKS_ARM64_H
#define SIMDJSON_STAGE1_FIND_MARKS_ARM64_H

#include "simdjson/simd_input_arm64.h"
#include "simdjson/simdutf8check_arm64.h"
#include "simdjson/stage1_find_marks.h"

#ifdef IS_ARM64
namespace simdjson {

template <>
really_inline uint64_t
compute_quote_mask<Architecture::ARM64>(uint64_t quote_bits) {
#ifdef __ARM_FEATURE_CRYPTO // some ARM processors lack this extension
  return vmull_p64(-1ULL, quote_bits);
#else
  return portable_compute_quote_mask(quote_bits);
#endif
}

template <>
really_inline void find_whitespace_and_structurals<Architecture::ARM64>(
    simd_input<Architecture::ARM64> in, uint64_t &whitespace,
    uint64_t &structurals) {
  const uint8x16_t low_nibble_mask =
      (uint8x16_t){16, 0, 0, 0, 0, 0, 0, 0, 0, 8, 12, 1, 2, 9, 0, 0};
  const uint8x16_t high_nibble_mask =
      (uint8x16_t){8, 0, 18, 4, 0, 1, 0, 1, 0, 0, 0, 3, 2, 1, 0, 0};
  const uint8x16_t structural_shufti_mask = vmovq_n_u8(0x7);
  const uint8x16_t whitespace_shufti_mask = vmovq_n_u8(0x18);
  const uint8x16_t low_nib_and_mask = vmovq_n_u8(0xf);

  uint8x16_t nib_0_lo = vandq_u8(in.i0, low_nib_and_mask);
  uint8x16_t nib_0_hi = vshrq_n_u8(in.i0, 4);
  uint8x16_t shuf_0_lo = vqtbl1q_u8(low_nibble_mask, nib_0_lo);
  uint8x16_t shuf_0_hi = vqtbl1q_u8(high_nibble_mask, nib_0_hi);
  uint8x16_t v_0 = vandq_u8(shuf_0_lo, shuf_0_hi);

  uint8x16_t nib_1_lo = vandq_u8(in.i1, low_nib_and_mask);
  uint8x16_t nib_1_hi = vshrq_n_u8(in.i1, 4);
  uint8x16_t shuf_1_lo = vqtbl1q_u8(low_nibble_mask, nib_1_lo);
  uint8x16_t shuf_1_hi = vqtbl1q_u8(high_nibble_mask, nib_1_hi);
  uint8x16_t v_1 = vandq_u8(shuf_1_lo, shuf_1_hi);

  uint8x16_t nib_2_lo = vandq_u8(in.i2, low_nib_and_mask);
  uint8x16_t nib_2_hi = vshrq_n_u8(in.i2, 4);
  uint8x16_t shuf_2_lo = vqtbl1q_u8(low_nibble_mask, nib_2_lo);
  uint8x16_t shuf_2_hi = vqtbl1q_u8(high_nibble_mask, nib_2_hi);
  uint8x16_t v_2 = vandq_u8(shuf_2_lo, shuf_2_hi);

  uint8x16_t nib_3_lo = vandq_u8(in.i3, low_nib_and_mask);
  uint8x16_t nib_3_hi = vshrq_n_u8(in.i3, 4);
  uint8x16_t shuf_3_lo = vqtbl1q_u8(low_nibble_mask, nib_3_lo);
  uint8x16_t shuf_3_hi = vqtbl1q_u8(high_nibble_mask, nib_3_hi);
  uint8x16_t v_3 = vandq_u8(shuf_3_lo, shuf_3_hi);

  uint8x16_t tmp_0 = vtstq_u8(v_0, structural_shufti_mask);
  uint8x16_t tmp_1 = vtstq_u8(v_1, structural_shufti_mask);
  uint8x16_t tmp_2 = vtstq_u8(v_2, structural_shufti_mask);
  uint8x16_t tmp_3 = vtstq_u8(v_3, structural_shufti_mask);
  structurals = neon_movemask_bulk(tmp_0, tmp_1, tmp_2, tmp_3);

  uint8x16_t tmp_ws_0 = vtstq_u8(v_0, whitespace_shufti_mask);
  uint8x16_t tmp_ws_1 = vtstq_u8(v_1, whitespace_shufti_mask);
  uint8x16_t tmp_ws_2 = vtstq_u8(v_2, whitespace_shufti_mask);
  uint8x16_t tmp_ws_3 = vtstq_u8(v_3, whitespace_shufti_mask);
  whitespace = neon_movemask_bulk(tmp_ws_0, tmp_ws_1, tmp_ws_2, tmp_ws_3);
}
} // namespace simdjson

#endif // IS_ARM64
#endif // SIMDJSON_STAGE1_FIND_MARKS_ARM64_H
