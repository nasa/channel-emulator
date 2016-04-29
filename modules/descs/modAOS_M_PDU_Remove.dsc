<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_M_PDU_Remove</name>
  <title>Packet Processing Function</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>This service accepts fixed-length Multiplexing Protocol Data Units (M_PDUs), with Packet Zones that contain variable-length Encapsulation Packets. A single M_PDU may carry a partial or whole Packet, or multiples, depending on the relative lengths; there may also be idle data if no incoming traffic was available to fill the remainder of the Packet Zone.[+br/+][+br/+]Encapsulation Packets are extracted from the Packet Zone buffers and reassembled in their own buffer, then wrapped and sent via the primary output to the next segment, which typically removes the Packet header.</desc>
  <config>
	<setting>
      <prompt>Support IPE</prompt>
      <type>boolean</type>
	  <default>true</default>
      <desc>If true (default), expect the presence of the Internet Protocol Extension (IPE) shim, which may be 1 to 8 octets.</desc>
	  <setMethod>setSupportIPE</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.supportIPE</path>
    </setting>
	<setting>
      <prompt>Multi-Packet Zone</prompt>
      <type>boolean</type>
	  <default>true</default>
      <desc>If true, one packet may end in the Packet Zone and one or more additional packets may be inserted afterward (assuming there is room) in the same unit. If false, new packets will only begin only at the first index in the Packet Zone.</desc>
	  <setMethod>setMultiPacketZone</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.multiPacketZone</path>
    </setting>
	<setting>
      <prompt>Allow Packets After Fill</prompt>
      <type>boolean</type>
	  <default>false</default>
      <desc>If false, stop searching for new packets in the Packet Zone when a fill value of 0xe0 is encountered. If true, continue searching for new packets until a non-empty one is found or the end of the Packet Zone is reached.</desc>
	  <setMethod>setAllowPacketsAfterFill</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.allowPacketsAfterFill</path>
    </setting>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
  </config>
</modinfo>
