#!/bin/bash
source ce_run.inc

CHANNEL=test1
IFACE=emu01

ce_run channel.addChannel ${CHANNEL}

ce_run iface.activate ${IFACE}

ce_run file.manage file1 eth_frames.bin
ce_run file.openForWriting file1

ce_run dll.load modFdTransmitter
ce_run dll.load modEthReceiver

# channel configuration
ce_run modFdTransmitter.add ${CHANNEL} filetx0
ce_run modFdTransmitter.connectDevice ${CHANNEL} filetx0 file1
ce_run modFdTransmitter.startup ${CHANNEL} filetx0

ce_run modEthReceiver.add ${CHANNEL} ethrx0
ce_run modEthReceiver.connectDevice ${CHANNEL} ethrx0 ${IFACE}
ce_run modEthReceiver.connectOutput ${CHANNEL} ethrx0 filetx0
ce_run modEthReceiver.startup ${CHANNEL} ethrx0


