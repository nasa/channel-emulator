<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEncapPkt_Remove</name>
  <title>Encapsulation Packet Decoding</title>
  <cat>CCSDS</cat>
  <depends>
  </depends>
  <desc>This service accepts an Encapsulation Packet wrapper, extracts the data field buffer, wraps it as the correct type (currently only IPv4 datagram wrappers are auto-detected), and sends it on via the primary output link. The Internet Protocol Extension header (IPE) is used by default to determine the type of the buffer payload.</desc>
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
  </config>
</modinfo>
