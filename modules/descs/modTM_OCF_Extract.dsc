<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTM_OCF_Extract</name>
  <title>Operational Control Field Extraction Service</title>
  <cat>TM</cat>
  <depends>
  </depends>
  <desc>The OCF Extraction Service accepts TM Transfer Frame wrappers on its primary input queue. If the Operational Control Field Flag of a frame is true, its OCF is wrapped separately and sent via the auxiliary output. The complete, unmodified frame is then send via the primary output.</desc>
  <config>
    <ref>
		<id>TM_Common_PC_Settings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.TM_PhysicalChannel.idlePattern</ignore>
    </ref>
    <ref>
		<id>TM_Common_MC_Settings</id>
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
