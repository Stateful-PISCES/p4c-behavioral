import json
import p4_hlir.hlir as H
import types
import re
import os
import stat

EXEC = "/bin/sh -ve"
DIR = "~/my-change/ovs/utilities"
OFCTL = "/ovs-ofctl"
OFPROTO = "OpenFlow15"
# special char for matching all
MATCH_ALL = "*"

P4_FILE="l2_switch"
REG_READ = 'register_read'
REG_WRITE = 'register_write'

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

def get_actions_string(table_id, action_name, action_setting, p4_locks, p4_regs):
    actions_list = action_setting['actions']
    modified_actions_list = []
    for action in actions_list:
        # Parse register rules
        reg_actions = action.split(':')
        if reg_actions[0] in [REG_READ, REG_WRITE]:
            # Populate register action rule
            if len(reg_actions) != 2:
                raise "Register action %s is invalid. (reg_action:reg_name)" % action
            if reg_actions[0] == REG_READ:
                matched = re.search(r'(.+)\[(\d+)\]->(.+)', reg_actions[1])
                reg_name, idx, field_name = matched.group(1), matched.group(2), matched.group(3)
            elif reg_actions[0] == REG_WRITE:
                matched = re.search(r'(\d+)->(.+)\[(\d+)\]', reg_actions[1])
                value, reg_name, idx = matched.group(1), matched.group(2), matched.group(3)
            else:
                raise "Unknown register action: %s" % reg_actions[0]
            if reg_name not in p4_regs:
                raise "Register specified in action %s is undeclared in P4 file." % action
            idx = int(idx)
            reg_id = p4_regs.keys().index(reg_name)
            if reg_actions[0] == REG_READ:
                action = '%s:sreg[%d][%d]->%s' % (REG_READ, reg_id, idx, field_name)
            elif reg_actions[0] == REG_WRITE:
                action = '%s:%s->sreg[%d][%d]' % (REG_WRITE, value, reg_id, idx)
        elif action == 'resubmit':
            action = "resubmit(,%d)" % (table_id+1)
        else:
            # Default case, just keep it as is
            # TODO: need to check for invalid actions
            pass
        modified_actions_list.append(action)
    # Add P4 action lock
    if 'lock' in action_setting:
        lock_name = action_setting['lock']
        if lock_name not in p4_locks:
            raise "Action lock %s used in action %s is undeclared in .json file."\
                    % (lock_name, action_name)
        lock_index = p4_locks.index(lock_name)
        lock_prim, unlock_prim = 'lock:%d' % lock_index, 'unlock:%d' % lock_index
        modified_actions_list.insert(0, lock_prim)
        modified_actions_list.append(unlock_prim)
    return ",".join(modified_actions_list)

def write_table_rules(rule_file, tables, ovscmd, p4_locks, p4_regs):
    protocol = "--protocols="+OFPROTO
    br = "br0"
    basecmd = [ovscmd, protocol]

    table_flow_rules = []
    # Delete old rules
    delcmd = basecmd + ["del-flows", br]
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
            actions_str = get_actions_string(i, action_name, settings,\
                                             p4_locks, p4_regs)
            # Match condition string
            if settings["match"] == MATCH_ALL:
                match_str = None
                table_setting = form_table_setting_str(table_id_str, priority_str,\
                        match_str, actions_str)
                table_flow_rules.append(basecmd+["add-flow",br,"\""+table_setting+"\""])
            else:
                for match_value in settings["match"]:
                    match_str = match_value    
                    table_setting = form_table_setting_str(table_id_str, priority_str,\
                        match_str, actions_str)
                    table_flow_rules.append(basecmd+["add-flow",br,"\""+table_setting+"\""])

    # Populated all the table flow rules
    for rule in table_flow_rules:
        rule_file.write(" ".join(rule) + "\n")
        

def write_flow_rules_to_file(gen_dir, tables, p4_locks, p4_regs):
    filepath = gen_dir+"/"+P4_FILE+"_gen.sh"
    with open(filepath, "wb") as rule_file:
        write_exec_path(rule_file)
        ovscmd = write_ofctl_dir_path(rule_file)
        write_table_rules(rule_file, tables, ovscmd, p4_locks, p4_regs)
    # Marking the file as executable
    st = os.stat(filepath)
    os.chmod(filepath, st.st_mode | stat.S_IXUSR | stat.S_IXGRP | stat.S_IXOTH)

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
    write_flow_rules_to_file(gen_dir, tables, p4_locks, hlir.p4_registers)
