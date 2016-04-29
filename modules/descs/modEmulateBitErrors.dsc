<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modEmulateBitErrors</name>
  <title>Bit Error Emulation</title>
  <cat>Emulation</cat>
  <depends>
  </depends>
  <desc>This module accepts wrapped data of any type on its primary input. For each received data unit, it performs a random check with a configurable probability to determine whether an error will be introduced. If so, a bit is flipped at random within the unprotected section of the buffer (the area after protectedHeaderBits and before protectedTrailerBits). The module may be configured to allow more than one error per unit, so multiple random checks are performed up to the maximum configured, causing zero to maxErrorsPerUnit flipped bits per unit. The data wrapper is unchanged, and is sent via the output primary link to the target segment.</desc>
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
      <prompt>Error Probability</prompt>
      <type>double</type>
      <default>0.0</default>
      <desc>The chance that an error will occur in any one random check as a double-precision floating-point value between 0.0 and 1.0</desc>
      <setMethod>setErrorProbability</setMethod>
      <min>0.0</min>
      <max>1.0</max>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.errorProbability</path>
    </setting>
    <setting>
      <prompt>Maximum Errors Per Unit</prompt>
      <type>int</type>
      <desc>The number of checks that will be performed on every data unit.</desc>
      <default>1</default>
      <setMethod>setMaxErrorsPerUnit</setMethod>
      <min>1</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.maxErrorsPerUnit</path>
    </setting>
    <setting>
      <prompt>Timeline</prompt>
      <desc>Allow different BER/max error pairs to go into effect at different times. This is a list of time/BER/max error groupings. Time is a floating-point number representing seconds relative to the start of the emulation; BER is a floating point number between 0.0 and 1.0; and max errors per unit is an integer greater than or equal to 1. They must appear in the list in chronological order; out-of-order pairs will be rejected.</desc>
      <type>structList</type>
      <setMethod>setErrorProbability</setMethod>
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
          <prompt>Error Probability</prompt>
          <type>double</type>
          <desc dup="true">The chance that an error will occur in any one random check as a double-precision floating-point value between 0.0 and 1.0</desc>
          <min>0.0</min>
          <max>1.0</max>
          <order>2</order>
        </setting>
        <setting>
          <prompt>Maximum Errors Per Unit</prompt>
          <type>int</type>
          <desc dup="true">The number of checks that will be performed on every data unit.</desc>
          <min>1</min>
          <order>3</order>
        </setting>
      </structList>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.timeLine</path>
    </setting>
    <setting>
      <prompt>Protected Header Bits</prompt>
      <type>int</type>
      <default>0</default>
      <desc>Prevent modification of the first specified bits (not octets) in each data unit.</desc>
      <setMethod>setProtectedHeaderBits</setMethod>
      <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.protectedHeaderBits</path>
    </setting>
    <setting>
      <prompt>Protected Trailer Bits</prompt>
      <type>int</type>
      <desc>Prevent modification of the last specified bits (not octets) in each data unit.</desc>
      <default>0</default>
      <setMethod>setProtectedTrailerBits</setMethod>
      <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.protectedTrailerBits</path>
    </setting>
  </config>
</modinfo>
