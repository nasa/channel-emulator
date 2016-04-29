<?xml version="1.0" encoding="UTF-8" ?>
<modinfo>
  <name>modTM_MC_Rcv</name>
  <title>Master Channel Reception Function</title>
  <cat>TM</cat>
  <depends>
  </depends>
  <desc>The Master Channel Generation Service accepts TM Transfer Frame wrappers on its primary input queue. The length and MCID of the frame are tested, and if there is a mismatch, it is dropped. The Master Channel Frame Count of the incoming frame is compared to a counter, and a warning is generated if they are not identical. Idle Data is discarded.</desc>
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
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
    </ref>
  </config>
</modinfo>