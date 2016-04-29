<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEmulateDelay</name>
  <title>Delay Emulation</title>
  <cat>Emulation</cat>
  <depends>
  </depends>
  <desc>This module accepts wrapped data of any type on its primary input. It holds the data unit for a specified amount of time, and then releases it via it primary output link to the target. The delay time can be variable if desired, resulting in an emulation of jitter, and as a result the order of the data units may optionally be modified from the order they were received in. The delay time may either be calculated from the timestamp on the wrapper or the time that the module began processing the unit. The timestamp on the wrapper is generated when the wrapper is created, and calculating the delay from it may help to smooth out fluctuations in processing time. The expected throughput may also be provided as a setting, and from it the module will calculate the delay-bandwidth product and resize its message queue accordingly. However, if the low or high water marks have been set manually, their values will not be changed.</desc>
  <config>
    <ref>
        <id>CommonSegmentSettings</id>
        <ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
        <ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MRU</ignore>
        <ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
        <ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
        <ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.deviceName</ignore>
    </ref>
    <setting>
      <prompt>Base Delay (seconds)</prompt>
      <type>double</type>
      <default>0.0</default>
      <desc>Hold each unit for a base number of seconds (a double-precision floating-point value).</desc>
      <setMethod>setDelaySeconds</setMethod>
      <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.delaySeconds</path>
    </setting>
    <setting>
      <prompt>Jitter (seconds)</prompt>
      <type>double</type>
      <desc>Add a random value from 0.0 to the specified seconds (as a double-precision floating point value) onto the value of the base delay before sending the data out.</desc>
      <default>0.0</default>
      <setMethod>setJitterSeconds</setMethod>
      <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.jitterSeconds</path>
    </setting>
    <setting>
      <prompt>Timeline</prompt>
      <type>structList</type>
      <desc>Allow different delay/jitter pairs to go into effect at different times. This is a list of time/delay/jitter groupings. Time is a floating-point number representing seconds relative to the start of the emulation; delay and jitter are also floating-point numbers representing seconds. They must appear in the list in chronological order; out-of-order pairs will be rejected.</desc>
      <setMethod>setDelayAndJitter</setMethod>
      <unsetMethod>clearTimeLine</unsetMethod>
      <structList sort="ascending">
        <setting>
          <prompt>Start Time</prompt>
          <type>double</type>
          <desc>The time in seconds that this will take effect.</desc>
          <min>0.0</min>
          <key>1</key>
          <order>1</order>
        </setting>
        <setting>
          <prompt>Base Delay (seconds)</prompt>
          <type>double</type>
          <desc dup="true">Hold each unit for a base number of seconds (a double-precision floating-point value).</desc>
          <min>0.0</min>
          <order>2</order>
        </setting>
        <setting>
          <prompt>Jitter (seconds)</prompt>
          <type>double</type>
          <desc dup="true">Add a random value from 0.0 to the specified seconds (as a double-precision floating point value) onto the value of the base delay before sending the data out.</desc>
          <min>0.0</min>
          <order>3</order>
        </setting>
      </structList>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.timeLine</path>
    </setting>
    <setting>
      <prompt>Allow Jitter Reorder</prompt>
      <type>boolean</type>
      <desc>If true, variations in timing due to a jitter value greater than 0.0s can result in the reordering of data units.</desc>
      <default>false</default>
      <setMethod>setAllowJitterReorder</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.allowJitterRorder</path>
    </setting>
    <setting>
      <prompt>Expected kbit/s Throughput</prompt>
      <type>int</type>
      <desc>Used to automatically calculate the delay-bandwidth product and set the length of the message queue to accommodate it.</desc>
      <default>0</default>
      <setMethod>setExpectedKbits</setMethod>
      <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.expectedKbits</path>
    </setting>
    <setting>
      <prompt>Use Time Stamp</prompt>
      <type>boolean</type>
      <desc>If true, the delay is calculated from the timestamp of the wrapper; if false, it is calculated from the current time.</desc>
      <default>false</default>
      <setMethod>setUseTimeStamp</setMethod>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.useTimeStamp</path>
    </setting>
    </config>
</modinfo>
