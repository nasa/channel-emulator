#!/bin/bash
source ce_run.inc
INPUT_IFACE=dummy0
OUTPUT_FILE=ethernet_payload.AOS

# External stuctures
ce_run channel.addChannel test1

ce_run iface.activate $INPUT_IFACE

ce_run file.manage file0 $OUTPUT_FILE
ce_run file.openForWriting file0

ce_run dll.load modEthReceiver
ce_run dll.load modEthFrame_Remove
ce_run dll.load modFdTransmitter
ce_run dll.load modEncapPkt_Add
ce_run dll.load modAOS_M_PDU_Add
ce_run dll.load modAOS_VC_Gen

echo "Setting up modFdTransmitter/file0tx"
ce_run modFdTransmitter.add test1 file0tx
ce_run modFdTransmitter.connectDevice test1 file0tx file0
ce_run modFdTransmitter.startup test1 file0tx
sleep 2

echo "Setting up modAOS_VC_Gen/vcadd0"
ce_run modAOS_VC_Gen.add test1 vcadd0
ce_run modAOS_VC_Gen.setFrameSize test1 vcadd0 i/128
ce_run modAOS_VC_Gen.setSCID test1 vcadd0 i/171
ce_run modAOS_VC_Gen.setVCID test1 vcadd0 i/1
ce_run modAOS_VC_Gen.connectOutput test1 vcadd0 file0tx
ce_run modAOS_VC_Gen.startup test1 vcadd0
sleep 2

echo "Setting up modAOS_M_PDU_Remove/mpduadd0"
ce_run modAOS_M_PDU_Add.add test1 mpduadd0
ce_run modAOS_M_PDU_Add.connectOutput test1 mpduadd0 vcadd0
ce_run modAOS_M_PDU_Add.setLength test1 mpduadd0 i/122
ce_run modAOS_M_PDU_Add.startup test1 mpduadd0
sleep 2

echo "Setting up modEncapPkt_Add/encapadd0"
ce_run modEncapPkt_Add.add test1 encapadd0
ce_run modEncapPkt_Add.connectOutput test1 encapadd0 mpduadd0
ce_run modEncapPkt_Add.startup test1 encapadd0
sleep 2

echo "Setting up modEthFrame_Remove/ethdel0"
ce_run modEthFrame_Remove.add test1 ethdel0
ce_run modEthFrame_Remove.connectOutput test1 ethdel0 encapadd0
ce_run modEthFrame_Remove.startup test1 ethdel0
sleep 2

echo "Setting up modEthReceiver/eth0rx"
ce_run modEthReceiver.add test1 eth0rx
ce_run modEthReceiver.connectDevice test1 eth0rx $INPUT_IFACE
ce_run modEthReceiver.connectOutput test1 eth0rx ethdel0
ce_run modEthReceiver.startup test1 eth0rx
sleep 2
