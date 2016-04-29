<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_VC_Gen</name>
  <title>Virtual Channel Generation Function</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>This module is responsible for encoding data into AOS Transfer Frames. It accepts units on its primary input with a specific buffer length, but otherwise the type is ignored (it’s the responsibility of the operator to ensure that the correct type of data is being fed). Then, the new AOS Transfer Frame is generated, copying the buffers of the units it has just received, and is sent on via the segment’s primary output link.</desc>
  <config>
    <ref>
		<id>AOS_Common_PC_Settings</id>
    </ref>
    <ref>
		<id>AOS_Common_MC_Settings</id>
    </ref>
    <ref>
		<id>AOS_Common_VC_Settings</id>
    </ref>
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
