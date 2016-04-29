<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modSplitter</name>
  <title>Splitter</title>
  <cat>Utility</cat>
  <depends>
  </depends>
  <desc>This module accepts wrapped data of any type on its primary input. A shallow copy of each unit is created; the original is sent on via the primary output. The copy is sent out via the auxiliary output. The copy retains only a generic wrapper of the buffer, so receiving segments expecting a specific wrapper type (for example, an AOS Transfer Frame) will report an error. One intended use of this module is logging.</desc>
  <config>
	<ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
	</ref>
  </config>
</modinfo>
