#!/bin/bash

INPUT_FILE=floating-leaves.jpg
OUTPUT_FILE=floating-leaves.jpg.AOS.Enet
CHANNEL=aos_eth_encap

source ce_run.inc

# External stuctures
ce_run channel.addChannel ${CHANNEL}

ce_run file.manage file0 ${INPUT_FILE}
ce_run file.openForReading file0

ce_run file.manage file1 ${OUTPUT_FILE}
ce_run file.openForWriting file1

ce_run dll.load modFdReceiver
ce_run dll.load modFdTransmitter
ce_run dll.load modUDP_Add
ce_run dll.load modEthFrame_Add
ce_run dll.load modEncapPkt_Add
ce_run dll.load modAOS_M_PDU_Add
ce_run dll.load modAOS_VC_Gen

echo "Setting up modFdTransmitter/file1tx"
ce_run modFdTransmitter.add ${CHANNEL} file1tx
ce_run modFdTransmitter.connectDevice ${CHANNEL} file1tx file1
ce_run modFdTransmitter.startup ${CHANNEL} file1tx
sleep 2

echo "Setting up modEthFrame_Add/ethadd0"
ce_run modEthFrame_Add.add ${CHANNEL} ethadd0
ce_run modEthFrame_Add.connectOutput ${CHANNEL} ethadd0 file1tx
ce_run modEthFrame_Add.startup ${CHANNEL} ethadd0
sleep 2

echo "Setting up modAOS_VC_Gen/vcadd0"
ce_run modAOS_VC_Gen.add ${CHANNEL} vcadd0
ce_run modAOS_VC_Gen.setFrameSize ${CHANNEL} vcadd0 i/128
ce_run modAOS_VC_Gen.setSCID ${CHANNEL} vcadd0 i/171
ce_run modAOS_VC_Gen.setVCID ${CHANNEL} vcadd0 i/1
ce_run modAOS_VC_Gen.setUseFrameCRC ${CHANNEL} vcadd0 b/true
ce_run modAOS_VC_Gen.connectOutput ${CHANNEL} vcadd0 ethadd0
ce_run modAOS_VC_Gen.startup ${CHANNEL} vcadd0
sleep 2

echo "Setting up modAOS_M_PDU_Add/mpduadd0"
ce_run modAOS_M_PDU_Add.add ${CHANNEL} mpduadd0
ce_run modAOS_M_PDU_Add.connectOutput ${CHANNEL} mpduadd0 vcadd0
ce_run modAOS_M_PDU_Add.setLength ${CHANNEL} mpduadd0 i/122
ce_run modAOS_M_PDU_Add.startup ${CHANNEL} mpduadd0
sleep 2

echo "Setting up modEncapPkt_Add/encapadd0"
ce_run modEncapPkt_Add.add ${CHANNEL} encapadd0
ce_run modEncapPkt_Add.connectOutput ${CHANNEL} encapadd0 mpduadd0
ce_run modEncapPkt_Add.startup ${CHANNEL} encapadd0
sleep 2

echo "Setting up modUDP_Add/udpadd0"
ce_run modUDP_Add.add ${CHANNEL} udpadd0
ce_run modUDP_Add.setSrcAddr ${CHANNEL} udpadd0 10.10.1.1
ce_run modUDP_Add.setSrcPort ${CHANNEL} udpadd0 i/12345
ce_run modUDP_Add.setDstAddr ${CHANNEL} udpadd0 10.10.1.2
ce_run modUDP_Add.setDstPort ${CHANNEL} udpadd0 i/54321
ce_run modUDP_Add.setUDPCRC ${CHANNEL} udpadd0 b/true
ce_run modUDP_Add.connectOutput ${CHANNEL} udpadd0 encapadd0
ce_run modUDP_Add.startup ${CHANNEL} udpadd0
sleep 2

echo "Setting up modFdReceiver/file0rx"
ce_run modFdReceiver.add ${CHANNEL} file0rx
ce_run modFdReceiver.connectDevice ${CHANNEL} file0rx file0
ce_run modFdReceiver.setMaxRead ${CHANNEL} file0rx i/1024
ce_run modFdReceiver.connectOutput ${CHANNEL} file0rx udpadd0
ce_run modFdReceiver.startup ${CHANNEL} file0rx
