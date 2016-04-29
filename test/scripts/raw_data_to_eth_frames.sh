#!/bin/bash

INPUT_FILE=floating-leaves.jpg
OUTPUT_FILE=floating-leaves.jpg.enet
CHANNEL=test1

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

echo "Setting up modUDP_Add/udpadd0"
ce_run modUDP_Add.add ${CHANNEL} udpadd0
ce_run modUDP_Add.setSrcAddr ${CHANNEL} udpadd0 10.10.1.1
ce_run modUDP_Add.setSrcPort ${CHANNEL} udpadd0 i/12345
ce_run modUDP_Add.setDstAddr ${CHANNEL} udpadd0 10.10.1.2
ce_run modUDP_Add.setDstPort ${CHANNEL} udpadd0 i/54321
ce_run modUDP_Add.setUDPCRC ${CHANNEL} udpadd0 b/true
ce_run modUDP_Add.connectOutput ${CHANNEL} udpadd0 ethadd0
ce_run modUDP_Add.startup ${CHANNEL} udpadd0
sleep 2

echo "Setting up modFdReceiver/file0rx"
ce_run modFdReceiver.add ${CHANNEL} file0rx
ce_run modFdReceiver.connectDevice ${CHANNEL} file0rx file0
ce_run modFdReceiver.setMaxRead ${CHANNEL} file0rx i/1024
ce_run modFdReceiver.connectOutput ${CHANNEL} file0rx udpadd0
ce_run modFdReceiver.startup ${CHANNEL} file0rx

echo -n "Allowing 5 seconds before deleting channel ${CHANNEL}"
for i in `seq 1 5`; do echo -n "."; sleep 1; done

echo ""
ce_run channel.removeSegment ${CHANNEL} file0rx
ce_run channel.removeSegment ${CHANNEL} udpadd0
ce_run channel.removeSegment ${CHANNEL} ethadd0
ce_run channel.removeSegment ${CHANNEL} file1tx

ce_run channel.removeChannel ${CHANNEL}

ce_run dll.unload modFdReceiver
ce_run dll.unload modFdTransmitter
ce_run dll.unload modUDP_Add
ce_run dll.unload modEthFrame_Add

ce_run file.close file1
ce_run file.unmanage file1

ce_run file.close file0
ce_run file.unmanage file0
