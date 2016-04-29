<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEmulatePhaseAmbiguity</name>
  <title>Phase Ambiguity Emulation</title>
  <cat>Emulation</cat>
  <depends></depends>
  <desc>This module accepts wrapped data of any type on its primary input. It left shifts and/or inverts the bits of every octet in the data block. If there is a carry left over, it will put it at the beginning of the next incoming block; if there is no block waiting in the queue it will append the carry to the current block.</desc>
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
	  <desc>Left shift all data in each block by the specified integer in the range 0-7.</desc>
      <setMethod>setLeftShiftBits</setMethod>
	  <min>0</min>
	  <max>7</max>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.leftShiftBits</path>
    </setting>
	<setting>
      <prompt>Invert</prompt>
      <type>boolean</type>
	  <desc>Whether to flip all the bits in the block or not.</desc>
      <default>false</default>
      <setMethod>setInversion</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.invert</path>
    </setting>
	<setting>
	  <prompt>Wait Microseconds</prompt>
      <type>int</type>
	  <desc>After processing a block, wait up to the specified microseconds for another block to appear in the queue. If nothing arrives, append the carry to the current block and send it.</desc>
      <setMethod>setLeftShiftBits</setMethod>
	  <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.maxUsecsForNewData</path>
    </setting>
  </config>
</modinfo>
