<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modAOS_VC_Mux</name>
  <title>Virtual Channel Multiplexing Function</title>
  <cat>AOS</cat>
  <depends>
  </depends>
  <desc>This module has multiple primary input links from which it accepts AOS Transfer Frames with different Global Virtual Channel Identifiers (GVCIDs). The frames are prioritized in the queue according to their GVCID. By default, all have the same priority. If those are modified, frames with the highest-priority GVCID are sent until there are none remaining; then, frames with the next highest priority are sent until there are none remaining, and so on. If high priority traffic does not abate, low priority traffic will never be sent.[+br/+][+br/+]This service is optionally periodic – it can send frames at a constant rate and send Idle Frames with VCID 63 (3Fh) when no incoming data is available.</desc>
  <config>
    <ref>
		<id>AOS_Common_PC_Settings</id>
    </ref>
    <ref>
		<id>AOS_Common_MC_Settings</id>
    </ref>
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
	  <desc>Insert an incoming AOS Transfer Frame with the specified Virtual Channel Identifier into the message queue with the specified priority.</desc>
	  <setMethod>setPriority</setMethod>
	  <type>structList</type>
	  <structList>
        <setting>
          <prompt>VCID</prompt>
          <type>int</type>
          <min>0</min>
          <max>63</max>
          <desc>The Virtual Channel Identifier to apply the priority to.</desc>
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
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
  </config>
</modinfo>
