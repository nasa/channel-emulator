/* -*- C++ -*- */

//=============================================================================
/**
 * @file   HandlerLink.cpp
 * @author Tad Kollar  
 *
 * $Id: HandlerLink.cpp 2349 2013-09-03 13:51:02Z tkollar $
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "BaseTrafficHandler.hpp"
#include "HandlerLink.hpp"
#include "NetworkData.hpp"

using namespace nasaCE;

HandlerLink::HandlerLink():
		_source(0), _sourceOutputRank(PrimaryOutput),
		_target(0), _targetInputRank(PrimaryInput) {
	ND_DEBUG("Creating new HandlerLink (default constructor).\n");

}

HandlerLink::HandlerLink(
	BaseTrafficHandler* source,
	BaseTrafficHandler* target,
	const OutputRank sourceOutputRank /* = PrimaryOutput */,
	const InputRank targetInputRank /* = PrimaryInput */ ):
		_source(0), _sourceOutputRank(sourceOutputRank),
		_target(0), _targetInputRank(targetInputRank) {

	ND_DEBUG("Creating new HandlerLink.\n");
	connectSourceToTarget(source, target, sourceOutputRank, targetInputRank);
}

HandlerLink::~HandlerLink() {
	if (_target) {
		switch (_targetInputRank) {
			case PrimaryInput:
				_target->unsetLink_(BaseTrafficHandler::PrimaryInputLink, this);
				break;
			case AuxInput:
				_target->unsetLink_(BaseTrafficHandler::AuxInputLink, this);
				break;
		}
	}
	_target = 0;

	if (_source) {
		switch (_sourceOutputRank) {
			case PrimaryOutput:
				_source->unsetLink_(BaseTrafficHandler::PrimaryOutputLink, this);
				break;
			case AuxOutput:
				_source->unsetLink_(BaseTrafficHandler::AuxOutputLink, this);
				break;
		}
	}

	_source = 0;
}

void HandlerLink::connectSourceToTarget(
		BaseTrafficHandler* source,
		BaseTrafficHandler* target,
		const OutputRank sourceOutputRank,
		const InputRank targetInputRank ) {

	if ( ! source )
		throw nd_error("HandlerLink::connectSourceToTarget() called with a null source! No connection created.\n");

	if ( ! target ) {
		ND_ERROR("[%s] HandlerLink::connectSourceToTarget() called with null target! No connection created.\n",
			source->getName().c_str());
		return;
	}

	ND_DEBUG("Connecting HandlerLink: %s/%s -> %s/%s\n",
		source->getName().c_str(), (sourceOutputRank == PrimaryOutput)? "PrimaryOut" : "AuxOut",
		target->getName().c_str(), (targetInputRank == PrimaryInput)? "PrimaryIn" : "AuxIn");

	if ( source->isCircularConnection(target) )
		throw CircularConnection("Circular output connection attempted.");

	if ( targetInputRank == AuxInput && ! target->getAuxQueue())
		throw NoAuxiliaryQueue("Target does not have an auxiliary queue.");

	_source = source;
	_sourceOutputRank = sourceOutputRank;
	_target = target;
	_targetInputRank = targetInputRank;

	switch (_targetInputRank) {
		case PrimaryInput:
			target->setLink_(BaseTrafficHandler::PrimaryInputLink, this);
			break;
		case AuxInput:
			target->setLink_(BaseTrafficHandler::AuxInputLink, this);
			break;
	}

	switch (_sourceOutputRank) {
		case PrimaryOutput:
			source->setLink_(BaseTrafficHandler::PrimaryOutputLink, this);
			break;
		case AuxOutput:
			source->setLink_(BaseTrafficHandler::AuxOutputLink, this);
			break;
	}
}

int HandlerLink::send(NetworkData* data) {
	if ( exit_requested ) return -1;

	if (!_target) throw DisconnectedLink("Target input is not configured.");

	return _target->putData(data, _targetInputRank);
}
