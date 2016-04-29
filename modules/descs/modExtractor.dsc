<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modExtractor</name>
  <title>Extractor</title>
  <cat>Utility</cat>
  <depends>
  </depends>
  <desc>This module accepts wrapped data of any type on its primary input. A specified number of octets are removed from the header and the trailer of each received unit, and it is then sent via the output primary link to the target segment. If the original unit is too short it is sent unchanged.</desc>
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
      <prompt>Header Length</prompt>
      <type>int</type>
      <default>0</default>
	  <min>0</min>
      <desc>Remove the specified number of octets from the start of each received unit.</desc>
      <setMethod>setHeaderLength</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.headerLength</path>
    </setting>
    <setting>
      <prompt>Trailer Length</prompt>
      <type>int</type>
      <desc>Remove the specified number octets from the end of each received unit.</desc>
      <default>0</default>
	  <min>0</min>
      <setMethod>setTrailerLength</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.trailerLength</path>
    </setting>
  </config>
</modinfo>
