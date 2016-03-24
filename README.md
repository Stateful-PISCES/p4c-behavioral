p4c-behavioral
========

P4 compiler for the behavioral model (BM) target

Pre-requisites:  
Install p4-hlir (https://github.com/barefootnetworks/p4-hlir)

To install:  
sudo python setup.py install

To run:  
p4c-behavioral \<path_to_p4_program\> --gen-dir \<dir_for_generated_files\> [--thrift]  

For more info:  
p4c-behavioral --help  

p4c-bm is an alias for p4c-behavioral


Usage of flow type checker

./flow_type_checker.py <path_to_generated_p4_dict> --add-flow --br br0 --match "in_port=1,table=0,priority=32768,ethernet__etherType=0x800" --action "action=set_field:6->reg0,resubmit(1, 2),set_field:0x111112131418->ethernet__srcAddr,calc_fields_update(ipv4__hdrChecksum,csum16,fields:ipv4__version_ihl,ipv4__diffserv,ipv4__totalLen,ipv4__identification,ipv4__flags_fragOffset,ipv4__ttl,ipv4__protocol,ipv4__srcAddr,ipv4__dstAddr)" --protocols Openflow15 --ofctl-path <path_to_ofctl>
