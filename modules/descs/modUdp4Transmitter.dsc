<?xml version="1.0" encoding="UTF-8" ?>
<modinfo>
  <name>modUdp4Transmitter</name>
  <title>IPv4 UDP Transmitter</title>
  <cat>IPv4</cat>
  <depends>
    <modname>devUdp4</modname>
  </depends>
  <desc>This module can receive any type of wrapper and write its buffer to a socket maintained by an IPv4 UDP device, after which it will be encoded as the payload of an IPv4 UDP datagram.</desc>
  <config>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.primaryOutput</ignore>
    </ref>
    <setting>
      <prompt>Destination IPv4 Address</prompt>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.destAddress</path>
      <type>ipv4</type>
      <desc>Set the IPv4 address to send datagrams to. No checking is performed as to whether any system is actually receiving at that address.</desc>
	  <setMethod>setDstAddr</setMethod>
    </setting>
    <setting>
      <prompt>Destination IPv4 Port</prompt>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.destPort</path>
      <type>int</type>
      <desc>Set the UDP port to send datagrams to.</desc>
	  <setMethod>setDstPort</setMethod>
	  <min>0</min>
	  <max>65535</max>
    </setting>
  </config>
</modinfo>
