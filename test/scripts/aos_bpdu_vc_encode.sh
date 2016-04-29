#!/bin/bash
source ce_run.inc
INPUT_FILE=floating-leaves.jpg
OUTPUT_FILE=floating-leaves.jpg.B_PDU.AOS
CHANNEL=bpdu_test

# External stuctures
ce_run channel.addChannel bpdu_test

ce_run file.manage file0 ${INPUT_FILE}
ce_run file.openForReading file0

ce_run file.manage file1 ${OUTPUT_FILE}
ce_run file.openForWriting file1

ce_run dll.load modFdReceiver
ce_run dll.load modFdTransmitter
ce_run dll.load modAOS_B_PDU_Add
ce_run dll.load modAOS_VC_Gen

echo "Setting up modFdTransmitter/file1tx"
ce_run modFdTransmitter.add ${CHANNEL} file1tx
ce_run modFdTransmitter.connectDevice ${CHANNEL} file1tx file1
ce_run modFdTransmitter.startup ${CHANNEL} file1tx
sleep 2

echo "Setting up modAOS_VC_Gen/vcadd0"
ce_run modAOS_VC_Gen.add ${CHANNEL} vcadd0
ce_run modAOS_VC_Gen.setFrameSize ${CHANNEL} vcadd0 i/128
ce_run modAOS_VC_Gen.setSCID ${CHANNEL} vcadd0 i/171
ce_run modAOS_VC_Gen.setVCID ${CHANNEL} vcadd0 i/1
ce_run modAOS_VC_Gen.setUseOperationalControlField ${CHANNEL} vcadd0 b/true
ce_run modAOS_VC_Gen.connectOutput ${CHANNEL} vcadd0 file1tx
ce_run modAOS_VC_Gen.startup ${CHANNEL} vcadd0
sleep 2

echo "Setting up modAOS_B_PDU_Remove/bpduadd0"
ce_run modAOS_B_PDU_Add.add ${CHANNEL} bpduadd0
ce_run modAOS_B_PDU_Add.connectOutput ${CHANNEL} bpduadd0 vcadd0
# ce_run modAOS_B_PDU_Add.setLength ${CHANNEL} bpduadd0 i/122
ce_run modAOS_B_PDU_Add.startup ${CHANNEL} bpduadd0
sleep 2

echo "Setting up modFdReceiver/file0rx"
ce_run modFdReceiver.add ${CHANNEL} file0rx
ce_run modFdReceiver.connectDevice ${CHANNEL} file0rx file0
ce_run modFdReceiver.setMaxRead ${CHANNEL} file0rx i/1024
ce_run modFdReceiver.connectOutput ${CHANNEL} file0rx bpduadd0
ce_run modFdReceiver.startup ${CHANNEL} file0rx
sleep 2
