<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <config>
    <setting>
      <prompt>Frame Length</prompt>
      <type>int</type>
	  <min>0</min>
      <desc>The exact size for all TM transfer frames in the channel in whole octets.</desc>
	  <setMethod>setFrameSize</setMethod>
      <required/>
      <path>Channels.XYZ_CHANNEL_XYZ.TM_PhysicalChannel.frameSize</path>
    </setting>
    <setting>
      <prompt>Frame Error Control</prompt>
      <type>boolean</type>
      <desc>If true, enable frame error control via checksum.</desc>
	  <setMethod>setUseFrameEC</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.TM_PhysicalChannel.useFrameErrorControl</path>
    </setting>
    <setting>
      <prompt>Drop Bad Frames</prompt>
      <default>true</default>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.dropBadFrames</path>
      <type>boolean</type>
      <desc>If true, stop processing a frame if it is found to be invalid.</desc>
	  <setMethod>setDropBadFrames</setMethod>
    </setting>
    <setting>
      <prompt>Idle Pattern</prompt>
      <type>int</type>
	  <array/>
	  <min>0</min>
	  <max>255</max>
      <desc>For services that are periodic, an array of integers h1, h2, h3, ... is used as a pattern to fill the data field in idle units.</desc>
	  <setMethod>setIdlePattern</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.TM_PhysicalChannel.idlePattern</path>
    </setting>
  </config>
</modinfo>
