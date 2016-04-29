#!/bin/bash
source ce_run.inc
ETHDEV0=dummy0
ETHDEV1=dummy1

ce_run dll.load modEthReceiver
ce_run dll.load modEthTransmitter
ce_run channel.addChannel test1
ce_run iface.activate ${ETHDEV0}
ce_run iface.activate ${ETHDEV1}
ce_run modEthReceiver.add test1 ethrcv
ce_run modEthReceiver.connectDevice test1 ethrcv ${ETHDEV0}
ce_run modEthTransmitter.add test1 ethtx
ce_run modEthTransmitter.connectDevice test1 ethtx ${ETHDEV1}
ce_run modEthReceiver.connectOutput test1 ethrcv ethtx
ce_run modEthTransmitter.startup test1 ethtx
ce_run modEthReceiver.startup test1 ethrcv
