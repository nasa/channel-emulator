<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEncapPkt_Add</name>
  <title>Encapsulation Packet Encoding</title>
  <cat>CCSDS</cat>
  <depends>
  </depends>
  <desc>This service accepts a wrapper type limited to those specified in the CCSDS Space Link Identifiers recommended standard (currently only IPv4 datagram wrappers are auto-detected). It incorporates its buffer into an Encapsulation Packet wrapper, by default including an Internet Protocol Extension header (IPE), and sends that out via the primary output link. Usually the target is a Multiplexing PDU encoding segment.</desc>
  <config>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
    <setting>
      <prompt>Use Internet Protocol Extension (IPE)</prompt>
      <type>boolean</type>
      <desc>If true, use the Internet Protocol Extension (IPE) shim to identify the contents of the data field. If false, the protocol ID field or mission-customized methods are used.</desc>
      <default>true</default>
	  <setMethod>setSupportIPE</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.supportIPE</path>
    </setting>
    <setting>
      <prompt>Internet Protocol Extension (IPE) Value</prompt>
      <type>int64</type>
      <desc>Override IPE auto-detection and set the IPE of each packet to the specified 64-bit integer value.</desc>
	  <setMethod>setIPE</setMethod>
	  <unsetMethod>unsetIPE</unsetMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.IPE</path>
    </setting>
    <setting>
	  <prompt>Protocol ID</prompt>
      <type>int</type>
      <desc>Set the value of the Protocol ID field to an integer between 0 and 7.</desc>
	  <min>0</min>
	  <max>7</max>
	  <setMethod>setProtocol</setMethod>
	  <unsetMethod>unsetProtocol</unsetMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.protocol</path>
    </setting>
    <setting>
	  <prompt>Length of Length</prompt>
      <type>int</type>
      <desc>Set the value of the Length of Length field to an integer between 0 and 3. Defaults to 2.</desc>
	  <min>0</min>
	  <max>3</max>
	  <default>2</default>
	  <setMethod>setLengthOfLength</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.lengthOfLength</path>
    </setting>
	<setting>
	  <prompt>User Defined Field</prompt>
      <type>int</type>
      <desc>Set the value of the User Defined Field to an integer between 0 and 15. Defaults to 0.</desc>
	  <min>0</min>
	  <max>15</max>
	  <default>0</default>
	  <setMethod>setUserDefinedField</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.userDefinedField</path>
    </setting>
	<setting>
	  <prompt>Protocol ID Extension</prompt>
      <type>int</type>
      <desc>Set the value of the Protocol ID Extension field to an integer between 0 and 15. Defaults to 0.</desc>
	  <min>0</min>
	  <max>15</max>
	  <default>0</default>
	  <setMethod>setProtocolIDExtension</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.protocolIDExtension</path>
    </setting>
	<setting>
	  <prompt>CCSDS Defined Field</prompt>
      <type>int</type>
      <desc>Set the value of the CCSDS Defined Field to an integer between 0 and 65,535. Note: This field is supposed to be reserved for future use and should be set to “all zeroes”, which is the default.</desc>
	  <min>0</min>
	  <max>65535</max>
	  <default>0</default>
	  <setMethod>setCCSDSDefinedField</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.CCSDSDefinedField</path>
    </setting>
  </config>
</modinfo>
