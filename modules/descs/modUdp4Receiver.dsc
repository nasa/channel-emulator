<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modUdp4Receiver</name>
  <title>IPv4 UDP Receiver</title>
  <cat>IPv4</cat>
  <depends>
	  <modName>devUdp4</modName>
  </depends>
  <desc>This module receives raw data from a UDP device; the packet headers are not retained. It then wraps it as untyped network data and sends it via its primary output link.</desc>
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
