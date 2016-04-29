<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEmulateRate</name>
  <title>Rate Emulation</title>
  <cat>Emulation</cat>
  <depends>
  </depends>
  <desc>This module accepts wrapped data of any type on its primary input. It relays the data immediately, but calculates the amount of time it would have taken to send it at the configured rate, and sleeps for that long before processing the next unit. The precision of the delay calculation can be improved by providing the tested throughput of the channel without this segment, so that value can be subtracted from the wait time. The higher the data rate, the more imprecise (usually slower) the rate becomes if the throughput value is not provided.</desc>
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
      <prompt>Rate Limit (kbit/s)</prompt>
      <type>int</type>
      <default>0</default>
      <desc>Limit the transmission speed of the channel to the specified kilobits per second.</desc>
      <setMethod>setRateLimit</setMethod>
      <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.rateLimit</path>
    </setting>
    <setting>
      <prompt>Uncapped Throughput</prompt>
      <type>int</type>
      <desc>The manually tested peak throughput of the channel without this segment functioning in kilobits per second. Improves the precision of the rate calculation.</desc>
      <default>0</default>
      <setMethod>setUnlimitedThroughput</setMethod>
      <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.unlimitedThroughput</path>
    </setting>
    <setting>
      <prompt>Timeline</prompt>
      <type>structList</type>
      <desc>Allow different rates to go into effect at different times. This is a list of time/rate pairs. Time is a floating-point number representing seconds relative to the start of the emulation; rate is in kilobits per second. They must appear in the list in chronological order; out-of-order pairs will be rejected.</desc>
      <setMethod>setRateLimit</setMethod>
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
          <prompt>Rate Limit (kbit/s)</prompt>
          <type>int</type>
          <desc dup="true">Limit the transmission speed of the channel to the specified kilobits per second.</desc>
          <min>0</min>
          <order>2</order>
        </setting>
      </structList>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.timeLine</path>
    </setting>
  </config>
</modinfo>
