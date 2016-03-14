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
 * File:   types.h
 * Author: mshahbaz@cs.princeton.edu
 */

#ifndef OVS_MATCH_OPENVSWITCH_TYPES_H
#define	OVS_MATCH_OPENVSWITCH_TYPES_H 1

/* -- Called in include/openvswitch/types.h -- */
#define OVS_FIELD_STRUCTS \
//::  for field_name, bit_width in ordered_field_instances_non_virtual__name_width:
//::    if field_name.startswith("standard_metadata"):
//::      continue
//::    #endif
//::
//::    if not (bit_width == 8 or bit_width == 16 or bit_width == 32 or bit_width == 64):
    struct ${field_name}_t { \
        uint8_t data[${bit_width}/8]; \
    }; \
//::    #endif
//::  #endfor
    \

#endif	/* OVS_MATCH_OPENVSWITCH_TYPES_H */