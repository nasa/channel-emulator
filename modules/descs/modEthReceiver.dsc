<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEthReceiver</name>
  <title>Ethernet Frame Reception</title>
  <cat>Ethernet</cat>
  <depends>
	  <modName>devEthernet</modName>
  </depends>
  <desc>The Ethernet Frame Reception function reads from a packet socket managed by a CE Ethernet Device. When an IEEE 802.3 Ethernet frame buffer is received, it is wrapped, time stamped, and then sent to the target via the segment’s primary output link. Little checking on the validity of the structure in the buffer is performed; it is assumed that it has already been validated at lower levels in the operating system. The target is typically a Ethernet Frame Decoding segment.</desc>
  <config>
	<ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.lowWaterMark</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.highWaterMark</ignore>
	</ref>
  </config>
</modinfo>
