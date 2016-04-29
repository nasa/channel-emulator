<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEthFrame_Add</name>
  <title>Ethernet Frame Encoding</title>
  <cat>Ethernet</cat>
  <depends>
  </depends>
  <desc>This module can take any type of data as input, encapsulate it in an IEEE 802.3 Ethernet frame, and send it via its primary output to the next segment. Source and destination MAC addresses must be manually configured.</desc>
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
      <prompt>Destination MAC Address</prompt>
      <type>mac</type>
      <desc>The destination MAC address in the form "a:b:c:d:e:f", where each octet a-f is in hexadecimal format.</desc>
      <setMethod>setDstMAC</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.dstMAC</path>
    </setting>
    <setting>
	  <prompt>Source MAC Address</prompt>
      <type>mac</type>
      <desc>The source MAC address in the form "a:b:c:d:e:f", where each octet a-f is in hexadecimal format.</desc>
      <setMethod>setSrcMAC</setMethod>
	  <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.srcMAC</path>
	</setting>
	<setting>
      <prompt>Default Payload Type</prompt>
      <type>int</type>
      <desc>Set the default payload type to the specified integer. This value is put into the Type/Length Field in the Ethernet header if the incoming wrapper type is unrecognized. Without this setting, the payload length is used in that field instead of the type.</desc>
      <setMethod>setDefaultPayloadType</setMethod>
	  <min>0</min>
	  <max>65535</max>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.defaultPayloadType</path>
    </setting>
  </config>
</modinfo>
