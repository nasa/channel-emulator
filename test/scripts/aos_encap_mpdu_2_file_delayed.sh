#!/bin/bash
source ce_run.inc
INPUT_FILE=floating-leaves.jpg
OUTPUT_FILE=floating-leaves.jpg.Encap.M_PDU.AOS
CHANNEL=encap_mpdu_test

# External stuctures
ce_run channel.addChannel ${CHANNEL}

ce_run file.manage file0 ${INPUT_FILE}
ce_run file.openForReading file0

ce_run file.manage file1 ${OUTPUT_FILE}
ce_run file.openForWriting file1

ce_run dll.load modFdReceiver
ce_run dll.load modFdTransmitter
ce_run dll.load modUDP_Add
ce_run dll.load modEncapPkt_Add
ce_run dll.load modAOS_M_PDU_Add
ce_run dll.load modAOS_VC_Gen
ce_run dll.load modAOS_AF_Gen
ce_run dll.load modEmulateDelay

echo "Setting up modFdTransmitter/file1tx"
ce_run modFdTransmitter.add ${CHANNEL} file1tx
ce_run modFdTransmitter.connectDevice ${CHANNEL} file1tx file1
ce_run modFdTransmitter.startup ${CHANNEL} file1tx
sleep 2

echo "Setting up modEmulateDelay/delay0"
ce_run modEmulateDelay.add ${CHANNEL} delay0
ce_run modEmulateDelay.setDelaySeconds ${CHANNEL} delay0 d/5.4
ce_run modEmulateDelay.connectOutput ${CHANNEL} delay0 file1tx
ce_run modEmulateDelay.startup ${CHANNEL} delay0
sleep 2

echo "Setting up modAOS_AF_Gen/afgen0"
ce_run modAOS_AF_Gen.add ${CHANNEL} afgen0
ce_run modAOS_AF_Gen.setFrameSize ${CHANNEL} afgen0 i/128
ce_run modAOS_AF_Gen.setUseInsertZone ${CHANNEL} afgen0 b/true
ce_run modAOS_AF_Gen.setInsertZoneSize ${CHANNEL} afgen0 i/8
ce_run modAOS_AF_Gen.setUseFrameEC ${CHANNEL} afgen0 b/true
ce_run modAOS_AF_Gen.setPerformErrorControl ${CHANNEL} afgen0 b/true
ce_run modAOS_AF_Gen.connectOutput ${CHANNEL} afgen0 file1tx
ce_run modAOS_AF_Gen.startup ${CHANNEL} afgen0
sleep 2

echo "Setting up modAOS_VC_Gen/vcgen0"
ce_run modAOS_VC_Gen.add ${CHANNEL} vcgen0
ce_run modAOS_VC_Gen.setFrameSize ${CHANNEL} vcgen0 i/128
ce_run modAOS_VC_Gen.setSCID ${CHANNEL} vcgen0 i/171
ce_run modAOS_VC_Gen.setVCID ${CHANNEL} vcgen0 i/1
ce_run modAOS_VC_Gen.setUseInsertZone ${CHANNEL} vcgen0 b/true
ce_run modAOS_VC_Gen.setInsertZoneSize ${CHANNEL} vcgen0 i/8
ce_run modAOS_VC_Gen.setUseFrameEC ${CHANNEL} vcgen0 b/true
ce_run modAOS_VC_Gen.connectOutput ${CHANNEL} vcgen0 afgen0
ce_run modAOS_VC_Gen.startup ${CHANNEL} vcgen0
sleep 2

echo "Setting up modAOS_M_PDU_Remove/mpduadd0"
ce_run modAOS_M_PDU_Add.add ${CHANNEL} mpduadd0
ce_run modAOS_M_PDU_Add.connectOutput ${CHANNEL} mpduadd0 vcgen0
# ce_run modAOS_M_PDU_Add.setLength ${CHANNEL} mpduadd0 i/122
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
sleep 2
