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
 * File:   ofproto-dpif-xlate.h
 * Author: mshahbaz@cs.princeton.edu
 */

#ifndef OVS_ACTION_OFPROTO_DPIF_XLATE_H
#define	OVS_ACTION_OFPROTO_DPIF_XLATE_H 1

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_ACTION_HELPER_FUNCS \
    \

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_RECIRC_UNROLL_ACTIONS_CASES \
    \

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_COMPOSE_AND_XLATE_FUNCS \
    \

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_DO_XLATE_ACTIONS_CASES \
    \

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_COMPOSE_CALC_FIELDS_CASES \
//::  for header_name in ordered_header_instances_regular:
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
    case MFF_${field_name.upper()}: \
        nl_msg_put_flag(ctx->odp_actions, OVS_CALC_FIELD_ATTR_${field_name.upper()}); \
        break; \
//::    #endfor
//::  #endfor
    \

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_COMPOSE_ADD_TO_FIELD_CASES \
//::  for header_name in ordered_header_instances_regular:
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
//    case MFF_${field_name.upper()}: \
//::      if bit_width == 8:
//        compose_add_to_field_(ctx, OVS_KEY_ATTR_${field_name.upper()}, &value->u8, sizeof value->u8); \
//::      elif bit_width == 16:
//        compose_add_to_field_(ctx, OVS_KEY_ATTR_${field_name.upper()}, &value->be16, sizeof value->be16); \
//::      elif bit_width == 32:
//        compose_add_to_field_(ctx, OVS_KEY_ATTR_${field_name.upper()}, &value->be32, sizeof value->be32); \
//::      elif bit_width == 64:
//        compose_add_to_field_(ctx, OVS_KEY_ATTR_${field_name.upper()}, &value->be64, sizeof value->be64); \
//::      else:
//::        pass  # TODO: handle this case (i.e., for arbitrary byte sizes).
//::      #endif
//        break; \
//::    #endfor
//::  #endfor
//    \

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_COMPOSE_SUB_FROM_FIELD_CASES \
//::  for header_name in ordered_header_instances_regular:
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
//    case MFF_${field_name.upper()}: \
//::      if bit_width == 8:
//        compose_sub_from_field_(ctx, OVS_KEY_ATTR_${field_name.upper()}, &value->u8, sizeof value->u8); \
//::      elif bit_width == 16:
//        compose_sub_from_field_(ctx, OVS_KEY_ATTR_${field_name.upper()}, &value->be16, sizeof value->be16); \
//::      elif bit_width == 32:
//        compose_sub_from_field_(ctx, OVS_KEY_ATTR_${field_name.upper()}, &value->be32, sizeof value->be32); \
//::      elif bit_width == 64:
//        compose_sub_from_field_(ctx, OVS_KEY_ATTR_${field_name.upper()}, &value->be64, sizeof value->be64); \
//::      else:
//::        pass  # TODO: handle this case (i.e., for arbitrary byte sizes).
//::      #endif
//        break; \
//::    #endfor
//::  #endfor
//    \

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_XLATE_ADD_TO_FIELD_CASES \
	union mf_value tmp; \
//::  for header_name in ordered_header_instances_regular:
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
	case MFF_${field_name.upper()}: \
//::      if bit_width == 8:
		memset(&wc->masks._${header_name}.hdr.${field_name}, 0xff, sizeof value->u8); \
		apply_mask((const uint8_t *) value, (const uint8_t *) mask, \
			(uint8_t *) &tmp, sizeof value->u8); \
		tmp.u8 = flow->_${header_name}.hdr.${field_name} + tmp.u8; \
		apply_mask((const uint8_t *) &tmp, (const uint8_t *) mask, \
			(uint8_t *) &flow->_${header_name}.hdr.${field_name}, sizeof value->u8); \
//::      elif bit_width == 16:
		memset(&wc->masks._${header_name}.hdr.${field_name}, 0xff, sizeof value->be16); \
		apply_mask((const uint8_t *) value, (const uint8_t *) mask, \
			(uint8_t *) &tmp, sizeof value->be16); \
		tmp.be16 = htons(ntohs(flow->_${header_name}.hdr.${field_name}) + ntohs(tmp.be16)); \
		apply_mask((const uint8_t *) &tmp, (const uint8_t *) mask, \
			(uint8_t *) &flow->_${header_name}.hdr.${field_name}, sizeof value->be16); \
//::      elif bit_width == 32:
		memset(&wc->masks._${header_name}.hdr.${field_name}, 0xff, sizeof value->be32); \
		apply_mask((const uint8_t *) value, (const uint8_t *) mask, \
			(uint8_t *) &tmp, sizeof value->be32); \
		tmp.be32 = htonl(ntohl(flow->_${header_name}.hdr.${field_name}) + ntohl(tmp.be32)); \
		apply_mask((const uint8_t *) &tmp, (const uint8_t *) mask, \
			(uint8_t *) &flow->_${header_name}.hdr.${field_name}, sizeof value->be32); \
//::      elif bit_width == 64:
		memset(&wc->masks._${header_name}.hdr.${field_name}, 0xff, sizeof value->be64); \
		apply_mask((const uint8_t *) value, (const uint8_t *) mask, \
			(uint8_t *) &tmp, sizeof value->be64); \
		tmp.be64 = htonll(ntohll(flow->${header_name}.hdr.${field_name}) + ntohll(tmp.be64)); \
		apply_mask((const uint8_t *) &tmp, (const uint8_t *) mask, \
			(uint8_t *) &flow->_${header_name}.hdr.${field_name}, sizeof value->be64); \
//::      else:
//::        pass  # TODO: handle this case (i.e., for arbitrary byte sizes).
//::      #endif
		break; \
//::    #endfor
//::  #endfor
	  \

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_XLATE_SUB_FROM_FIELD_CASES \
	union mf_value tmp; \
//::  for header_name in ordered_header_instances_regular:
//::    for field_name, bit_width in ordered_header_instances_non_virtual_field__name_width[header_name]:
	case MFF_${field_name.upper()}: \
//::      if bit_width == 8:
		memset(&wc->masks._${header_name}.hdr.${field_name}, 0xff, sizeof value->u8); \
		apply_mask((const uint8_t *) value, (const uint8_t *) mask, \
			(uint8_t *) &tmp, sizeof value->u8); \
		tmp.u8 = flow->_${header_name}.hdr.${field_name} - tmp.u8; \
		apply_mask((const uint8_t *) &tmp, (const uint8_t *) mask, \
			(uint8_t *) &flow->_${header_name}.hdr.${field_name}, sizeof value->u8); \
//::      elif bit_width == 16:
		memset(&wc->masks._${header_name}.hdr.${field_name}, 0xff, sizeof value->be16); \
		apply_mask((const uint8_t *) value, (const uint8_t *) mask, \
			(uint8_t *) &tmp, sizeof value->be16); \
		tmp.be16 = htons(ntohs(flow->_${header_name}.hdr.${field_name}) - ntohs(tmp.be16)); \
		apply_mask((const uint8_t *) &tmp, (const uint8_t *) mask, \
			(uint8_t *) &flow->_${header_name}.hdr.${field_name}, sizeof value->be16); \
//::      elif bit_width == 32:
		memset(&wc->masks._${header_name}.hdr.${field_name}, 0xff, sizeof value->be32); \
		apply_mask((const uint8_t *) value, (const uint8_t *) mask, \
			(uint8_t *) &tmp, sizeof value->be32); \
		tmp.be32 = htonl(ntohl(flow->_${header_name}.hdr.${field_name}) - ntohl(tmp.be32)); \
		apply_mask((const uint8_t *) &tmp, (const uint8_t *) mask, \
			(uint8_t *) &flow->_${header_name}.hdr.${field_name}, sizeof value->be32); \
//::      elif bit_width == 64:
		memset(&wc->masks._${header_name}.hdr.${field_name}, 0xff, sizeof value->be64); \
		apply_mask((const uint8_t *) value, (const uint8_t *) mask, \
			(uint8_t *) &tmp, sizeof value->be64); \
		tmp.be64 = htonll(ntohll(flow->${header_name}.hdr.${field_name}) - ntohll(tmp.be64)); \
		apply_mask((const uint8_t *) &tmp, (const uint8_t *) mask, \
			(uint8_t *) &flow->_${header_name}.hdr.${field_name}, sizeof value->be64); \
//::      else:
//::        pass  # TODO: handle this case (i.e., for arbitrary byte sizes).
//::      #endif
		break; \
//::    #endfor
//::  #endfor
	  \

/* -- Called in ofproto/ofproto-dpif-xlate.c -- */
#define OVS_COMPOSE_ADD_REMOVE_HEADER_CASES \
//::  for header_name in ordered_header_instances_regular:
    case MFF_${header_name.upper()}_VALID: \
		nl_msg_put_flag(ctx->odp_actions, OVS_KEY_ATTR__${header_name.upper()}); \
        break; \
//::  #endfor
    \

#endif	/* OVS_ACTION_OFPROTO_DPIF_XLATE_H */
