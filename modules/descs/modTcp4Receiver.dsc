<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTcp4Receiver</name>
  <title>IPv4 TCP Receiver</title>
  <cat>IPv4</cat>
  <depends>
	  <modName>devTcp4Server</modName>
  </depends>
  <desc>This module receives raw data from either a TCP Server or TCP Client device; the packet headers are not retained. It then wraps it as untyped network data and sends it via its primary output link. If the associated device is a Server, it will also handle accepting new connections. Only one connection is allowed at a time (although more clients may be waiting to have their connection accepted). This is because for two-way channels, there is not necessarily any address information that can be used to properly de-multiplex outgoing data to multiple clients.</desc>
  <config>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.lowWaterMark</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.highWaterMark</ignore>
    </ref>
	<setting>
      <prompt>Wait For Full MRU</prompt>
      <type>boolean</type>
	  <desc>If true, the segment will wait to receive its full MRU before continuing; if false, it will continue once there is nothing else to read.</desc>
      <default>false</default>
      <setMethod>setReceiveMax</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.receiveMax</path>
    </setting>
  </config>
</modinfo>
