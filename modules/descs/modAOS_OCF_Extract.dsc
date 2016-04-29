<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_OCF_Extract</name>
  <title>Operational Control Field Extraction Service</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>The OCF Extraction Service accepts AOS Transfer Frame wrappers on its primary input queue. If the Operational Control Field is in use, its OCF is wrapped separately and sent via the auxiliary output. The complete, unmodified frame is then send via the primary output.</desc>
  <config>
    <ref>
		<id>AOS_Common_PC_Settings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.AOS_PhysicalChannel.idlePattern</ignore>
    </ref>
	<ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
  </config>
</modinfo>
