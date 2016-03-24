#!/usr/bin/env python

import p4_hlir.hlir.p4 as p4
from tenjin.util import *
import sys
import os
import json
import argparse
import cPickle as pickle
import re
import subprocess

# Valid match field defined in OVS
MATCH_TABLE = "table"
MATCH_PRIORITY = "priority"
MATCH_IN_PORT = "in_port"
MATCH_REG = "reg"
VALID_MATCH_TAGS = [MATCH_TABLE, MATCH_PRIORITY, MATCH_IN_PORT]

# Valid Actions defined in OVS
ACTION_TYPE = "action"
ACTION_OUTPUT = "output"
ACTION_DEPARSE = "deparse"
ACTION_RESUBMIT = "resubmit"
ACTION_SET_FIELD = "set_field"
ACTION_CALC_FIELDS = "calc_fields_update"
ACTION_VERIFY_FIELDS = "verify_fields_update"
VALID_ACTION_TAGS = [
  ACTION_OUTPUT, ACTION_DEPARSE, ACTION_RESUBMIT,
  ACTION_SET_FIELD, ACTION_CALC_FIELDS,
  ACTION_VERIFY_FIELDS
]


def exit_with_error(error_msg):
  """ Exit with error code after printing the error """
  sys.stderr.write("flow_type_checker: Error: %s\n" % error_msg)
  sys.exit(1)


def split_func_args(s):
  """ Splits the functions and its arguments """
  pattern = r'(\w[\w\d_]*)\((.*)\)$'
  match = re.match(pattern, s)
  if match:
    return list(match.groups())
  else:
    return []


def get_parser():
  """ Parser definition for the flow type checker. """
  parser = argparse.ArgumentParser(description='PIECES flow type checker arguments')
  parser.add_argument('parse_dict_file',
    metavar='parse_dict_file',
    type=str,
    help='Location of the parser definition dictionary file'
      ' with p4 to ovs parser specification.')

  parser.add_argument('--add-flow',
    action='store_true',
    default=False,
    help='Toggle add flow')

  parser.add_argument('--check-flow',
    action='store_true',
    default=False,
    help='Toggle to only check flow')

  parser.add_argument('--protocols',
    metavar="protocols",
    type=str,
    default=None,
    help='Protocol for ovs-ofctl argument')

  parser.add_argument('--br',
    metavar='bridge_name',
    type=str,
    default=None,
    help='Bridge name. ex: --br br0')

  parser.add_argument('--match',
    metavar='match_vals',
    type=str,
    default=None,
    help='Match values for the flow.\n'
      'ex: --match "table=0,priority=32768"')

  parser.add_argument('--action',
    metavar='action_vals',
    type=str,
    default=None,
    help='Action values for the flow.\n'
      'ex: --action "actions=resubmit(,1)"')

  parser.add_argument('--ofctl-path',
    metavar='ofctl_path',
    type=str,
    default=None,
    help='Path of the ovs ofctl to invoke')
  return parser


def check_flow_type(match_vals, action_vals, parser_def_dict):
  """ Checks the flow type to see if it is correct. """
  if not match_vals:
    exit_with_error("No match values given")
  if not action_vals:
    exit_with_error("No action values given")

  # Checking the match variables
  match_list = match_vals.split(",")
  for match_elem in match_list:
    try:
      match_name, match_val = match_elem.split("=")
    except Exception:
      exit_with_error("Invalid match format.")
    if match_name in VALID_MATCH_TAGS:
      continue
    # TODO: Fix the wrong registry value
    if MATCH_REG in match_name:
      continue
    if match_name not in parser_def_dict['field_info']:
      exit_with_error("Match variable is not part of the spec")
    match_dict = parser_def_dict['field_info'][match_name]
    #TODO: Support something other than hex or int
    try:
      if match_val.startswith("0x"):
        match_val = int(match_val, 16)
      else:
        match_val = int(match_val)
      if match_val.bit_length() > match_dict['bit_width']:
        exit_with_error("The argument exceeds the allowed input size."
          " Given: %s, Allowed: %s" %
          (match_val.bit_length(), match_dict['bit_width']))
    except Exception:
      exit_with_error("Invalid match value format. "
        "Please provide either int or hex")

  # Checking the actions
  action_name, actions = action_vals.split("=")
  if action_name != ACTION_TYPE:
    exit_with_error("Invalid action type name")
  r = re.compile(r'(?:[^,(]|\([^)]*\))+')
  action_list = r.findall(actions) 
  action_dict = parser_def_dict['action_info']
  for action in action_list:
    r = re.compile(r'(?:[^:(]|\([^)]*\))+')
    action_elems = r.findall(action) 
    # This is the case when there is only one action
    if len(action_elems) == 1:
      if action_elems[0] in action_dict: 
        continue
      if action_elems[0] in VALID_ACTION_TAGS:
        continue
      if "(" in action_elems[0]:
        func_name, arg_string = split_func_args(action_elems[0])
        if func_name in VALID_ACTION_TAGS:
          continue
        func_args = [x.strip() for x in arg_string.split(',')]
        if func_name in action_dict:
          #TODO: Look into the function call and the arg types
          continue
      exit_with_error("Invalid action name")
    # This is the case when there is an argument to a function 
    elif len(action_elems) == 2:
      action_name, action_arg = action_elems
      if action_name in VALID_ACTION_TAGS:
        continue
      if action_name in action_dict:
        call_seq = action_dict[action_name]['call_sequence']
        # TODO: Look into call sequence
        continue
    else:
      exit_with_error("Invalid number of arguments for an action")


def add_flow_types(bridge_name, match_vals, action_vals,
    ofctl_path, protocols=None):
  """ Actually call the add flow with ovs ofctl """
  if not ofctl_path:
    exit_with_error("Need a valid ofctl path")
  subprocess_lst = [ofctl_path]
  # Add the protocols
  if protocols:
    subprocess_lst.append("--protocols=%s" % protocols)
  subprocess_lst.append("add-flow")
  subprocess_lst.append(bridge_name)
  # For the match action string
  add_str = "%s %s" % (match_vals, action_vals)
  subprocess_lst.append(add_str)
  print "Calling %s with %s args" % (ofctl_path, subprocess_lst)
  output = subprocess.call(subprocess_lst)
  return output == 0


def main():
  """ Main function to take care of the parser,
      check flow and add them to ovs utility if needed.
  """
  parser = get_parser()
  input_args = sys.argv[1:]
  args, unparsed_args = parser.parse_known_args()
  parse_dict = None
  try:
    parse_dict = pickle.load(open(args.parse_dict_file, "rb"))
  except Exception:
    exit_with_error("Failed to load the parse dictionary file")

  flow_type_arg = None
  add_flow = args.add_flow
  check_flow = args.check_flow
  if not add_flow and not check_flow:
    exit_with_error("You must either add or check flow")
  if add_flow and check_flow:
    exit_with_error("Please specify only one add or check flow")
  match_vals = args.match
  action_vals = args.action
  bridge_name = args.br
  if not bridge_name:
    exit_with_error("No bridge name given")

  check_flow_type(match_vals, action_vals, parse_dict)
  if add_flow:
    # Add the actual flow to the ofctl utility
    add_flow_types(bridge_name, match_vals, action_vals,
      args.ofctl_path, protocols=args.protocols)

if __name__ == "__main__":
    main()
