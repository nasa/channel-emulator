<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_MC_Mux</name>
  <title>Master Channel Multiplexing Function</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>This module has multiple primary input links from which it accepts AOS Transfer Frames with different Master Channel Identifiers (MCIDs). The frames are prioritized in the queue according to their MCID. By default, all have the same priority. If those are modified, frames with the highest-priority MCID are sent until there are none remaining; then, frames with the next highest priority are sent until there are none remaining, and so on. If high priority traffic does not abate, low priority traffic will never be sent.[+br/+][+br/+]This service is optionally periodic – it sends frames at a constant rate and will send Idle Frames when there is no incoming data. The SCID setting in the configuration file is used to specify which Master Channel Identifier the Idle Frames will have (and that is the only use of the SCID setting in this particular module). The Virtual Channel Identifier is always 63 (3Fh).</desc>
  <config>
    <setting>
      <prompt>Default Priority</prompt>
      <type>int</type>
	  <min>0</min>
      <desc>Incoming AOS Transfer Frames are inserted into the message queue with the specified integer priority if they don’t have their own priority specified.</desc>
	  <setMethod>setDefaultPriority</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.defaultPriority</path>
    </setting>
    <setting>
      <prompt>Channel Priorities</prompt>
	  <desc>Insert an incoming AOS Transfer Frame with the specified Spacecraft Identifier into the message queue with the specified priority.</desc>
	  <setMethod>setPriority</setMethod>
	  <type>structList</type>
	  <structList>
        <setting>
          <prompt>SCID</prompt>
          <type>int</type>
          <min>0</min>
          <max>255</max>
          <desc>The Spacecraft Identifier to apply the priority to.</desc>
          <order>1</order>
        </setting>
        <setting>
          <prompt>Priority</prompt>
          <type>int</type>
          <desc>The priority to apply.</desc>
          <min>0</min>
          <order>2</order>
        </setting>
      </structList>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.channelID_Priorities</path>
    </setting>
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
