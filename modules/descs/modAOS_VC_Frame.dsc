<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_VC_Frame</name>
  <title>Virtual Channel Frame Service</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>This service accepts a generic network data wrapper whose buffer must contain a correctly formed AOS Transfer Frame. The segment re-wraps it with an AOS Transfer Frame wrapper. It’s sent on via the primary output link to the next segment, typically the Virtual Channel Multiplexing function.[+br/+][+br/+]Incoming transfer frames must have identical GVCIDs. Frames with a mismatched GVCID (i.e. that does not match the one the segment is configured to accept) will be dropped.[+br/+][+br/]The service can be configured as either asynchronous or periodic. If periodic, an AOS Transfer Frame will always be sent after a constant time interval. If no data is available at transmission time, an Idle Frame will be generated. Note that the Idle Frame will have a VCID of 63 (3Fh) rather than the same VCID as incoming frames.</desc>
  <config>
    <ref>
		<id>AOS_Common_PC_Settings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.AOS_PhysicalChannel.idlePattern</ignore>
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
