<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_AF_Rcv</name>
  <title>All Frames Reception Function</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>The All Frames Reception function accepts AOS Transfer Frame wrappers into its primary input queue. It performs error control decoding if enabled: if the code in Frame Error Control Field is invalid, the frame is dropped. If header error correction is enabled, the Reed-Solomon (10,6) algorithm performs correction on the header. If it is uncorrectable, the frame is dropped and the bad header counter incremented.[+br/+][+br/+]If the frame is valid, it is passed on to the segment configured as the modAOS_AF_Rcv segment’s primary outp.</desc>
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
