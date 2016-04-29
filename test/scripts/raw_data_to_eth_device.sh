#!/bin/bash

source ce_run.inc
INPUT_FILE=floating-leaves.jpg
OUTPUT_DEV=emu01

# External stuctures
ce_run channel.addChannel test1

ce_run file.manage file0 ${INPUT_FILE}
ce_run file.openForReading file0

ce_run iface.activate ${OUTPUT_DEV}

ce_run dll.load modFdReceiver
ce_run dll.load modEthTransmitter
ce_run dll.load modUDP_Add
ce_run dll.load modEthFrame_Add

echo "Setting up modEthTransmitter/eth0tx"
ce_run modEthTransmitter.add test1 eth0tx
ce_run modEthTransmitter.connectDevice test1 eth0tx ${OUTPUT_DEV}
ce_run modEthTransmitter.startup test1 eth0tx
sleep 2

echo "Setting up modEthFrame_Add/ethadd0"
ce_run modEthFrame_Add.add test1 ethadd0
ce_run modEthFrame_Add.getSrcMAC test1 ethadd0
ce_run modEthFrame_Add.setDstMAC test1 ethadd0 00:30:48:57:6c:27
ce_run modEthFrame_Add.getDstMAC test1 ethadd0
ce_run modEthFrame_Add.connectOutput test1 ethadd0 eth0tx
ce_run modEthFrame_Add.startup test1 ethadd0
sleep 2

echo "Setting up modUDP_Add/udpadd0"
ce_run modUDP_Add.add test1 udpadd0
ce_run modUDP_Add.setSrcAddr test1 udpadd0 10.201.1.254
ce_run modUDP_Add.setSrcPort test1 udpadd0 i/54321
ce_run modUDP_Add.setDstAddr test1 udpadd0 10.201.1.1
ce_run modUDP_Add.setDstPort test1 udpadd0 i/12345
ce_run modUDP_Add.setUDPCRC test1 udpadd0 b/true
ce_run modUDP_Add.connectOutput test1 udpadd0 ethadd0
ce_run modUDP_Add.startup test1 udpadd0
sleep 2

echo "Setting up modFdReceiver/file0rx"
ce_run modFdReceiver.add test1 file0rx
ce_run modFdReceiver.connectDevice test1 file0rx file0
ce_run modFdReceiver.setMaxRead test1 file0rx i/1024
ce_run modFdReceiver.connectOutput test1 file0rx udpadd0
ce_run modFdReceiver.startup test1 file0rx
sleep 2
