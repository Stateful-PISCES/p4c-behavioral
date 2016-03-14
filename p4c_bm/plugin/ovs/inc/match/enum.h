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
 * File:   enum.h
 * Author: mshahbaz@cs.princeton.edu
 */

#ifndef OVS_MATCH_ENUM_H
#define	OVS_MATCH_ENUM_H 1

/* -- Included in lib/meta-flow.h -- */

/* @Shahbaz:
 * 1. Make sure to add preceding tabs in the following fields. Otherwise, will result in errors.
 * 2. For now prerequisites are not handled.
 * 3. For now all fields are maskable.
 */

//::  base_oxm_offset = 45
//::  for header_name in ordered_header_instances_non_virtual:
//::    if header_name == "standard_metadata":
//::      continue
//::    #endif
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
    /* "${field_name}".
     *
     * ${field_name} field.
     *
     * Type: be${bit_width}.
     * Formatting: hexadecimal.
     * Maskable: bitwise.
     * Prerequisites: none.
     * Access: read/write.
     * NXM: none.
     * OXM: OXM_OF_${field_name.upper()}(${base_oxm_offset}) since OF1.5 and v2.3.
     */
    MFF_${field_name.upper()},
//::      base_oxm_offset += 1

//::    #endfor
//::  #endfor
//::
//::  for header_name in ordered_header_instances_regular:
    /* "${header_name}_valid".
     *
     * ${header_name}_valid field.
     *
     * Type: be${8}.
     * Formatting: hexadecimal.
     * Maskable: bitwise.
     * Prerequisites: none.
     * Access: read/write.
     * NXM: none.
     * OXM: OXM_OF_${header_name.upper()}_VALID(${base_oxm_offset}) since OF1.5 and v2.3.
     */
    MFF_${header_name.upper()}_VALID,
//::      base_oxm_offset += 1

//::  #endfor

/* Do NOT REMOVE THIS. */
    // MFF_N_IDS

#endif	/* OVS_MATCH_ENUM_H */