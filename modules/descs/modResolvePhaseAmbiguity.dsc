<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modResolvePhaseAmbiguity</name>
  <title>Phase Ambiguity Resolution</title>
  <cat>Emulation</cat>
  <depends></depends>
  <desc>This module accepts wrapped data of any type on its primary input. It left shifts and/or inverts the bits of every octet in the data block. The value of the shift and inversion can either be set statically or dynamically. During dynamic detection, the segment searches for shifted/inverted ASMs; the contents of the ASM can be configured to any set of octets. If there is a carry left over, it will put it at the beginning of the next incoming block; if there is no block waiting in the queue it will drop the carry.</desc>
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
	  <prompt>Left Shift Bits</prompt>
      <type>int</type>
	  <min>0</min>
	  <max>7</max>
	  <desc>Represents the left shift that is expected in incoming data, so right shift all data in each block by the specified integer in the range 0-7.</desc>
      <setMethod>setLeftShiftBits</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.leftShiftBits</path>
    </setting>
	<setting>
      <prompt>Invert</prompt>
      <type>boolean</type>
	  <desc>Represents the expected inversion in incoming data, so if true, flip all the bits in the block.</desc>
      <default>false</default>
      <setMethod>setInversion</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.invert</path>
    </setting>
	<setting>
      <prompt>Detect Shift</prompt>
      <type>boolean</type>
	  <desc>If true, ignore the values of "Left Shift Bits" and "Invert", and look for modified ASMs to determine the values automatically.</desc>
      <default>false</default>
      <setMethod>setDetectShift</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.detectShift</path>
    </setting>
	<setting>
      <prompt>Allowed Marker Bit Errors</prompt>
      <type>int</type>
	  <desc>The number of bit errors that can be encountered in an ASM to still be considered valid.</desc>
      <default>0</default>
	  <min>0</min>
      <setMethod>setAllowedMarkerBitErrors</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.allowedMarkerBitErrors</path>
    </setting>
	<setting>
	  <prompt>Wait Microseconds</prompt>
      <type>int</type>
	  <min>0</min>
	  <desc>After processing a block, wait up to the specified microseconds for another block to appear in the queue. If nothing arrives, append the carry to the current block and send it.</desc>
      <setMethod>setLeftShiftBits</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.maxUsecsForNewData</path>
    </setting>
  </config>
</modinfo>
