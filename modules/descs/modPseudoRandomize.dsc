<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modPseudoRandomize</name>
  <title>Pseudo Randomization</title>
  <cat>CCSDS</cat>
  <depends>
  </depends>
  <desc>This module accepts wrapped data of any type on its primary input. Each unit is exclusive-ORed with a the bit sequence specified in the CCSDS Recommended Standard for TM Synchronization and Channel Coding (Blue Book), Section 9. If a unit had previously been pseudo-randomized, the original message is produced.</desc>
  <config>
	<ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
	</ref>
  </config>
</modinfo>
