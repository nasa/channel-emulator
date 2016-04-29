<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_M_PDU_Add</name>
  <title>Packet Extraction Function</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>This service accepts variable-length Encapsulation Packet wrappers. A single large Encapsulation Packet may either be split across multiple M_PDUs, or, several small Encapsulation Packets may be inserted into a single M_PDU Packet Zone. If there is empty space remaining in the Packet Zone and another Packet is waiting in the queue, the front of that Packet will be added to the current M_PDU. The rest will be put into the next M_PDU. Finally, M_PDU wrappers are sent via the primary output to the next segment, typically the primary input of Virtual Channel Generation segment.[+br/+][+br/+]The size of the M_PDU (and from it, the size of the Packet Zone) is determined automatically by querying the MRU of the primary output’s target segment if it already exists. Otherwise, the MTU setting is used for the M_PDU length. If neither is set, the incoming data must be dropped.</desc>
  <config>
    <setting>
      <prompt>Incoming Packet Timeout (&#x3bc;s)</prompt>
      <type>int</type>
	  <min>0</min>
      <desc>Set the number of microseconds to wait for a new incoming packet before sending the current one partially empty (i.e. the remainder of the Packet Zone containing only fill packets instead of data).</desc>
	  <setMethod>setWaitForNextPacket</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.maxUsecsForNextPacket</path>
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
      <prompt>Fill Pattern</prompt>
      <type>int</type>
	  <min>0</min>
	  <max>255</max>
	  <array/>
      <desc>Sets the Packet Zone fill pattern to the specified array of comma-separated values.</desc>
	  <setMethod>setFillPattern</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.fillPattern</path>
    </setting>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
  </config>
</modinfo>
