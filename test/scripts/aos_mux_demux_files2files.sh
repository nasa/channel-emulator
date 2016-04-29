#!/bin/bash

INPUT_FILE0=floating-leaves.jpg
INPUT_FILE1=alien-night.jpg
OUTPUT_FILE0=floating-leaves.reconstituted.jpg
OUTPUT_FILE1=alien-night.reconstituted.jpg
CHANNEL=aosMuxDemuxTest
AOS_FRAME_SIZE=128
SCID=171
WAIT_SECS=2

source ce_run.inc

# External stuctures
ce_run channel.addChannel ${CHANNEL}

ce_run file.manage infile0 ${INPUT_FILE0}
ce_run file.openForReading infile0

ce_run file.manage infile1 ${INPUT_FILE1}
ce_run file.openForReading infile1

ce_run file.manage outfile0 ${OUTPUT_FILE0}
ce_run file.openForWriting outfile0

ce_run file.manage outfile1 ${OUTPUT_FILE1}
ce_run file.openForWriting outfile1

ce_run dll.load modFdReceiver
ce_run dll.load modFdTransmitter
ce_run dll.load modUDP_Add
ce_run dll.load modUDP_Remove
ce_run dll.load modEncapPkt_Add
ce_run dll.load modEncapPkt_Remove
ce_run dll.load modAOS_M_PDU_Add
ce_run dll.load modAOS_M_PDU_Remove
ce_run dll.load modAOS_VC_Gen
ce_run dll.load modAOS_VC_Rcv
ce_run dll.load modAOS_VC_Mux
ce_run dll.load modAOS_VC_Demux
ce_run dll.load modAOS_AF_Gen
ce_run dll.load modAOS_AF_Rcv

# -------------------------------------------------------------------
# Output pipe #1 config
# -------------------------------------------------------------------
echo "Setting up modFdTransmitter/outfile0tx"
ce_run modFdTransmitter.add ${CHANNEL} outfile0tx
ce_run modFdTransmitter.connectDevice ${CHANNEL} outfile0tx outfile0
ce_run modFdTransmitter.startup ${CHANNEL} outfile0tx
sleep ${WAIT_SECS}

echo "Setting up modUDP_Remove/udpdel0"
ce_run modUDP_Remove.add ${CHANNEL} udpdel0
ce_run modUDP_Remove.connectOutput ${CHANNEL} udpdel0 outfile0tx
ce_run modUDP_Remove.startup ${CHANNEL} udpdel0
sleep ${WAIT_SECS}

echo "Setting up modEncapPkt_Remove/encapdel0"
ce_run modEncapPkt_Remove.add ${CHANNEL} encapdel0
ce_run modEncapPkt_Remove.connectOutput ${CHANNEL} encapdel0 udpdel0
ce_run modEncapPkt_Remove.startup ${CHANNEL} encapdel0
sleep ${WAIT_SECS}

echo "Setting up modAOS_M_PDU_Remove/mpdudel0"
ce_run modAOS_M_PDU_Remove.add ${CHANNEL} mpdudel0
ce_run modAOS_M_PDU_Remove.connectOutput ${CHANNEL} mpdudel0 encapdel0
ce_run modAOS_M_PDU_Remove.startup ${CHANNEL} mpdudel0
sleep ${WAIT_SECS}

echo "Setting up modAOS_VC_Rcv/vcrcv0"
ce_run modAOS_VC_Rcv.add ${CHANNEL} vcrcv0
ce_run modAOS_VC_Rcv.connectOutput ${CHANNEL} vcrcv0 mpdudel0
ce_run modAOS_VC_Rcv.startup ${CHANNEL} vcrcv0
sleep ${WAIT_SECS}

# -------------------------------------------------------------------
# Output pipe #2 config
# -------------------------------------------------------------------
echo "Setting up modFdTransmitter/outfile1tx"
ce_run modFdTransmitter.add ${CHANNEL} outfile1tx
ce_run modFdTransmitter.connectDevice ${CHANNEL} outfile1tx outfile1
ce_run modFdTransmitter.startup ${CHANNEL} outfile1tx
sleep ${WAIT_SECS}

echo "Setting up modUDP_Remove/udpdel1"
ce_run modUDP_Remove.add ${CHANNEL} udpdel1
ce_run modUDP_Remove.connectOutput ${CHANNEL} udpdel1 outfile1tx
ce_run modUDP_Remove.startup ${CHANNEL} udpdel1
sleep ${WAIT_SECS}

echo "Setting up modEncapPkt_Remove/encapdel1"
ce_run modEncapPkt_Remove.add ${CHANNEL} encapdel1
ce_run modEncapPkt_Remove.connectOutput ${CHANNEL} encapdel1 udpdel1
ce_run modEncapPkt_Remove.startup ${CHANNEL} encapdel1
sleep ${WAIT_SECS}

echo "Setting up modAOS_M_PDU_Remove/mpdudel1"
ce_run modAOS_M_PDU_Remove.add ${CHANNEL} mpdudel1
ce_run modAOS_M_PDU_Remove.connectOutput ${CHANNEL} mpdudel1 encapdel1
ce_run modAOS_M_PDU_Remove.startup ${CHANNEL} mpdudel1
sleep ${WAIT_SECS}

echo "Setting up modAOS_VC_Rcv/vcrcv1"
ce_run modAOS_VC_Rcv.add ${CHANNEL} vcrcv1
ce_run modAOS_VC_Rcv.connectOutput ${CHANNEL} vcrcv1 mpdudel1
ce_run modAOS_VC_Rcv.startup ${CHANNEL} vcrcv1
sleep ${WAIT_SECS}

# -------------------------------------------------------------------
# Mux/demux, All Frames Gen/Rcv config
# -------------------------------------------------------------------
echo "Setting up modAOS_VC_Demux/vcdmux0"
ce_run modAOS_VC_Demux.add ${CHANNEL} vcdmux0
ce_run modAOS_VC_Demux.connectOutput ${CHANNEL} vcdmux0 i/1 vcrcv0
ce_run modAOS_VC_Demux.connectOutput ${CHANNEL} vcdmux0 i/2 vcrcv1
ce_run modAOS_VC_Demux.startup ${CHANNEL} vcdmux0
sleep ${WAIT_SECS}

echo "Setting up modAOS_AF_Rcv/afrcv"
ce_run modAOS_AF_Rcv.add ${CHANNEL} afrcv
ce_run modAOS_AF_Rcv.connectOutput ${CHANNEL} afrcv vcdmux0
ce_run modAOS_AF_Rcv.startup ${CHANNEL} afrcv
sleep ${WAIT_SECS}

# TODO: good place to test delay, BER, etc.

echo "Setting up modAOS_AF_Gen/afgen"
ce_run modAOS_AF_Gen.add ${CHANNEL} afgen
ce_run modAOS_AF_Gen.connectOutput ${CHANNEL} afgen afrcv
ce_run modAOS_AF_Gen.startup ${CHANNEL} afgen
sleep ${WAIT_SECS}

echo "Setting up modAOS_VC_Mux/vcmux0"
ce_run modAOS_VC_Mux.add ${CHANNEL} vcmux0
ce_run modAOS_VC_Mux.connectOutput ${CHANNEL} vcmux0 afgen
ce_run modAOS_VC_Mux.startup ${CHANNEL} vcmux0
sleep ${WAIT_SECS}

# -------------------------------------------------------------------
# Input pipe #1 config
# -------------------------------------------------------------------
echo "Setting up modAOS_VC_Gen/vcgen0"
ce_run modAOS_VC_Gen.add ${CHANNEL} vcgen0
ce_run modAOS_VC_Gen.connectOutput ${CHANNEL} vcgen0 vcmux0
ce_run modAOS_VC_Gen.startup ${CHANNEL} vcgen0
sleep ${WAIT_SECS}

echo "Setting up modAOS_M_PDU_Add/mpduadd0"
ce_run modAOS_M_PDU_Add.add ${CHANNEL} mpduadd0
ce_run modAOS_M_PDU_Add.connectOutput ${CHANNEL} mpduadd0 vcgen0
ce_run modAOS_M_PDU_Add.startup ${CHANNEL} mpduadd0
sleep ${WAIT_SECS}

echo "Setting up modEncapPkt_Add/encapadd0"
ce_run modEncapPkt_Add.add ${CHANNEL} encapadd0
ce_run modEncapPkt_Add.connectOutput ${CHANNEL} encapadd0 mpduadd0
ce_run modEncapPkt_Add.startup ${CHANNEL} encapadd0
sleep ${WAIT_SECS}

echo "Setting up modUDP_Add/udpadd0"
ce_run modUDP_Add.add ${CHANNEL} udpadd0
ce_run modUDP_Add.setSrcAddr ${CHANNEL} udpadd0 10.10.1.1
ce_run modUDP_Add.setSrcPort ${CHANNEL} udpadd0 i/12345
ce_run modUDP_Add.setDstAddr ${CHANNEL} udpadd0 10.10.1.2
ce_run modUDP_Add.setDstPort ${CHANNEL} udpadd0 i/54321
ce_run modUDP_Add.setUDPCRC ${CHANNEL} udpadd0 b/true
ce_run modUDP_Add.connectOutput ${CHANNEL} udpadd0 encapadd0
ce_run modUDP_Add.startup ${CHANNEL} udpadd0
sleep ${WAIT_SECS}

echo "Setting up modFdReceiver/infile0rx"
ce_run modFdReceiver.add ${CHANNEL} infile0rx
ce_run modFdReceiver.connectDevice ${CHANNEL} infile0rx infile0
ce_run modFdReceiver.setMaxRead ${CHANNEL} infile0rx i/1024
ce_run modFdReceiver.connectOutput ${CHANNEL} infile0rx udpadd0

# -------------------------------------------------------------------
# Input pipe #2 config
# -------------------------------------------------------------------
echo "Setting up modAOS_VC_Gen/vcgen1"
ce_run modAOS_VC_Gen.add ${CHANNEL} vcgen1
ce_run modAOS_VC_Gen.connectOutput ${CHANNEL} vcgen1 vcmux0
ce_run modAOS_VC_Gen.startup ${CHANNEL} vcgen1
sleep ${WAIT_SECS}

echo "Setting up modAOS_M_PDU_Add/mpduadd1"
ce_run modAOS_M_PDU_Add.add ${CHANNEL} mpduadd1
ce_run modAOS_M_PDU_Add.connectOutput ${CHANNEL} mpduadd1 vcgen1
ce_run modAOS_M_PDU_Add.startup ${CHANNEL} mpduadd1
sleep ${WAIT_SECS}

echo "Setting up modEncapPkt_Add/encapadd1"
ce_run modEncapPkt_Add.add ${CHANNEL} encapadd1
ce_run modEncapPkt_Add.connectOutput ${CHANNEL} encapadd1 mpduadd1
ce_run modEncapPkt_Add.startup ${CHANNEL} encapadd1
sleep ${WAIT_SECS}

echo "Setting up modUDP_Add/udpadd1"
ce_run modUDP_Add.add ${CHANNEL} udpadd1
ce_run modUDP_Add.setSrcAddr ${CHANNEL} udpadd1 10.10.1.100
ce_run modUDP_Add.setSrcPort ${CHANNEL} udpadd1 i/22222
ce_run modUDP_Add.setDstAddr ${CHANNEL} udpadd1 10.10.1.101
ce_run modUDP_Add.setDstPort ${CHANNEL} udpadd1 i/33333
ce_run modUDP_Add.setUDPCRC ${CHANNEL} udpadd1 b/true
ce_run modUDP_Add.connectOutput ${CHANNEL} udpadd1 encapadd1
ce_run modUDP_Add.startup ${CHANNEL} udpadd1
sleep ${WAIT_SECS}

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

sleep 2
ce_run modAOS_VC_Mux.getValidFrameCount ${CHANNEL} vcmux0
