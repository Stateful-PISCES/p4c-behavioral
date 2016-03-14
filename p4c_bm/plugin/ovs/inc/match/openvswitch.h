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
 * File:   openvswitch.h
 * Author: mshahbaz@cs.princeton.edu
 */

#ifndef OVS_MATCH_OPENVSWITCH_H
#define	OVS_MATCH_OPENVSWITCH_H 1

/* -- Called in datapath/linux/compat/include/linux/openvswitch.h -- */
#define OVS_KEY_ATTRS \
//::  for header_name in ordered_header_instances_non_virtual:
//::    if header_name == "standard_metadata":
//::      continue
//::    #endif
    OVS_KEY_ATTR__${header_name.upper()}, \
//::  #endfor
    OVS_KEY_ATTR_VALID, \
    \

/* -- Called in datapath/linux/compat/include/linux/openvswitch.h -- */
#define OVS_KEY_STRUCTS \
//::  for header_name in ordered_header_instances_non_virtual:
//::    if header_name == "standard_metadata":
//::      continue
//::    #endif
    struct ovs_key__${header_name} { \
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
//::    #endfor
    }; \
    \
//::  #endfor
    struct ovs_key_valid { \
//::  for header_name in ordered_header_instances_regular:
        uint8_t _${header_name}_valid; \
//::  #endfor
    }; \
    \

#endif	/* OVS_MATCH_OPENVSWITCH_H */