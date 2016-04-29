<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modUDP_Add</name>
  <title>IPv4 UDP Datagram Encoding</title>
  <cat>IPv4</cat>
  <depends>
  </depends>
  <desc>This module accepts any type of wrapped data on its primary input, adds an IPv4 UDP Datagram header to it, and then sends it via its primary output link. It must be manually configured with the source/destination IP addresses and ports. The source address and port may well be fictitious. However, some care should still be used in selecting them if there is a real receiver so a firewall or other safeguards on the remote system do not reject them. The UDP checksum can be added to each packet, if desired.</desc>
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
      <prompt>Source IPv4 Address</prompt>
      <type>ipv4</type>
      <desc>Set the source IPv4 address on each outgoing datagram to a.b.c.d, where each octet a-d is in decimal notation.</desc>
	  <setMethod>setSrcAddr</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.srcAddr</path>
    </setting>
    <setting>
      <prompt>Source Port</prompt>
      <type>int</type>
	  <min>0</min>
	  <max>65535</max>
      <desc>Set the source port on each outgoing datagram to the specified integer.</desc>
	  <setMethod>setSrcPort</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.srcPort</path>
    </setting>
	<setting>
      <prompt>Destination IPv4 Address</prompt>
      <type>ipv4</type>
      <desc>Set the destination IPv4 address on each outgoing datagram to a.b.c.d, where each octet a-d is in decimal notation.</desc>
	  <setMethod>setDstAddr</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.dstAddr</path>
    </setting>
    <setting>
      <prompt>Destination Port</prompt>
      <type>int</type>
	  <min>0</min>
	  <max>65535</max>
      <desc>Set the destination port on each outgoing datagram to the specified integer.</desc>
	  <setMethod>setDstPort</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.dstPort</path>
    </setting>
	<setting>
      <prompt>Enable UDP Checksum</prompt>
      <type>boolean</type>
	  <desc>Select whether the UDP checksum will be calculated and inserted into each datagram.</desc>
      <default>true</default>
      <setMethod>setUDPCRC</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.useUDPCRC</path>
    </setting>
  </config>
</modinfo>
