<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>modFdReceiver</name>
  <title>File Descriptor Reader</title>
  <cat>File</cat>
  <depends></depends>
  <desc>This module reads input from a file descriptor managed by a CE File Device (or other Device that provides a file descriptor for I/O). It refers to its MRU setting to determine how much to read at once. If the device is a file, it may be reread and resent a specified number of times. Each buffer is wrapped as untyped data and sent via the primary output link to the next segment.</desc>
  <config>
    <ref>
      <id>CommonSegmentSettings</id>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.highWaterMark</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.lowWaterMark</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.auxOutput</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.MTU</ignore>
		<ignore>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.sendIntervalUsec</ignore>
    </ref>
    <setting>
      <prompt>Repetitions</prompt>
      <type>int</type>
      <desc>Re-read and re-send the file the specified number of times. If zero, the file is sent only once; if one, it is sent twice; etc.</desc>
      <min>0</min>
      <path>Channels.XYZ_CHANNEL_XYZ.XYZ_MODNAME_XYZ.repeatMax</path>
    </setting>
  </config>
</modinfo>
