import json
import p4_hlir.hlir as H
import types

import os

EXEC = "/bin/sh -ve"
DIR = "~/p4-vswitch/ovs/utilities"
OFCTL = "/ovs-ofctl"
OFPROTO = "OpenFlow15"
# special char for matching all
MATCH_ALL = "*"

P4_FILE="l2_switch"

def write_exec_path(rule_file):
    rule_file.write("#! "+EXEC+"\n\n")

def write_ofctl_dir_path(rule_file):
    rule_file.write("# Please make sure that you update the path to the current OVS directory.\n")
    rule_file.write("DIR="+DIR+"\n\n")
    return "$DIR"+OFCTL

def form_table_setting_str(table_id, priority, match, actions):
    match_setting = ",".join([x for x in [table_id, priority, match] if x is not None])
    table_setting = match_setting + " actions=" + actions
    return table_setting

def write_table_rules(rule_file, tables, ovscmd, p4_locks):
    protocol = "--protocols="+OFPROTO
    br = "br0"
    basecmd = [ovscmd, protocol]

    table_flow_rules = []
    # Delete old rules
    delcmd = basecmd + ["del-flow", br]
    table_flow_rules.append(delcmd)

    # Add flow rules for each table
    for i, tup in enumerate(tables):
        # If last egress table
        last_table = True if i == len(tables)-1 else False
        
        name, table = tup
        table_flow_rules.append(["# "+name+" Table %d" % (i)])
        # Table id string
        table_id_str = "table=%d" % (i)
        for action_name, settings in table.iteritems():
            # Priority string
            priority_str = "priority=%d" % (settings["priority"])
            actions_str = settings['actions']
            if actions_str == 'resubmit':
                actions_str = "resubmit(,%d)" % (i+1)
#
#            if action_name == "mac_learn":
#                actions_str = "controller"
#            elif action_name == "broadcast":
#                actions_str = "flood"
#            elif last_table and action_name == "_nop":
#                actions_str = "deparse,output:NXM_NX_REG0[]"
#            elif action_name == "_drop":
#                actions_str = ""
#            else:
#                actions_str = "resubmit(,%d)" % (i+1)
            # Add P4 action locks
            if 'lock' in settings:
                lock_name = settings['lock']
                lock_index = p4_locks.index(lock_name)
                lock_prim, unlock_prim = 'lock:%d' % lock_index, 'unlock:%d' % lock_index
                actions_str = lock_prim + ',' + actions_str + ',' + unlock_prim
            # Match condition string
            if settings["match"] == MATCH_ALL:
                match_str = None
                table_setting = form_table_setting_str(table_id_str, priority_str,\
                        match_str, actions_str)
                table_flow_rules.append(basecmd+["add_flow",br,"\""+table_setting+"\""])
            else:
                for match_value in settings["match"]:
                    match_str = match_value    
                    table_setting = form_table_setting_str(table_id_str, priority_str,\
                        match_str, actions_str)
                    table_flow_rules.append(basecmd+["add_flow",br,"\""+table_setting+"\""])

    # Populated all the table flow rules
    for rule in table_flow_rules:
        rule_file.write(" ".join(rule) + "\n")
        

def write_flow_rules_to_file(gen_dir, tables, p4_locks):
    filepath = gen_dir+"/"+P4_FILE+"_gen.sh"
    with open(filepath, "wb") as rule_file:
        write_exec_path(rule_file)
        ovscmd = write_ofctl_dir_path(rule_file)
        write_table_rules(rule_file, tables, ovscmd, p4_locks)

def populate_flow_tables(hlir, rule_args):
    tables = []
    table_args = rule_args["tables"]
    for name, control in hlir.p4_control_flows.iteritems():
        for node in control.call_sequence:
            if isinstance(node, H.p4_table):
                if node.name in table_args.keys():
                    tables.append((node.name, table_args[node.name]))
            elif isinstance(node, tuple):
                # P4 Expression case
                for left_subnode in node[1]:
                    if isinstance(left_subnode, H.p4_table):
                        if left_subnode.name in table_args.keys():
                            tables.append((left_subnode.name, table_args[left_subnode.name]))
                for right_subnode in node[2]:
                    if isinstance(right_subnode, H.p4_table):
                        if right_subnode.name in table_args.keys():
                            tables.append((right_subnode.name, table_args[right_subnode.name]))
    return tables

def render_flow_rules(hlir, gen_dir, fr_filepath):
    with open(fr_filepath) as arg_file:
        # Peel the outmost layer
        rule_args = json.load(arg_file)["p4_flow_rules"]
    
    tables = populate_flow_tables(hlir, rule_args)
    p4_locks = rule_args['locks']
    # For now, we only support custom table arguments
    write_flow_rules_to_file(gen_dir, tables, p4_locks)
