<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modUDP_Remove</name>
  <title>IPv4 UDP Datagram Decoding</title>
  <cat>IPv4</cat>
  <depends>
  </depends>
  <desc>This module accepts IPv4 UDP datagram wrappers on its primary input. If CRC checking is enabled, both the IP and UDP checksums are tested; if there is a mismatch, the datagram is dropped. If the checksums are valid, the payload is extracted, wrapped as untyped data, and sent on via the primary output link.</desc>
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
      <prompt>Test UDP Checksum</prompt>
      <type>boolean</type>
	  <desc>Select whether CRC testing will be performed on incoming datagrams.</desc>
      <default>true</default>
      <setMethod>setUDPCRC</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.useUDPCRC</path>
    </setting>
  </config>
</modinfo>
