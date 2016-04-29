<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTM_MC_Gen</name>
  <title>Master Channel Generation Function</title>
  <cat>TM</cat>
  <depends>
  </depends>
  <desc>The Master Channel Generation Service accepts TM Transfer Frame wrappers on its primary input queue. The length and MCID of the frame are tested, and if there is a mismatch, it is dropped. Otherwise the Master Channel Frame Count is incremented (or reset to 0 if it is already 255) and the frame is forwarded to the next target.</desc>
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
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
  </config>
</modinfo>
