#!/bin/bash
source ce_run.inc
CHANNEL=test1
IFACE1=emu01
IFACE2=emu02

# External stuctures
ce_run channel.addChannel ${CHANNEL}
ce_run iface.activate ${IFACE1}
ce_run iface.activate ${IFACE2}

ce_run dll.load modEthReceiver
ce_run dll.load modEthTransmitter
# ce_run dll.load modEmulateDelay

# ${IFACE1} -> ${IFACE2} configuration
ce_run modEthTransmitter.add ${CHANNEL} ethtx1
ce_run modEthTransmitter.connectDevice ${CHANNEL} ethtx1 ${IFACE2}
ce_run modEthTransmitter.startup ${CHANNEL} ethtx1

# ce_run modEmulateDelay.add ${CHANNEL} delay0
# ce_run modEmulateDelay.setDelaySeconds ${CHANNEL} delay0 d/2.0
# ce_run modEmulateDelay.setJitterSeconds ${CHANNEL} delay0 d/0.0
# ce_run modEmulateDelay.setExpectedKbits ${CHANNEL} delay0 i/100000
# ce_run modEmulateDelay.connectOutput ${CHANNEL} delay0 ethtx1
# ce_run modEmulateDelay.startup ${CHANNEL} delay0

ce_run modEthReceiver.add ${CHANNEL} ethrx0
ce_run modEthReceiver.connectDevice ${CHANNEL} ethrx0 ${IFACE1}
# ce_run modEthReceiver.connectOutput ${CHANNEL} ethrx0 delay0
ce_run modEthReceiver.connectOutput ${CHANNEL} ethrx0 ethtx1
ce_run modEthReceiver.startup ${CHANNEL} ethrx0

# ${IFACE2} -> ${IFACE1} configuration
ce_run modEthTransmitter.add ${CHANNEL} ethtx0
ce_run modEthTransmitter.connectDevice ${CHANNEL} ethtx0 ${IFACE1}
ce_run modEthTransmitter.startup ${CHANNEL} ethtx0

# ce_run modEmulateDelay.add ${CHANNEL} delay1
# ce_run modEmulateDelay.setDelaySeconds ${CHANNEL} delay1 d/1.5
# ce_run modEmulateDelay.setJitterSeconds ${CHANNEL} delay0 d/0.0
# ce_run modEmulateDelay.setExpectedKbits ${CHANNEL} delay1 i/100000
# ce_run modEmulateDelay.connectOutput ${CHANNEL} delay1 ethtx0
# ce_run modEmulateDelay.startup ${CHANNEL} delay1

ce_run modEthReceiver.add ${CHANNEL} ethrx1
ce_run modEthReceiver.connectDevice ${CHANNEL} ethrx1 ${IFACE2}
# ce_run modEthReceiver.connectOutput ${CHANNEL} ethrx1 delay1
ce_run modEthReceiver.connectOutput ${CHANNEL} ethrx1 ethtx0
ce_run modEthReceiver.startup ${CHANNEL} ethrx1
