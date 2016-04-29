<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEthFrame_Remove</name>
  <title>Ethernet Frame Decoding</title>
  <cat>Ethernet</cat>
  <depends>
  </depends>
  <desc>This module accepts IEEE 802.3 Ethernet frames on its primary input. It removes the frame header and wraps the payload in the data field. If the payload if of a recognized and supported type, it is wrapped with an object with specific support for that type; otherwise, it is treated as untyped. The wrapper is then sent via the primary output link to the target.</desc>
  <config>
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
