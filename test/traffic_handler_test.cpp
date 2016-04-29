/* -*- C++ -*- */

//=============================================================================
/**
 * @file   traffic_handler_test.cpp
 * @author Tad Kollar <>
 *
 * $Id: traffic_handler_test.cpp 1670 2012-08-28 13:25:28Z tkollar $ 
 * Copyright (c) 2009.
 *      NASA Glenn Research Center.  All rights reserved.
 *      See COPYING file that comes with this distribution.
 */
//=============================================================================

#include "NetworkData.hpp"
#include "BaseTrafficHandler.hpp"

#include <ace/Reactor.h>
#include <ace/TP_Reactor.h>
#include <ace/Thread_Manager.h>

using namespace nasaCE;

static ACE_THR_FUNC_RETURN event_loop(void *arg) {
	ACE_Reactor *reactor = static_cast<ACE_Reactor *>(arg);
	reactor->owner(ACE_OS::thr_self());
	reactor->run_reactor_event_loop();

	return 0;
}

// Handle the signal when the initiator is expecting an int* return type
class ExitHandler: public ACE_Event_Handler {
	int handle_signal(int signum, siginfo_t*, ucontext_t*) {
		ND_NOTICE("Received request to exit the program.\n");		
		ACE_Reactor::instance()->suspend_handlers();
		ACE_Thread_Manager::instance()->cancel_all();				
		ACE_Reactor::instance()->end_reactor_event_loop();
		
		return 0;
	}
};

int ACE_TMAIN(int argc, ACE_TCHAR* argv[]) {
	NetworkData* alphaData = new NetworkData();
	char alphabet[] = "abcdefghijklmnopqrstuvwxyz";		
	alphaData->replicateBuffer((ACE_UINT8*)alphabet, 26);

	ExitHandler* do_exit = new ExitHandler;
	ACE_TP_Reactor tp_reactor;
	ACE_Reactor net_reactor(&tp_reactor);
	ACE_Reactor::instance(&net_reactor);
	
	ACE_Reactor::instance()->register_handler(SIGINT, do_exit);
	ACE_Reactor::instance()->register_handler(SIGTERM, do_exit);
	
	ACE_Thread_Manager::instance()->spawn_n(4, event_loop, ACE_Reactor::instance());
	
	// Declare in reverse order.
/*	BaseTrafficHandler trafficProcessor3;
	TrafficHandler<NetworkData, NetworkData> trafficProcessor2(&trafficProcessor3);
	TrafficHandler<NetworkData, NetworkData> trafficProcessor1(&trafficProcessor2);

	trafficProcessor1.putData(alphaData);
	
	ACE_Thread_Manager::instance()->wait_grp(grp_id);
	ND_NOTICE("Reactor threads are done, shutting down.\n");
	ACE_Reactor::instance()->close();
	*/

	return 0;
}
