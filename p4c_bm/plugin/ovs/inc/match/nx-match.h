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
 * File:   nx-match.h
 * Author: mshahbaz@cs.princeton.edu
 */

#ifndef OVS_MATCH_NX_MATCH_H
#define	OVS_MATCH_NX_MATCH_H 1

/* -- Called in lib/nx-match.c -- */
#define OVS_MATCH_PUT_RAW \
//::  for header_name in ordered_header_instances_non_virtual:
//::    if header_name == "standard_metadata":
//::      continue
//::    #endif
//::
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
//::      if bit_width == 8:
    nxm_put_8m(b, MFF_${field_name.upper()}, oxm, \
               flow->_${header_name}.hdr.${field_name}, \
               match->wc.masks._${header_name}.hdr.${field_name}); \
//::      elif bit_width == 16:
    nxm_put_16m(b, MFF_${field_name.upper()}, oxm, \
                flow->_${header_name}.hdr.${field_name}, \
                match->wc.masks._${header_name}.hdr.${field_name}); \
//::      elif bit_width == 32:
    nxm_put_32m(b, MFF_${field_name.upper()}, oxm, \
                flow->_${header_name}.hdr.${field_name}, \
                match->wc.masks._${header_name}.hdr.${field_name}); \
//::      elif bit_width == 64:
    nxm_put_64m(b, MFF_${field_name.upper()}, oxm, \
                flow->_${header_name}.hdr.${field_name}, \
                match->wc.masks._${header_name}.hdr.${field_name}); \
//::      else:
    nxm_put(b, MFF_${field_name.upper()}, oxm, \
            &flow->_${header_name}.hdr.${field_name}, \
            &match->wc.masks._${header_name}.hdr.${field_name}, \
            sizeof flow->_${header_name}.hdr.${field_name}); \
//::      #endif
//::    #endfor
    \
//::  #endfor
//::
//::  for header_name in ordered_header_instances_regular:
    nxm_put_8m(b, MFF_${header_name.upper()}_VALID, oxm, \
               flow->valid.hdr._${header_name}_valid, \
               match->wc.masks.valid.hdr._${header_name}_valid); \
//::  #endfor
    \

#endif	/* OVS_MATCH_NX_MATCH_H */