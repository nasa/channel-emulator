<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modTM_MC_Mux</name>
  <title>Master Channel Multiplexing Function</title>
  <cat>TM</cat>
  <depends>
  </depends>
  <desc>This module has multiple primary input links from which it accepts TM Transfer Frames with different Master Channel Identifiers (MCIDs). The frames are prioritized in the queue according to their MCID. By default, all have the same priority. If those are modified, frames with the highest-priority MCID are sent until there are none remaining; then, frames with the next highest priority are sent until there are none remaining, and so on. If high priority traffic does not abate, low priority traffic will never be sent.[+br/+][+br/+]This service is optionally periodic – it can be configured to send frames at a constant rate and will send Idle Frames when there is no incoming data. The VCID and SCID settings in the configuration file are used to specify which GVCID value the Idle Frames will have (and that is the only use of the SCID/VCID settings in this particular module).</desc>
  <config>
    <setting>
      <prompt>Default Priority</prompt>
      <type>int</type>
	  <min>0</min>
      <desc>Incoming TM Transfer Frames are inserted into the message queue with the specified integer priority if they don’t have their own priority specified.</desc>
	  <setMethod>setDefaultPriority</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.defaultPriority</path>
    </setting>
    <setting>
      <prompt>Channel Priorities</prompt>
	  <type>structList</type>
	  <desc>Insert an incoming TM Transfer Frame with the specified Spacecraft Identifier into the message queue with the specified priority.</desc>
	  <setMethod>setPriority</setMethod>
	  <structList>
        <setting>
          <prompt>SCID</prompt>
          <type>int</type>
          <min>0</min>
          <max>1023</max>
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
		<id>TM_Common_PC_Settings</id>
    </ref>
    <ref>
		<id>TM_Common_MC_Settings</id>
    </ref>
    <ref>
		<id>TM_Common_VC_Settings</id>
    </ref>
    <ref>
		<id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
  </config>
</modinfo>