/* -*- C++ -*- */

//=============================================================================
/**
 * @file   printers.hpp
 * @author Tad Kollar  
 *
 * $Id: printers.hpp 2002 2013-05-31 18:03:06Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#ifndef _NASA_PRINTERS_HPP_
#define _NASA_PRINTERS_HPP_

#include "IPv4_UDP_Datagram.hpp"
#include "EncapsulationPacket.hpp"
#include "AOS_Multiplexing_PDU.hpp"
#include "AOS_Transfer_Frame.hpp"
#include "TM_Transfer_Frame.hpp"
#include <iostream>
#include <iomanip>

using namespace std;

namespace nasaCE {

void print_udp_info(IPv4_UDP_Datagram* dgm) {
	cout << endl
		 << "*** IPv4 UDP DATAGRAM BREAKDOWN ***" << endl
		 << "___ IPv4 SECTION ___" << endl
		 << "   Packet Type: " << dgm->typeStr() << endl
		 << "    IP Version: " << dec << (int) dgm->getVersion() << endl
		 << "  Header Words: " << dec << (int) dgm->getHeaderWords() << endl
		 << "  Header Bytes: " << dec << (int) dgm->IPv4Packet::getHeaderLength() << endl
		 << "     TOS Value: " << hex << (int) dgm->getTOS() << endl
		 << "  Total Length: " << dec << dgm->getTotalLength() << endl
		 << "      Identity: " << hex << dgm->getIdent() << endl
		 << "         Flags: " << hex << dgm->getFlags() << endl
		 << "        Offset: " << dec << dgm->getOffset() << endl
		 << "           TTL: " << dec << (int) dgm->getTTL() << endl
		 << " Protocol Type: " << hex << (int) dgm->getProtocol() << endl
		 << "      Checksum: " << hex << uppercase << setw(4) << setfill('0') << dgm->getChecksum() << endl
		 << "     Source IP: " << dgm->getSrcIP().get_quad() << endl
		 << "      Dest. IP: " << dgm->getDstIP().get_quad() << endl
		 << "   Data Length: " << dec << dgm->IPv4Packet::getDataLength() << endl
		 << "  Is Fragment?: " << ((dgm->isFragment())? "TRUE" : "FALSE") << endl
		 << "Checksum Okay?: " << ((dgm->hasValidChecksum())? "TRUE" : "FALSE") << endl
		 << endl << "___ UDP SECTION ___" << endl
		 << "      Src Port: " << dec << dgm->getSrcPort() << endl
		 << "     Dest Port: " << dec << dgm->getDstPort() << endl
		 << "        Length: " << dec << dgm->getUDPLength() << endl
		 << "    Stored CRC: " << hex << uppercase << setw(4) << setfill('0') << dgm->getUDPChecksum() << endl
		 << "  Computed CRC: " << hex << uppercase << setw(4) << setfill('0') << dgm->computeUDPChecksum() << endl
		 << "     CRC Valid? " << ((dgm->hasValidUDPChecksum())? "TRUE" : "FALSE") << endl
		 << "  Header Bytes: " << dec << (int) dgm->getHeaderLength() << endl
		 << " Exp. Data Len: " << dec << dgm->getExpectedDataLength() << endl
		 << "   Data Length: " << dec << dgm->getDataLength() << endl
		 << "    Truncated?: " << ((dgm->isTruncated())? "TRUE" : "FALSE") << endl;

	cout << endl;
};

void print_encap_info(EncapsulationPacketBase* pkt) {
	cout << endl
		 << "*** ENCAPSULATING PACKET BREAKDOWN ***" << endl
		 << "   Header octets: " << dec << (unsigned) pkt->getHeaderOctets() << endl
		 << "  Version Number: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) pkt->getPacketVersionNumber() << endl
		 << "     Protocol ID: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) pkt->getProtocolID() << endl
		 << "Length of Length: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) pkt->getLengthOfLength() << endl
		 << "   Actual Length: " << dec << pkt->getUnitLength() << endl
		 << "       Truncated? " << ((pkt->isTruncated())? "TRUE" : "FALSE") << endl;
		if ( pkt->hasUserDefinedField() )
	cout <<	" User Defined Fd: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) pkt->getUserDefinedField() << endl;
		if ( pkt->hasProtocolIDExtension() )
	cout <<	"Protocol ID Extn: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) pkt->getProtocolIDExtension() << endl;
		if ( pkt->hasCssdsDefinedField() )
	cout <<	"CCSDS Defined Fd: " << hex << uppercase << setw(4) << setfill('0') << (unsigned) pkt->getCcsdsDefinedField() << endl;
		if ( pkt->hasPacketLength() )
	cout <<	"   Packet Length: " << dec << (unsigned long) pkt->getPacketLength() << endl;
		if ( pkt->hasIPE() ) {
	cout << "  IPE byte count: " << dec << (unsigned) dynamic_cast<EncapsulationPacketWithIPE*>(pkt)->countIPEOctets() << endl
	 	 << "             IPE: " << hex << uppercase << setw(4) << setfill('0') <<
	 	 						(unsigned long long) dynamic_cast<EncapsulationPacketWithIPE*>(pkt)->getIPE() << endl;
	}
	cout << endl;
}

void print_mpdu_info(AOS_Multiplexing_PDU* mpdu) {
	cout << endl
		<< "*** AOS MPDU BREAKDOWN ***" << endl
		<< "  Entire Header: " << hex << uppercase << setw(4) << setfill('0') << (unsigned) mpdu->getHeader() << endl
		<< "     Rsvd Spare: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) mpdu->getRsvdSpare() << endl
		<< " 1st Header Ptr: " << hex << uppercase << setw(4) << setfill('0') << (unsigned) mpdu->getFirstHeaderIndex() << endl
		<< "Headerless Data: " << (mpdu->hasHeaderlessData()? "TRUE" : "FALSE") << endl
		<< "     Has Header: " << (mpdu->hasHeader()? "TRUE" : "FALSE") << endl
		<< "   Is Idle Data: " << (mpdu->isIdle()? "TRUE" : "FALSE") << endl
		<< "   Total Length: " << dec << mpdu->getUnitLength() << endl

		<< endl;
}

void print_transfer_frame_info(AOS_Transfer_Frame* frame) {
	cout << endl
		 << "*** AOS TRANSFER FRAME BREAKDOWN ***" << endl
		 << "   TF Version#: " << hex << uppercase << (unsigned) frame->getTFVN() << endl
		 << " Spacecraft ID: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) frame->getSpacecraftID() << endl
		 << "          MCID: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) frame->getMCID() << endl
		 << "          VCID: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) frame->getVirtualChannelID() << endl
		 << "     Idle Data? " << (frame->isIdle()? "TRUE" : "FALSE") << endl
		 << "         GVCID: " << hex << uppercase << setw(4) << setfill('0') << (unsigned) frame->getGVCID() << endl
		 << "VC Frame Count: " << dec << frame->getVCFrameCount() << endl
		 << "   Replay Flag: " << (frame->getReplayFlag()? "TRUE" : "FALSE") << endl
		 << " VC Cycle Flag: " << (frame->getVCFrameCountCycleUsageFlag()? "TRUE" : "FALSE") << endl
		 << "Reserved Spare: " << hex << uppercase << setw(2) << setfill('0') << (int) frame->getRsvdSpare() << endl
		 << "VC Count Cycle: " << dec << (unsigned) frame->getVCFrameCountCycle() << endl
		 << "(Entire SigFd): " << hex << uppercase << (unsigned) frame->getSignalingField() << endl;
		 if (frame->hasHeaderErrorControl()) cout << "Header ErrCntl: " << hex << uppercase << setw(frame->spanFrameHeaderErrorControl * 2) << setfill('0') << frame->hasHeaderErrorControl() << endl;
	cout << "Has Insrt Zone? " << (frame->getInSduLen()? "TRUE" : "FALSE") << endl;
		 if (frame->getInSduLen()) cout << "Insrt Zone Len: " << dec << frame->getInSduLen() << endl;
	cout << "   Data Length: " << dec << frame->getDataLength() << endl
		 << " Has Oper Ctrl? " << (frame->hasOperationalControl()? "TRUE" : "FALSE") << endl;
		 if (frame->hasOperationalControl())
	cout << "   Report Type: " << dec << (unsigned) frame->getReportType() << endl;
	cout << "Frame Err Ctrl? " << (frame->hasFrameErrorControl()? "TRUE" : "FALSE") << endl;
		 if (frame->hasFrameErrorControl())
	cout << "  CRC-16-CCITT: " << hex << uppercase << setw(frame->spanFrameErrorControlField) << setfill('0') << frame->getFrameErrorControl() << endl
		 << "   calc'ed CRC: " << hex << uppercase << setw(frame->spanFrameErrorControlField) << setfill('0') << frame->getFrameCRC16() << endl
		 << " Has Valid CRC: " << (frame->hasValidCRC()? "TRUE" : "FALSE") << endl;

	cout << endl;
}

void print_tm_transfer_frame_info(TM_Transfer_Frame* frame) {
	cout << endl
		 << "*** TM TRANSFER FRAME BREAKDOWN ***" << endl
		 << "   TF Version#: " << hex << uppercase << (unsigned) frame->getTFVN() << endl
		 << " Spacecraft ID: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) frame->getSpacecraftID() << endl
		 << "          MCID: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) frame->getMCID() << endl
		 << "          VCID: " << hex << uppercase << setw(2) << setfill('0') << (unsigned) frame->getVCID() << endl
		 << "          OCFF: " << (frame->getOCFF()? "TRUE" : "FALSE") << endl
		 << "MC Frame Count: " << dec << static_cast<int>(frame->getMCFrameCount()) << endl
		 << "VC Frame Count: " << dec << static_cast<int>(frame->getVCFrameCount()) << endl
		 << " Data Fld Stat: " << hex << uppercase << setw(4) << setfill('0') << (unsigned) frame->getDataFieldStat() << endl
		 << "     Sync Flag: " << (frame->getSynchronizationFlag()? "TRUE" : "FALSE") << endl
		 << "PacketOrdrFlag: " << (frame->getPacketOrderFlag()? "TRUE" : "FALSE") << endl
		 << "    Seg Len ID: " << hex << uppercase << setw(1) << (unsigned) frame->getSegmentLengthID() << endl
		 << "1st Header Ptr: " << hex << uppercase << setw(4) << setfill('0') << (unsigned) frame->getFirstHeaderPointer() << endl
		 << " No Pkt Start?: " << (frame->noPacketStart()? "TRUE" : "FALSE") << endl
		 << "    Only Idle?: " << (frame->onlyIdleData()? "TRUE" : "FALSE") << endl
		 << "   Data Length: " << dec << frame->getDataLength() << " octets" << endl
		 << " Has Oper Ctrl? " << (frame->hasOperationalControl()? "TRUE" : "FALSE") << endl
		 << "       isValid: " << dec << frame->isValid() << endl;
		 if (frame->hasOperationalControl()) cout <<
		    "   Report Type: " << dec << static_cast<int>(frame->getReportType()) << endl;
		 if (frame->hasFrameErrorControl()) cout <<
		    "  CRC-16-CCITT: " << hex << uppercase << setw(frame->spanFrameErrorControlField) << setfill('0') << frame->getFrameErrorControl() << endl
		 << "   calc'ed CRC: " << hex << uppercase << setw(frame->spanFrameErrorControlField) << setfill('0') << frame->getFrameCRC16() << endl
		 << " Has Valid CRC: " << (frame->hasValidCRC()? "TRUE" : "FALSE") << endl;

	cout << endl;
}

void print_clcw_info(TC_Comm_Link_Control_Word* clcw) {
	cout << endl
		 << "*** CLCW BREAKDOWN ***" << endl
		 << "Ctrl Word Type: " << clcw->getControlWordType() << endl
		 << "Version Number: " << clcw->getCLCWVersionNumber() << endl
		 << "        Status: " << hex << uppercase << (unsigned) clcw->getStatusField() << endl
		 << " COP In Effect: " << (unsigned) clcw->getCOPInEffect() << endl
		 << " VC Identifier: " << hex << uppercase << setw(2) << setfill('0') << clcw->getVirtualChannelIdentifier() << endl
		 << "   No RF Avail: " << (clcw->getNoRFAvailFlag()? "TRUE" : "FALSE") << endl
		 << "   No Bit Lock: " << (clcw->getNoBitLockFlag()? "TRUE" : "FALSE") << endl
		 << "       Lockout: " << (clcw->getLockoutFlag()? "TRUE" : "FALSE") << endl
		 << "          Wait: " << (clcw->getWaitFlag()? "TRUE" : "FALSE") << endl
		 << "    Retransmit: " << (clcw->getRetransmitFlag()? "TRUE" : "FALSE") << endl
		 << "FARM-B Counter: " << hex << uppercase << setw(2) << setfill('0') << clcw->getFARMBCounter() << endl
		 << "  Report Value: " << hex << uppercase << setw(2) << setfill('0') << clcw->getReportValue() << endl;

	cout << endl;
}

} // namespace nasaCE

#endif // _NASA_PRINTERS_HPP_
