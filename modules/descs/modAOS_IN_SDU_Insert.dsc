<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_IN_SDU_Insert</name>
  <title>Insert Zone Insertion Service</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>The IN_SDU Insertion Service accepts AOS Transfer Frame wrappers on its primary input queue. On its auxiliary input queue it receives fixed-length data units to insert into the Insert Zone of the frames it receives. As frames arrive, the buffers of any units waiting in the auxiliary input queue are copied into the appropriate location of each frame, and then the altered frames are sent via the primary output queue. This segment should be placed immediately before an AOS All-Frames Generation service so that error correcting symbols can be calculated.</desc>
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
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
  </config>
</modinfo>
