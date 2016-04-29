<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_MC_Frame</name>
  <title>Master Channel Frame Service</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>This service accepts a generic network data wrapper whose buffer must contain a correctly formed AOS Transfer Frame. The segment re-wraps it as an AOS Transfer Frame. It’s sent on via the primary output link to the next segment, typically either the Master Channel Multiplexing function or All Frames Generation function.[+br/+][+br/+]Incoming transfer frames may have different GVCIDs as long as their MCIDs are identical. Frames with a mismatched MCID (i.e. that does not match the one the segment is configured to accept) will be dropped.[+br/+][+br/+]The service can be configured as either asynchronous or periodic. If periodic, an AOS Transfer Frame will always be sent after a constant time interval. If no data is available at transmission time, an Idle Frame will be sent using the Spacecraft Identifier specified for the segment and Virtual Channel Identifier 63 (3Fh).</desc>
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
