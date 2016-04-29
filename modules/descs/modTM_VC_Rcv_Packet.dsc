<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTM_VC_Rcv_Packet</name>
  <title>Virtual Channel Reception – Packet Service</title>
  <cat>TM</cat>
  <depends>
  </depends>
  <desc>This service accepts fixed-length TM Transfer Frames, with Data Fields that contain variable-length Encapsulation Packets or Space Packets. A single frame may carry a partial or whole Packet, or multiples, depending on the relative lengths; there may also be an Idle Packet if no incoming traffic was available to fill the remainder of the Data Field.[+br/+][+br/+]Packets are extracted from the Data Field buffers and reassembled in their own buffer, then wrapped and sent via the primary output to the next segment, which typically removes the Packet header or processes it in some other fashion.</desc>
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
      <prompt>Support IPE</prompt>
      <type>boolean</type>
	  <default>true</default>
      <desc>If true (default), expect the presence of the Internet Protocol Extension (IPE) shim for Encapsulation Packets, which may be 1 to 8 octets.</desc>
	  <setMethod>setSupportIPE</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.supportIPE</path>
    </setting>
	<setting>
      <prompt>Multi-Packet Data Field</prompt>
      <type>boolean</type>
	  <default>true</default>
      <desc>If true, expect that one Packet may end in the Data Field and one or more additional packets may be found afterward in the same unit. If false, new packets will only begin only at the first index in the Data Field.</desc>
	  <setMethod>setMultiDataField</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.multiDataField</path>
    </setting>
    <setting>
      <prompt>Primary Outputs</prompt>
	  <desc>Packets are directed by Packet Version Number (PVN) to either the Primary or Auxiliary input of the specified segment. At this time only Encapsulation Packets are implemented. The segment must already exist in the channel.</desc>
	  <setMethod>connectPrimaryOutput</setMethod>
	  <unsetMethod>DO_NOT_UNSET</unsetMethod>
	  <type>structList</type>
      <structList>
        <setting>
          <prompt>Target Segment</prompt>
          <type>output</type>
          <desc>The existing segment that will receive the output.</desc>
          <order>1</order>
        </setting>
        <setting>
          <prompt>Target Input Rank</prompt>
          <type>string</type>
          <acceptedValues>
            <item value="PrimaryInput">Primary</item>
            <item value="AuxiliaryInput">Auxiliary</item>
          </acceptedValues>
          <desc>Primary or Auxiliary.</desc>
          <order>2</order>
        </setting>
        <setting>
          <prompt>PVN</prompt>
          <type>int</type>
          <desc>The Packet Version Number to route.</desc>
          <acceptedValues>
			<item value="0">Space Packet (0)</item>
			<item value="1">SCPS NP(1)</item>
			<item value="7">Encapsulation Packet(7)</item>
          </acceptedValues>
          <order>3</order>
        </setting>
      </structList>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutputs</path>
    </setting>
	<setting>
      <prompt>Allow Packets After Fill</prompt>
      <type>boolean</type>
	  <default>false</default>
	  <desc>If false, stop searching for new packets in the Data Field when a fill value of 0xe0 is encountered. If true, continue searching for new packets until a non-empty one is found or the end of the Data Field is reached. [+i+]Note: This setting will have no effect if an Idle Packet was used to pad the Data Field, as modTM_VC_Gen_Packet does[+/i+].</desc>
	  <setMethod>setAllowPacketsAfterFill</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.allowPacketsAfterFill</path>
    </setting>
	<ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutput</ignore>
    </ref>
  </config>
</modinfo>