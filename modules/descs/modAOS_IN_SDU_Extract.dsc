<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_IN_SDU_Extract</name>
  <title>Insert Zone Data Unit Extraction Service</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>The Insert Zone Data Unit Extraction Service accepts AOS Transfer Frame wrappers on its primary input queue. If the Insert Zone is in use, its IN_SDU is wrapped separately and sent via the auxiliary output. The complete, unmodified frame is then send via the primary output.</desc>
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
