#!/bin/bash
source ce_run.inc
INPUT_FILE=floating-leaves.jpg
OUTPUT_IFACE=emu11
CHANNEL=bpdu_gen_test

# External stuctures
ce_run channel.addChannel ${CHANNEL}

ce_run file.manage file0 ${INPUT_FILE}
ce_run file.openForReading file0

ce_run iface.activate ${OUTPUT_IFACE}

ce_run dll.load modFdReceiver
ce_run dll.load modAOS_B_PDU_Add
ce_run dll.load modAOS_VC_Gen
ce_run dll.load modAOS_AF_Gen
ce_run dll.load modEthTransmitter
ce_run dll.load modEthFrame_Add

echo "Setting up modEthTransmitter.add/ethtx0"
ce_run modEthTransmitter.add ${CHANNEL} ethtx0
ce_run modEthTransmitter.connectDevice ${CHANNEL} ethtx0 ${OUTPUT_IFACE}
ce_run modEthTransmitter.startup ${CHANNEL} ethtx0

echo "Setting up modEthFrame_Add/ethadd0"
ce_run modEthFrame_Add.add ${CHANNEL} ethadd0
ce_run modEthFrame_Add.getSrcMAC ${CHANNEL} ethadd0
ce_run modEthFrame_Add.setDstMAC ${CHANNEL} ethadd0 00:30:48:57:6a:d5
ce_run modEthFrame_Add.getDstMAC ${CHANNEL} ethadd0
ce_run modEthFrame_Add.connectOutput ${CHANNEL} ethadd0 ethtx0
ce_run modEthFrame_Add.startup ${CHANNEL} ethadd0
sleep 2

echo "Setting up modAOS_AF_Gen/afgen0"
ce_run modAOS_AF_Gen.add ${CHANNEL} afgen0
ce_run modAOS_AF_Gen.setFrameSize ${CHANNEL} afgen0 i/128
ce_run modAOS_AF_Gen.connectOutput ${CHANNEL} afgen0 ethadd0
ce_run modAOS_AF_Gen.startup ${CHANNEL} afgen0
sleep 2

echo "Setting up modAOS_VC_Gen/vcadd0"
ce_run modAOS_VC_Gen.add ${CHANNEL} vcadd0
ce_run modAOS_VC_Gen.setFrameSize ${CHANNEL} vcadd0 i/128
ce_run modAOS_VC_Gen.setSCID ${CHANNEL} vcadd0 i/171
ce_run modAOS_VC_Gen.setVCID ${CHANNEL} vcadd0 i/1
ce_run modAOS_VC_Gen.setUseOperationalControlField ${CHANNEL} vcadd0 b/true
ce_run modAOS_VC_Gen.connectOutput ${CHANNEL} vcadd0 afgen0
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
