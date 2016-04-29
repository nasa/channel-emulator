<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTM_OCF_Insert</name>
  <title>Operational Control Field Insertion Service</title>
  <cat>TM</cat>
  <depends>
  </depends>
  <desc>The OCF Insertion Service accepts TM Transfer Frame wrappers on its primary input queue. On its auxiliary input queue it receives four-octet data units to insert into the Operational Control Field of the frames it receives. As frames arrive, the buffers of any units waiting in the auxiliary input queue are copied into the appropriate location of each frame, and then the altered frames are sent via the primary output queue. By placing this segment immediately after a Virtual Channel Generation function, it will provide a Virtual Channel Operational Control Field Service; by placing it after a Master Channel Generation function, it will provide a Master Channel Operational Control Field Service.</desc>
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
