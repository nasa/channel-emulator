<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_VC_Rcv</name>
  <title>Virtual Channel Reception Function</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>This module accepts AOS Transfer Frames and extracts the payload from the Data Field. The segment must be explicitly configured to recognize the data structure in that field (unlike the reverse function, VC Generation, which will accept any type of data that is the correct length). That type can be one of either a Multiplexing PDU (M_PDU), Bitstream PDU (B_PDU), Virtual Channel Access PDU (VCA_PDU), or Idle data. The Data Field buffer is put into the appropriate type of wrapper and sent via the primary output.</desc>
  <config>
    <ref>
		<id>AOS_Common_PC_Settings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.AOS_PhysicalChannel.idlePattern</ignore>
    </ref>
    <ref>
		<id>AOS_Common_MC_Settings</id>
    </ref>
    <ref>
		<id>AOS_Common_VC_Settings</id>
    </ref>
	<ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
    </ref>
  </config>
</modinfo>
