<?xml version="1.0" encoding="UTF-8" ?>
<modinfo xmlns="http://channel-emulator.grc.nasa.gov/module-properties">
  <name>devSource</name>
  <title>Traffic Generator</title>
  <desc>This device generates a continuous, repeating pattern and writes it to a pipe, which in Linux has a buffer of 64k by default. Another module, typically modFdReceiver, will read blocks from the buffer and encapsulate each block in a network data wrapper of any type.</desc>
  <cat>device</cat>
  <depends></depends>
  <config>
    <ref>
      <id>CommonDeviceSettings</id>
    </ref>
    <setting>
      <prompt>Pattern</prompt>
      <type>int</type>
      <desc>Set the repeating pattern to the specified sequence, where each element of is an integer value in the range 0 to 255 (or 0x00 to 0xff)</desc>
	  <array/>
      <default>0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15</default>
	  <setMethod>setPattern</setMethod>
      <path>Devices.XYZ_MODNAME_XYZ.pattern</path>
    </setting>
    <setting>
      <prompt>Pattern Buffer Size</prompt>
      <type>int</type>
      <desc>A buffer with a length of this many octets will be created and filled with the current pattern</desc>
      <default>10240</default>
	  <setMethod>setPatternBufferSize</setMethod>
      <path>Devices.XYZ_MODNAME_XYZ.patternBufferSize</path>
	</setting>
  </config>
</modinfo>
