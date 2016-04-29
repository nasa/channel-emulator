<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTM_VC_Gen_Packet</name>
  <title>Virtual Channel Generation – Packet Service</title>
  <cat>TM</cat>
  <depends>
  </depends>
  <desc>This service accepts variable-length Encapsulation Packet or Space Packet wrappers and inserts them into TM Transfer Frames. A single large Packet may either be split across multiple frames, or, several small Packets may be inserted into a single Data Field. If there is empty space remaining in the Data Field and another Packet is waiting in the queue, the front of that Packet will be added to the Data Field of the current frame. The remainder will be put into the next frame. Finally, TM Transfer Frame wrappers are sent via the primary output to the next segment, for example the primary input of a Virtual Channel Multiplexing segment.</desc>
  <config>
    <ref>
		<id>TM_Common_PC_Settings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.TM_PhysicalChannel.idlePattern</ignore>
    </ref>
    <ref>
		<id>TM_Common_MC_Settings</id>
    </ref>
    <ref>
		<id>TM_Common_VC_Settings</id>
    </ref>
    <setting>
      <prompt>Incoming Packet Timeout (&#x3bc;s)</prompt>
      <type>int</type>
	  <min>0</min>
      <desc>Set the number of microseconds to wait for a new incoming packet before sending the current one partially empty (i.e. the remainder of the Data Field containing only an Idle Packet instead of data)</desc>
	  <setMethod>setWaitForNextPacket</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.maxUsecsForNextPacket</path>
    </setting>
	<setting>
      <prompt>Multi-Packet Data Field</prompt>
      <type>boolean</type>
	  <default>true</default>
      <desc>If true, one packet may end in the Data Field and one or more additional packets may be inserted afterward (assuming there is room) in the same unit. If false, new packets will only begin only at the first index in the Data Field</desc>
	  <setMethod>setMultiDataField</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.multiPacketDataField</path>
    </setting>
    <setting>
      <prompt>Fill Pattern</prompt>
      <type>int</type>
	  <min>0</min>
	  <max>255</max>
	  <array/>
      <desc>Fill Idle Packets (to pad the Data Field) with the pattern in the specified array.</desc>
	  <setMethod>setFillPattern</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.fillPattern</path>
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
