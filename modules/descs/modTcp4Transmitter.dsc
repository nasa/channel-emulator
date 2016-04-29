<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTcp4Transmitter</name>
  <title>IPv4 TCP Transmitter</title>
  <cat>IPv4</cat>
  <depends>
	  <modName>devTcp4Server</modName>
  </depends>
  <desc>This module can receive any type of wrapper and write its buffer to a socket maintained by an IPv4 TCP Client or Server device, after which it will be encoded as the payload of an IPv4 TCP packet. Data will only be transmitted its associated TCP device has an active connection; otherwise, data will remain in the message queue. If it fills up, it will cause the segment on the primary input link to block.</desc>
  <config>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutput</ignore>
    </ref>
  </config>
</modinfo>
