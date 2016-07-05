/*
 * Copyright 2013-present Barefoot Networks, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * File:   packets.h
 * Author: mshahbaz@cs.princeton.edu
 */

#ifndef OVS_MATCH_PACKETS_H
#define	OVS_MATCH_PACKETS_H 1

/* -- Called in lib/packets.h -- */
//::  for header_name in ordered_header_instances_non_virtual:
//::    if header_name == "standard_metadata":
//::      pass
//::    else:
//::      header_len = sum([bit_width for _, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]])/8
#define _${header_name.upper()}_HEADER_LEN ${header_len}
//::    #endif
//::  #endfor
#define VALID_HEADER_LEN ${len(ordered_header_instances_regular)}

/* -- Called in lib/packets.h -- */
#define OVS_HDR_STRUCTS \
//::  for header_name in ordered_header_instances_non_virtual:
//::    if header_name == "standard_metadata":
//::      continue
//::    #endif
//::    run_bit_width = 0
    OVS_PACKED( \
    struct _${header_name}_header { \
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
//::      if bit_width == 8:
        uint8_t ${field_name}; \
//::      elif bit_width == 16:
        ovs_be16 ${field_name}; \
//::      elif bit_width == 32:
        ovs_be32 ${field_name}; \
//::      elif bit_width == 64:
        ovs_be64 ${field_name}; \
//::      else:
        struct ${field_name}_t ${field_name}; \
//::      #endif
//::      run_bit_width += bit_width
//::    #endfor
    }); \
    BUILD_ASSERT_DECL(_${header_name.upper()}_HEADER_LEN == sizeof(struct _${header_name}_header)); \
    \
    OVS_PACKED( \
    struct _${header_name}_padded_header { \
        struct _${header_name}_header hdr; \
//::    pad_bits = 64 - (run_bit_width % 64)
//::    pad_bytes = 0
//::    if pad_bits < 64:
//::      pad_bytes = pad_bits/8
        uint8_t pad[${pad_bytes}]; \
//::    #endif
    }); \
    BUILD_ASSERT_DECL( \
        _${header_name.upper()}_HEADER_LEN+${pad_bytes} == sizeof(struct _${header_name}_padded_header)); \
    \
//::  #endfor
    OVS_PACKED( \
    struct valid_header { \
//::  for header_name in ordered_header_instances_regular:
        uint8_t _${header_name}_valid; \
//::  #endfor
    }); \
    BUILD_ASSERT_DECL(VALID_HEADER_LEN == sizeof(struct valid_header)); \
    \
    OVS_PACKED( \
    struct valid_padded_header { \
        struct valid_header hdr; \
//::    pad_bits = 64 - ((len(ordered_header_instances_regular)*8) % 64)
//::    pad_bytes = 0
//::    if pad_bits < 64:
//::      pad_bytes = pad_bits/8
        uint8_t pad[${pad_bytes}]; \
//::    #endif
    }); \
    BUILD_ASSERT_DECL( \
        VALID_HEADER_LEN+${pad_bytes} == sizeof(struct valid_padded_header)); \
    \

/* -- Called in lib/packets.h -- */
#define OVS_HDR_DECLS \
//::  for header_name in ordered_header_instances_regular:
    void packet_set__${header_name}( \
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
//::      if bit_width == 8:
        uint8_t ${field_name}, \
//::      elif bit_width == 16:
        ovs_be16 ${field_name}, \
//::      elif bit_width == 32:
        ovs_be32 ${field_name}, \
//::      elif bit_width == 64:
        ovs_be64 ${field_name}, \
//::      else:
        struct ${field_name}_t ${field_name}, \
//::      #endif
//::    #endfor
        struct dp_packet *packet); \
    \
//::  #endfor
    void packet_set_valid( \
//::  for header_name in ordered_header_instances_regular:
        uint8_t _${header_name}_valid, \
//::  #endfor
        struct dp_packet *packet); \
    \

/* -- Called in lib/packets.c -- */
#define OVS_HDR_DEFS \
//::  for header_name in ordered_header_instances_regular:
    void packet_set__${header_name}( \
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
//::      if bit_width == 8:
        uint8_t ${field_name}, \
//::      elif bit_width == 16:
        ovs_be16 ${field_name}, \
//::      elif bit_width == 32:
        ovs_be32 ${field_name}, \
//::      elif bit_width == 64:
        ovs_be64 ${field_name}, \
//::      else:
        struct ${field_name}_t ${field_name}, \
//::      #endif
//::    #endfor
        struct dp_packet *packet) \
    { \
//::    if OPT_INLINE_EDITING:
        struct _${header_name}_header *_${header_name} = dp_packet_${header_name}(packet); \
//::    else:
        struct _${header_name}_header *_${header_name} = &packet->_${header_name}; \
//::    #endif
        \
//::    if OPT_INCREMENTAL_CHECKSUM:
//::      # TODO: we assume that field_list consists of fields from a same header and each header only has one checksum.
//::      if header_name in incremental_header_checksums:
//::        csum_field_name = incremental_header_checksums[header_name]["field_name"]
        ovs_be16 new_csum = _${header_name}->${csum_field_name}; \
//::        field_names_ = []
//::        for field_name_ in incremental_header_checksums[header_name]["field_list"]:
//::          if aligned_field_info[field_name_]["name"] in field_names_:
//::            continue
//::          #endif
//::          field_name = aligned_field_info[field_name_]["name"]
//::          field_names_.append(aligned_field_info[field_name_]["name"])
//::          bit_width = aligned_field_info[field_name_]["bit_width"]
//::          if bit_width == 8:
        if (offsetof(struct _${header_name}_header, ${field_name}) % 2 == 0) \
            new_csum = recalc_csum16(new_csum, htons(_${header_name}->${field_name} << 8), htons(${field_name} << 8)); \
        else \
            new_csum = recalc_csum16(new_csum, htons((uint16_t) _${header_name}->${field_name}), htons((uint16_t) ${field_name})); \
//::          elif bit_width == 16:
        new_csum = recalc_csum16(new_csum, _${header_name}->${field_name}, ${field_name}); \
//::          elif bit_width == 32:
        new_csum = recalc_csum16(new_csum, _${header_name}->${field_name}, ${field_name}); \
        new_csum = recalc_csum16(new_csum, _${header_name}->${field_name} >> 16, ${field_name} >> 16); \
//::          elif bit_width == 32:
        new_csum = recalc_csum16(new_csum, _${header_name}->${field_name}, ${field_name}); \
        new_csum = recalc_csum16(new_csum, _${header_name}->${field_name} >> 16, ${field_name} >> 16); \
        new_csum = recalc_csum16(new_csum, _${header_name}->${field_name} >> 32, ${field_name} >> 32); \
        new_csum = recalc_csum16(new_csum, _${header_name}->${field_name} >> 48, ${field_name} >> 48); \
//::          else:
//::            # TODO: handle this case (for arbitrary byte combinations).
        \
//::          #endif
//::        #endfor
        _${header_name}->${csum_field_name} = new_csum; \
//::      #endif
        \
//::    #endif
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
        _${header_name}->${field_name} = ${field_name}; \
//::    #endfor
    } \
    \
//:: #endfor
//::
    void packet_set_valid( \
//::  for header_name in ordered_header_instances_regular:
        uint8_t _${header_name}_valid, \
//::  #endfor
        struct dp_packet *packet) \
    { \
//::  for header_name in ordered_header_instances_regular:
        packet->_${header_name}_valid = _${header_name}_valid; \
//::  #endfor
    } \
    \

#endif	/* OVS_MATCH_PACKETS_H */
