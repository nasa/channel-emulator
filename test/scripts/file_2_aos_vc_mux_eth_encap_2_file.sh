#!/bin/bash

INPUT_FILE0=floating-leaves.jpg
INPUT_FILE1=alien-night.jpg
OUTPUT_FILE=AOS_vc_muxed.AOS.Enet
CHANNEL=aos_eth_encap

source ce_run.inc

# External stuctures
ce_run channel.addChannel ${CHANNEL}

ce_run file.manage infile0 ${INPUT_FILE0}
ce_run file.openForReading infile0

ce_run file.manage infile1 ${INPUT_FILE1}
ce_run file.openForReading infile1

ce_run file.manage outfile0 ${OUTPUT_FILE}
ce_run file.openForWriting outfile0

ce_run dll.load modFdReceiver
ce_run dll.load modFdTransmitter
ce_run dll.load modUDP_Add
ce_run dll.load modEthFrame_Add
ce_run dll.load modEncapPkt_Add
ce_run dll.load modAOS_M_PDU_Add
ce_run dll.load modAOS_VC_Gen
ce_run dll.load modAOS_VC_Mux

# -------------------------------------------------------------------
# Single output pipe config
# -------------------------------------------------------------------
echo "Setting up modFdTransmitter/outfile0tx"
ce_run modFdTransmitter.add ${CHANNEL} outfile0tx
ce_run modFdTransmitter.connectDevice ${CHANNEL} outfile0tx outfile0
ce_run modFdTransmitter.startup ${CHANNEL} outfile0tx
sleep 2

echo "Setting up modEthFrame_Add/ethadd0"
ce_run modEthFrame_Add.add ${CHANNEL} ethadd0
ce_run modEthFrame_Add.connectOutput ${CHANNEL} ethadd0 outfile0tx
ce_run modEthFrame_Add.startup ${CHANNEL} ethadd0
sleep 2

echo "Setting up modAOS_VC_Mux/vcmux0"
ce_run modAOS_VC_Mux.add ${CHANNEL} vcmux0
ce_run modAOS_VC_Mux.setDefaultPriority ${CHANNEL} vcmux0 i/1000
ce_run modAOS_VC_Mux.connectOutput ${CHANNEL} vcmux0 ethadd0
ce_run modAOS_VC_Mux.startup ${CHANNEL} vcmux0
sleep 2

# -------------------------------------------------------------------
# Input pipe #1 config
# -------------------------------------------------------------------
echo "Setting up modAOS_VC_Gen/vcadd0"
ce_run modAOS_VC_Gen.add ${CHANNEL} vcadd0
ce_run modAOS_VC_Gen.setFrameSize ${CHANNEL} vcadd0 i/128
ce_run modAOS_VC_Gen.setSCID ${CHANNEL} vcadd0 i/171
ce_run modAOS_VC_Gen.setVCID ${CHANNEL} vcadd0 i/1
ce_run modAOS_VC_Gen.setUseFrameEC ${CHANNEL} vcadd0 b/true
ce_run modAOS_VC_Gen.connectOutput ${CHANNEL} vcadd0 vcmux0
ce_run modAOS_VC_Gen.startup ${CHANNEL} vcadd0
sleep 2

echo "Setting up modAOS_M_PDU_Add/mpduadd0"
ce_run modAOS_M_PDU_Add.add ${CHANNEL} mpduadd0
ce_run modAOS_M_PDU_Add.connectOutput ${CHANNEL} mpduadd0 vcadd0
# ce_run modAOS_M_PDU_Add.setLength ${CHANNEL} mpduadd0 i/120
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

echo "Setting up modFdReceiver/infile0rx"
ce_run modFdReceiver.add ${CHANNEL} infile0rx
ce_run modFdReceiver.connectDevice ${CHANNEL} infile0rx infile0
ce_run modFdReceiver.setMaxRead ${CHANNEL} infile0rx i/1024
ce_run modFdReceiver.connectOutput ${CHANNEL} infile0rx udpadd0

# -------------------------------------------------------------------
# Input pipe #2 config
# -------------------------------------------------------------------
echo "Setting up modAOS_VC_Gen/vcadd1"
ce_run modAOS_VC_Gen.add ${CHANNEL} vcadd1
ce_run modAOS_VC_Gen.setFrameSize ${CHANNEL} vcadd1 i/128
ce_run modAOS_VC_Gen.setSCID ${CHANNEL} vcadd1 i/171
ce_run modAOS_VC_Gen.setVCID ${CHANNEL} vcadd1 i/2
ce_run modAOS_VC_Gen.setUseFrameEC ${CHANNEL} vcadd1 b/true
ce_run modAOS_VC_Gen.connectOutput ${CHANNEL} vcadd1 vcmux0
ce_run modAOS_VC_Gen.startup ${CHANNEL} vcadd1
sleep 2

echo "Setting up modAOS_M_PDU_Add/mpduadd1"
ce_run modAOS_M_PDU_Add.add ${CHANNEL} mpduadd1
ce_run modAOS_M_PDU_Add.connectOutput ${CHANNEL} mpduadd1 vcadd1
# ce_run modAOS_M_PDU_Add.setLength ${CHANNEL} mpduadd1 i/120
ce_run modAOS_M_PDU_Add.startup ${CHANNEL} mpduadd1
sleep 2

echo "Setting up modEncapPkt_Add/encapadd1"
ce_run modEncapPkt_Add.add ${CHANNEL} encapadd1
ce_run modEncapPkt_Add.connectOutput ${CHANNEL} encapadd1 mpduadd1
ce_run modEncapPkt_Add.startup ${CHANNEL} encapadd1
sleep 2

echo "Setting up modUDP_Add/udpadd1"
ce_run modUDP_Add.add ${CHANNEL} udpadd1
ce_run modUDP_Add.setSrcAddr ${CHANNEL} udpadd1 10.10.1.100
ce_run modUDP_Add.setSrcPort ${CHANNEL} udpadd1 i/22222
ce_run modUDP_Add.setDstAddr ${CHANNEL} udpadd1 10.10.1.101
ce_run modUDP_Add.setDstPort ${CHANNEL} udpadd1 i/33333
ce_run modUDP_Add.setUDPCRC ${CHANNEL} udpadd1 b/true
ce_run modUDP_Add.connectOutput ${CHANNEL} udpadd1 encapadd1
ce_run modUDP_Add.startup ${CHANNEL} udpadd1
sleep 2

echo "Setting up modFdReceiver/infile1rx"
ce_run modFdReceiver.add ${CHANNEL} infile1rx
ce_run modFdReceiver.connectDevice ${CHANNEL} infile1rx infile1
ce_run modFdReceiver.setMaxRead ${CHANNEL} infile1rx i/1024
ce_run modFdReceiver.connectOutput ${CHANNEL} infile1rx udpadd1

# -------------------------------------------------------------------
# Allow input to flow
# -------------------------------------------------------------------
ce_run modFdReceiver.startup ${CHANNEL} infile0rx
ce_run modFdReceiver.startup ${CHANNEL} infile1rx
