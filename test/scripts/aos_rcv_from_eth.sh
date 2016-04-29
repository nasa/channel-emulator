#!/bin/bash
source ce_run.inc
OUTPUT_FILE=floating-leaves.reconstituted.jpg
INPUT_IFACE=grc0
CHANNEL=bpdu_rcv_test

# External stuctures
ce_run channel.addChannel ${CHANNEL}

ce_run file.manage file0 ${OUTPUT_FILE}
ce_run file.openForWriting file0

ce_run iface.activate ${INPUT_IFACE}

ce_run dll.load modFdTransmitter
ce_run dll.load modAOS_B_PDU_Remove
ce_run dll.load modAOS_VC_Rcv
ce_run dll.load modEthReceiver
ce_run dll.load modEthFrame_Remove
ce_run dll.load modAOS_AF_Rcv

echo "Setting up modFdTransmitter/filetx0"
ce_run modFdTransmitter.add ${CHANNEL} filetx0
ce_run modFdTransmitter.connectDevice ${CHANNEL} filetx0 file0
ce_run modFdTransmitter.startup ${CHANNEL} filetx0
sleep 2

echo "Setting up modAOS_B_PDU_Remove/bpdudel0"
ce_run modAOS_B_PDU_Remove.add ${CHANNEL} bpdudel0
ce_run modAOS_B_PDU_Remove.connectOutput ${CHANNEL} bpdudel0 filetx0
ce_run modAOS_B_PDU_Remove.startup ${CHANNEL} bpdudel0
sleep 2

echo "Setting up modAOS_VC_Rcv/vcrcv0"
ce_run modAOS_VC_Rcv.add ${CHANNEL} vcrcv0
ce_run modAOS_VC_Rcv.setFrameSize ${CHANNEL} vcrcv0 i/128
ce_run modAOS_VC_Rcv.setSCID ${CHANNEL} vcrcv0 i/171
ce_run modAOS_VC_Rcv.setVCID ${CHANNEL} vcrcv0 i/1
ce_run modAOS_VC_Rcv.setUseOperationalControlField ${CHANNEL} vcrcv0 b/true
ce_run modAOS_VC_Rcv.setServiceType ${CHANNEL} vcrcv0 Bitstream
ce_run modAOS_VC_Rcv.connectOutput ${CHANNEL} vcrcv0 bpdudel0
ce_run modAOS_VC_Rcv.startup ${CHANNEL} vcrcv0
sleep 2

echo "Setting up modAOS_AF_Rcv/afrcv0"
ce_run modAOS_AF_Rcv.add ${CHANNEL} afrcv0
ce_run modAOS_AF_Rcv.setFrameSize ${CHANNEL} afrcv0 i/128
ce_run modAOS_AF_Rcv.connectOutput ${CHANNEL} afrcv0 vcrcv0
ce_run modAOS_AF_Rcv.startup ${CHANNEL} afrcv0

echo "Setting up modEthFrame_Remove/ethdel0"
ce_run modEthFrame_Remove.add ${CHANNEL} ethdel0
ce_run modEthFrame_Remove.connectOutput ${CHANNEL} ethdel0 afrcv0
ce_run modEthFrame_Remove.startup ${CHANNEL} ethdel0
sleep 2

echo "Setting up modEthReceiver.add/ethrx0"
ce_run modEthReceiver.add ${CHANNEL} ethrx0
ce_run modEthReceiver.connectOutput ${CHANNEL} ethrx0 ethdel0
ce_run modEthReceiver.connectDevice ${CHANNEL} ethrx0 ${INPUT_IFACE}
ce_run modEthReceiver.startup ${CHANNEL} ethrx0
