<?xml version="1.0" encoding="UTF-8" ?>
<modinfo>
  <config>
    <setting>
      <prompt>Frame Length</prompt>
      <path>Channels.XYZ_CHANNEL_XYZ.AOS_PhysicalChannel.frameSize</path>
      <type>int</type>
	  <min>0</min>
      <desc>The exact size for all AOS transfer frames in the channel in whole octets.</desc>
	  <setMethod>setFrameSize</setMethod>
      <required/>
    </setting>
    <setting>
      <prompt>Header Error Control</prompt>
      <path>Channels.XYZ_CHANNEL_XYZ.AOS_PhysicalChannel.useHeaderErrorControl</path>
      <type>boolean</type>
      <desc>If true, enable frame header error control using a Reed-Solomon (10,6) code.</desc>
	  <setMethod>setUseHeaderEC</setMethod>
    </setting>
    <setting>
      <prompt>Frame Error Control</prompt>
      <path>Channels.XYZ_CHANNEL_XYZ.AOS_PhysicalChannel.useFrameErrorControl</path>
      <type>boolean</type>
      <desc>If true, enable error control for the entire frame via a checksum.</desc>
	  <setMethod>setUseFrameEC</setMethod>
    </setting>
    <setting>
      <prompt>Drop Bad Frames</prompt>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.dropBadFrames</path>
      <type>boolean</type>
      <default>true</default>
      <desc>If true, stop processing a frame if it is found to be invalid.</desc>
	  <setMethod>setDropBadFrames</setMethod>
    </setting>
    <setting>
      <prompt>Insert Zone Size</prompt>
      <path>Channels.XYZ_CHANNEL_XYZ.AOS_PhysicalChannel.insertZoneSize</path>
      <type>int</type>
	  <min>0</min>
	  <default>0</default>
      <desc>Reserve the specified number of octets in each transfer frame for the Insert Zone. If the Insert Zone is not in use, this should always be set to zero.</desc>
	  <setMethod>setInsertZoneSize</setMethod>
    </setting>
    <setting>
      <prompt>Idle Pattern</prompt>
      <path>Channels.XYZ_CHANNEL_XYZ.AOS_PhysicalChannel.idlePattern</path>
      <type>int</type>
	  <array/>
	  <min>0</min>
	  <max>255</max>
      <desc>For services that are periodic, an array of integers h1, h2, h3, ... is used as a pattern to fill the data field in idle units.</desc>
	  <setMethod>setIdlePattern</setMethod>
    </setting>
  </config>
</modinfo>
