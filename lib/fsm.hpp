/*!
    @file  fsm.hpp
    @brief Finite state machine templates include file
 */
// Obtained from http://satsky.spb.ru/articles/fsm/fsmEng.php
//
// Date:      23 January  2003
// Modified:  29 November 2003
//            01 December 2003
//            31 July     2004
//            01 August   2004
//            07 August   2004   Doxygen remarks
//            20 August   2005   exception safety changes
//
// Copyright (c) Sergey Satskiy 2003 - 2005
//               <sergesatsky@yahoo.com>
//
// Permission to copy, use, modify, sell and distribute this software 
// is granted provided this copyright notice appears in all copies. 
// This software is provided "as is" without express or implied
// warranty, and with no claim as to its suitability for any purpose.
//


#ifndef FSMTEMPLATE_H
#define FSMTEMPLATE_H

#include <vector>
#include <algorithm>
#include <stdexcept>
#include <deque>
#include <string>
#include <functional>
#include <iterator>
#include <iostream>
#include <boost/function.hpp>




    /*! Used to start a finite state machine description in a constructor. 
        Used for the template which bounds functions to states. 
        Preliminary requirements: FSMStateType and FSMEventType 
        symbolic names must be defined. 
     */
#define FSM_BEGIN( StartFSMState ) (StartFSMState),                         \
                                   FSM::SStateMachineProxy<FSMStateType,    \
                                                           FSMEventType>    \
                                       () << (                              \
                                   FSM::SStatesListProxy<FSMStateType>() <<


    /*! Used to provide a list of legal states in a constructor. 
        Used for the template which bounds functions to states. 
        There are no preliminary requirements. 
     */
#define FSM_STATES


    /*! Used to provide a list of transitions for a single event 
        in a constructor. Used for the template which bounds functions to 
        states. Preliminary requirements: FSMStateType and FSMEventType 
        symbolic names must be defined.
     */
#define FSM_EVENT( Arg )           )                                        \
                                   << (FSM::STransitionsProxy<FSMStateType, \
                                                              FSMEventType> \
                                      ( Arg ) <<


    /*! Used to finish a finite state machine description in a constructor. 
        Used for the template which bounds functions to states. 
        There are no preliminary requirements.
     */
#define FSM_END                    )


    /*! Used to reset a finite state machine to the initail state. 
        Used for both versions of the template. 
        There are no preliminary requirements.
     */
#define FSM_RESET                   FSM::ResetMachine




    /*! Used to start a finite state machine description in a constructor. 
        Used for the template which bounds functions to transitions. 
        Preliminary requirements: FSMStateType and FSMEventType symbolic 
        names must be defined.
     */
#define FUNCFSM_BEGIN( StartFSMState ) (StartFSMState),                         \
                                   FSM::SFuncStateMachineProxy<FSMStateType,    \
                                                               FSMEventType>    \
                                       () << (                                  \
                                   FSM::SStatesListProxy<FSMStateType>() <<


    /*! Used to provide a list of legal states in a constructor. Used for 
        the template which bounds functions to transitions. 
        There are no preliminary requirements. 
     */
#define FUNCFSM_STATES


    /*! Used to provide a list of transitions for a single event in a constructor. 
        Used for the template which bounds functions to transitions. 
        Preliminary requirements: FSMStateType and FSMEventType symbolic 
        names must be defined.
     */
#define FUNCFSM_EVENT( Arg )           )                                        \
                                   << (FSM::SFuncTransitionsProxy<FSMStateType, \
                                                                  FSMEventType> \
                                      ( Arg ) <<


    /*! Used to finish a finite state machine description in a constructor. 
        Used for the template which bounds functions to transitions. 
        There are no preliminary requirements.
     */
#define FUNCFSM_END                    )


    /*! Used to reset a finite state machine to the initail state. Used for both 
        versions of the template. There are no preliminary requirements. 
     */
#define FUNCFSM_RESET                   FSM::ResetMachine
                                      



    /*! @brief All the FSMs related classes are defined here.
        FSM help defines directives are located outside of the namespace.
     */
namespace FSM
{

        //! @brief Machine manipulators are defined here.
    enum SMachineManipulator 
    { 
        ResetMachine = 0        //!< Manipulator resets the machine to the initial state.
    };


        //! Transition manipulators are defined here.
    enum STransitionManipulator 
    { 
        NONE = 0,               //!< Used to mark a "nothing to do" transition.
        EXCEPTION = 1           //!< Used to mark a transition which should generate an exception.
    };
    

        //! Exception definition for the state machine template class.
    class SStateMachineException : public std::exception
    {
        private:
            const std::string      Message;

        public:
                //! Construct an exception object
                //! @param Msg Exception reason description.
            SStateMachineException( const std::string &  Msg ) : Message( Msg ) 
            {}

                //! Construct an exception object
                //! @param Msg Exception reason description.
            SStateMachineException( const char *         Msg ) : Message( Msg ) 
            {}

                //! Return an exception reason description 
                //! @return Exception reason.
            virtual const char * what( void ) const throw() 
            { return Message.c_str(); }

                //! Destroy an exception object. 
            virtual ~SStateMachineException() throw () {}

        private:
            SStateMachineException();
    };


        /*! @brief Predefined strategy for the template which bounds functions to states. 
                   Strategy supposes no entry, no exit functions calls. 
            @param SState User defined state type 
            @param SEvent User defined event type
         */
    template < typename SState, typename SEvent >
    class SEmptyFunctor
    {
        public:
                //! Calls no functions. Inlined to nothing.
            void operator() ( const SState &, 
                              const SEvent &, 
                              const SState &  ) 
            { return; }
    };


        /*! @brief Predefined strategy for the template which bounds functions to states. 
                   Strategy supposes entry functions calls only.
            @param SState User defined state type 
            @param SEvent User defined event type
         */
    template < typename SState, typename SEvent >
    class SOnEnterFunctor
    {
        public:
                /*! @brief Calls OnEnter(...) function of the To state.
                    @param From A state we move from
                    @param Event An event which was a transition reason
                    @param To A state we move to.
                 */
            void operator() ( SState &        From, 
                              const SEvent &  Event, 
                              SState &        To )
            { To.OnEnter( From, Event ); }
    };


        /*! @brief Predefined strategy for the template which bounds functions to states. 
                   Strategy supposes exit functions calls only.
            @param SState User defined state type 
            @param SEvent User defined event type
         */
    template < typename SState, typename SEvent >
    class SOnExitFunctor
    {
        public:
                /*! @brief Calls OnExit(...) function of the To state. 
                    @param From A state we move from
                    @param Event An event which was a transition reason
                    @param To A state we move to.
                 */
            void operator() ( SState &        From, 
                              const SEvent &  Event, 
                              SState &        To )
            { From.OnExit( Event, To ); }
    };


        /*! @brief Predefined strategy for the template which bounds functions to states. 
                   Strategy supposes both entry and exit functions calls.
            @param SState User defined state type 
            @param SEvent User defined event type
         */
    template < typename SState, typename SEvent >
    class SOnMoveFunctor
    {
        public:
                /*! @brief Calls OnExit(...) function of the From state first and 
                           then OnEnter(...) function of the To state.
                    @param From A state we move from
                    @param Event An event which was a transition reason
                    @param To A state we move to.
                 */
            void operator() ( SState &        From, 
                              const SEvent &  Event, 
                              SState &        To )
            { 
              const SState &  FromConst( From );
              const SState &  ToConst( To );
  
              From.OnExit( Event, ToConst ); 
              To.OnEnter( FromConst, Event ); 
            }
    };
        

        /*! @brief Predefined strategy for both versions of the template. 
                   Strategy supposes throwing exception in case of the unknown event. 
            @param SEvent User defined event type
         */
    template < typename SEvent >
    class SThrowStrategy
    {
        public:
                /*! @brief Throws the SStateMachineException. Parameter is unused.
                    @exception SStateMachineException  with the following 
                               message: "Unknown event."
                 */
            void operator() ( const SEvent & ) const 
            { throw SStateMachineException( "Unknown event." ); }
    };


        /*! @brief Predefined strategy for both versions of the template. 
                   Strategy supposes ignoging the unknown event. 
            @param SEvent User defined event type
         */
    template < typename SEvent >
    class SIgnoreStrategy
    {
        public:
                //! Does nothing. Parameter is unused.
            void operator() ( const SEvent & ) const 
            { return; }
    };



    //! @cond SKIP

        //
        // Temporary class to collect all the registered states
        //
    template < typename SState >
    class SStatesListProxy
    {
        private:
            std::vector<SState>    StatesProxy;

        public:
            SStatesListProxy() { return; }

            SStatesListProxy &  operator << ( const SState &  TheState )
            { 
                    // Check whether the state is unique
                if ( std::find( StatesProxy.begin(), 
                                StatesProxy.end(), 
                                TheState ) != StatesProxy.end() )
                {
                    throw SStateMachineException( "States must be unique." );
                }
                StatesProxy.push_back( TheState ); 
                return *this; 
            }

            const std::vector<SState> &  GetStates( void ) const 
            { return StatesProxy; }
    };

    
    //! @endcond


    //! @cond SKIP

        //
        // Temporary class to collect transitions for a single event
        //
    template < typename SState, typename SEvent >
    class STransitionsProxy
    {
        private:
            SEvent                  Event;
            std::vector<SState>     NewStates;
            std::vector< int >      Manipulators;

        public:
            explicit STransitionsProxy( const SEvent &  TheEvent ) : 
                Event( TheEvent ) 
            { return; }

            STransitionsProxy & operator << ( const SState &  TheState ) 
            { 
                NewStates.push_back( TheState ); 
                Manipulators.push_back( -1 );
                return *this; 
            }

            STransitionsProxy & operator << ( const STransitionManipulator &  Manipulator )
            {
                NewStates.push_back( SState() );
                Manipulators.push_back( Manipulator );
                return *this;
            }

            const std::vector<SState> &  GetStates( void ) const 
            { return NewStates; }

            const std::vector<int> &  GetManipulators( void ) const
            { return Manipulators; }

            const SEvent &  GetEvent( void ) const 
            { return Event; }

        private:
            STransitionsProxy();
    };

    //! @endcond



    //! @cond SKIP

        //
        // Temporary class to collect all the transitions
        //
    template < typename SState, typename SEvent >
    class SStateMachineProxy
    {
            typedef SState                   StateType;
            typedef SEvent                   EventType;
            typedef std::vector<StateType>   SStatesList;
            typedef std::vector<EventType>   SEventsList;

            typedef std::vector< int >       SManipulatorsList;

        private:
            SStatesList                     MachineStatesProxy;
            bool                            HaveStates;
            SEventsList                     MachineEventsProxy;
            std::vector<SStatesList>        Transitions;
            std::vector<SManipulatorsList>  TransitionsManipulators;
    
        public:
            SStateMachineProxy() : HaveStates( false ) 
            { return; }

            SStateMachineProxy & operator << 
                ( const SStatesListProxy<SState> &   TheStates )
            { 
                if ( HaveStates ) 
                {
                    throw SStateMachineException( "Too much state lists." );
                }
                HaveStates = true;
                MachineStatesProxy = TheStates.GetStates();
                return *this; 
            }

            SStateMachineProxy & operator << 
                ( const STransitionsProxy<SState,SEvent> &  TheTransitions )
            { 
                if ( !HaveStates ) 
                {
                    throw SStateMachineException( "States must be defined "
                                                  "before the transitions." );
                }
            
                if ( MachineStatesProxy.size() != 
                         TheTransitions.GetStates().size() ) 
                {
                    throw SStateMachineException( "Transition states do not "
                                                  "coincide to the states list." );
                }

                    // Check that the event is unique
                if ( std::find( MachineEventsProxy.begin(), 
                                MachineEventsProxy.end(), 
                                TheTransitions.GetEvent() ) 
                         != MachineEventsProxy.end() ) 
                {
                    throw SStateMachineException( "Events must be unique." );
                }
                    
                    // Check the transition validity
                CheckTransitionValidity( TheTransitions.GetStates().begin(), 
                                         TheTransitions.GetStates().end(),
                                         TheTransitions.GetManipulators().begin(),
                                         TheTransitions.GetManipulators().begin() );

                MachineEventsProxy.push_back( TheTransitions.GetEvent() );
                Transitions.push_back( TheTransitions.GetStates() );
                TransitionsManipulators.push_back( TheTransitions.GetManipulators() );
                return *this; 
            }
                
                
                //
                // Check that the transitions lead to a registered state
                //
            void CheckTransitionValidity( 
                    typename SStatesList::const_iterator  First, 
                    typename SStatesList::const_iterator  Last,
                    std::vector<int>::const_iterator      ManipulatorFirst,
                    std::vector<int>::const_iterator      ManipulatorLast )
            {
                for ( ; First != Last; ++First, ++ManipulatorFirst )
                {
                    if ( *ManipulatorFirst == -1 )  // It's not a manipulator
                    {
                        if ( std::find( MachineStatesProxy.begin(), 
                                        MachineStatesProxy.end(), *First ) 
                                 == MachineStatesProxy.end() ) 
                        {
                            throw SStateMachineException( "Transition state "
                                                          "is not valid." );
                        }
                    }
                }
            }       

            const SStatesList &  GetStates( void ) const
            { return MachineStatesProxy; }

            const std::vector<SStatesList> & GetTransitions( void ) const
            { return Transitions; }

            const std::vector<SManipulatorsList> &  GetManipulators( void ) const
            { return TransitionsManipulators; }

            const SEventsList &  GetEvents( void ) const
            { return MachineEventsProxy; }
    };

    //! @endcond


            
        /*! @brief Finite state machine template which bounds functions to the states.
            @param SState User defined state type.
            @param SEvent User defined event type.
            @param SFunctor User defined startegy of functions calls. 
                            Default strategy is no calls.
            @param SUnknownEventStrategy User defined strategy of reacting on 
                                         the unknown event. Default strategy is 
                                         throwing an exception.
         */
    template < typename SState, 
               typename SEvent, 
               typename SFunctor = SEmptyFunctor<SState,SEvent>, 
               typename SUnknownEventStrategy = SThrowStrategy<SEvent> > 
    class SStateMachine
    {
        public:
            //! @cond SKIP
            typedef SState                          StateType;
            typedef SEvent                          EventType;
            typedef std::vector<StateType>          SStatesList;
            typedef std::vector<EventType>          SEventsList;
            typedef std::vector<SStatesList>        TransitionsType;
            typedef std::deque<EventType>           EventsQueueType;
            typedef std::vector<int>                ManipulatorsList;
            typedef std::vector<ManipulatorsList>   ManipulatorsType;
            //! @endcond

        private:
            StateType              CurrentState;          // Current machine state
            SStatesList            States;                // A list of the registered states
            SEventsList            Events;                // A list of the registered events
            TransitionsType        Transitions;           // A table of transitions between states
            ManipulatorsType       Manipulators;          // A table of manipulators
            SFunctor               Functor;               // Transition function
            SUnknownEventStrategy  UnknownEventStrategy;  // Unknown event strategy
            EventsQueueType        EventsQueue;           // Internal events queue to support events 
                                                          //   that were generated in the transition 
                                                          //   functions
            bool                   InProcess;             // To be sure that we are processing the events
            int                    CurrentStateIndex;     // Index of column in a transition table 
                                                          //   (0 - based)
            StateType              InitialState;          // Start machine state

        public:
                /*! @brief The only way to cunstruct a state machine is to provide 
                           a complete description.
                    @param StartState Finite state machine start state
                    @param ProxyMachine The machine transition table
                    @exception SStateMachineException  if any problem is found 
                                                       in the machine description.
                 */
            SStateMachine( const StateType &  StartState, 
                           const SStateMachineProxy<StateType,
                                                    EventType> &  ProxyMachine ) :
                CurrentState( StartState ), 
                States( ProxyMachine.GetStates() ),
                Events( ProxyMachine.GetEvents() ), 
                Transitions( ProxyMachine.GetTransitions() ), 
                Manipulators( ProxyMachine.GetManipulators() ),
                InProcess( false ),
                CurrentStateIndex( -1 ), 
                InitialState( StartState )
            {
                    // Check that the start state is valid
                typename SStatesList::iterator  k( std::find( States.begin(), 
                                                              States.end(), 
                                                              StartState ) );
                if ( k == States.end() ) 
                {
                    throw SStateMachineException( "Start state is invalid." );
                }
                CurrentStateIndex = std::distance( States.begin(), k );
            }


                /*! @brief Reset the machine to the initial state. 
                           Warning: no transition functions will be called if any.
                    @return A reference to the machine
                 */
            SStateMachine & Reset( void ) 
            {
                CurrentState = InitialState; 
                CurrentStateIndex = GetStateIndex( CurrentState );
                InProcess = false;
                EventsQueue.clear();
                return *this;
            }


                /*! @brief Returns the current machine state.
                    @return The current state.
                 */
            StateType GetCurrentState( void ) const 
            { return CurrentState; }



                /*! @brief Accepts an event. First the transition will be executed and then 
                           the bound functions (if any) will be called.
                    @param Event An event that should be processed.
                    @return A reference to the machine itself.
                    @exception In case of any exception (default Unknown even strategy,
                               user functor exception etc) the machine will be reset.
                 */
            SStateMachine &  AcceptEvent( const EventType &  Event )
            {
                EventsQueue.push_back( Event );
                if ( InProcess ) return *this;
                InProcess = true;
                while ( !EventsQueue.empty() )
                {
                    ProcessEvent( EventsQueue.front() );
                    EventsQueue.pop_front();
                }
                InProcess = false;
                return *this;
            }


                /*! @brief Another way to accept an event. First the transition will be executed 
                           and then the bound functions (if any) will be called.
                    @param Event An event that should be processed.
                    @return A reference to the machine itself.
                    @exception In case of any exception (default Unknown even strategy,
                               user functor exception etc) the machine will be reset.
                 */
            SStateMachine & operator << ( const EventType &  Event )
            { return AcceptEvent( Event ); }


                /*! @brief Process a machine manipulator.
                    @param Manipulator A manipulator to be processed.
                    @return A reference to the machine itself.
                 */
            SStateMachine & operator << ( SMachineManipulator Manipulator )
            { 
                if ( Manipulator != ResetMachine ) 
                {
                    Reset();
                    throw SStateMachineException( "Unknown manipulator. Machine is reset." );
                }
                return Reset();
            } 
        
        private:
            void ProcessEvent( const EventType &  Event )
            {   
                try
                {
                    int  EventIndex( GetEventIndex( Event ) );
                    if ( EventIndex == -1 ) return;

                    StateType      OldState( CurrentState );
            
                    if ( (Manipulators[EventIndex])[CurrentStateIndex] == EXCEPTION )
                    {
                        throw SStateMachineException( "Transition is forbidden. Machine is reset." );
                    }
                
                    if ( (Manipulators[EventIndex])[CurrentStateIndex] == NONE )
                    {
                        return;
                    }

                    CurrentState = (Transitions[EventIndex])[CurrentStateIndex];
                    CurrentStateIndex = GetStateIndex( CurrentState );

                    Functor( OldState, Event, CurrentState );
                }
                catch ( ... )
                {
                    Reset();
                    throw;
                }
            }
        
            int GetEventIndex( const EventType &  Event ) const
            {   
                typename SEventsList::const_iterator   k( std::find( Events.begin(), 
                                                                     Events.end(), 
                                                                     Event ) );
                if ( k == Events.end() ) 
                { 
                    UnknownEventStrategy( Event ); 
                    return -1; 
                }
                return std::distance( Events.begin(), k );
            }

            int GetStateIndex( const StateType &  State ) const
            {
                return std::distance( States.begin(), 
                                      std::find( States.begin(), 
                                                 States.end(), 
                                                 State ) );
            }

        private:
                //
                // Prevent the illegal usage
                //
            SStateMachine();
            SStateMachine & operator= ( const SStateMachine & );
            SStateMachine( const SStateMachine & );
    };
    


        /*! @brief Allow writing the current machine state to an output stream.
            @param Stream Reference to the output stream.
            @param Machine A machine whose state should be outputted.
            @return A reference to a stream
         */
    template < typename SState, 
               typename SEvent, 
               typename SFunctor, 
               typename SUnknownEventStrategy > 
        std::ostream & operator << (std::ostream &  Stream, 
                                    const SStateMachine<SState,
                                                        SEvent,
                                                        SFunctor,
                                                        SUnknownEventStrategy> &  Machine )
    { return Stream << Machine.GetCurrentState(); }
    



        //
        // Second vertion of the finite state machine where functions are
        // linked to transitions but not to the states
        //
    

        /*! @brief Base class for the states to allow transition function specification
            @param Child A type which derives from the StateBase
            @param Event A user defined state type.
         */
    template < typename  Child,
               typename  Event >
    class StateBase
    {
            // Should all of them be const? - No. It is easy to imagine that
            // some fields in the states and events should be changed
        typedef boost::function< void ( Child &,
                                        Event &,
                                        Child & ) >   CallbackType;

        private:
            CallbackType    Callback;

        protected:
            StateBase() : Callback( 0 ) {}

        public:
                /*! @brief Memorises the callback for the transition.
                    @param NewCallback A callback to remember
                    @return A reference to the user defined state type object
                 */
            Child & operator [] ( const CallbackType &  NewCallback )
            {
                Callback = NewCallback;
                return static_cast< Child & >( *this );
            }
            
                /*! @brief Provides a memorized callback. 
                           Second call of the method will return 0 - callback.
                    @return A callback
                 */
            CallbackType  GetCallback( void )
            {
            	/*
                CallbackType        Temporary( Callback );

                Callback = 0;
                return Temporary;
                */
                return Callback;
            }
    };


    //! @cond SKIP

        // A bundle of the state related information
    template < typename SState, typename SEvent >
    struct SFuncBundle
    {
        SState                                  NewState;
        int                                     Manipulator;
        boost::function< void ( SState &,
                                SEvent &,
                                SState & ) >    Callback;

        SFuncBundle() {}
    };

    //! @endcond


        // Start state spec is the same as in the first version
        // States list is the same as in the first version

        // Transition accepting class is different.
        // Let's define it.


    //! @cond SKIP

        //
        // Temporary class to collect transitions for a single event
        //
    template < typename SState, typename SEvent >
    class SFuncTransitionsProxy
    {
        typedef SFuncBundle< SState, SEvent >   Bundle;
        
        private:
            SEvent                  Event;
            std::vector<Bundle>     NewStates;

        public:
            explicit SFuncTransitionsProxy( const SEvent &  TheEvent ) : 
                Event( TheEvent ) 
            { return; }

            SFuncTransitionsProxy & operator << ( SState &  TheState ) 
            { 
                Bundle      NewBundle;

                NewBundle.NewState = TheState;
                NewBundle.Manipulator = -1;
                NewBundle.Callback = TheState.GetCallback();

                NewStates.push_back( NewBundle );
                return *this; 
            }

            SFuncTransitionsProxy & operator << ( const STransitionManipulator &  Manipulator )
            {
                Bundle      NewBundle;

                NewBundle.NewState = SState();
                NewBundle.Manipulator = Manipulator;
                NewBundle.Callback = 0;
                
                NewStates.push_back( NewBundle );
                return *this;
            }

            const std::vector<Bundle> &  GetStates( void ) const 
            { return NewStates; }

            const SEvent &  GetEvent( void ) const 
            { return Event; }

        private:
            SFuncTransitionsProxy();
    };

    //! @endcond


               
    //! @cond SKIP

        //
        // Temporary class to collect all the transitions
        //
    template < typename SState, typename SEvent >
    class SFuncStateMachineProxy
    {
        typedef SFuncBundle< SState, SEvent >   Bundle;
        
        typedef SState                   StateType;
        typedef SEvent                   EventType;
        typedef std::vector<StateType>   SStatesList;
        typedef std::vector<EventType>   SEventsList;
        typedef std::vector< Bundle >    SBundlesList;

        private:
            SStatesList                     MachineStatesProxy;
            bool                            HaveStates;
            SEventsList                     MachineEventsProxy;
            std::vector<SBundlesList>       Transitions;
    
        public:
            SFuncStateMachineProxy() : HaveStates( false ) 
            { return; }

            SFuncStateMachineProxy & operator << 
                ( const SStatesListProxy<SState> &   TheStates )
            { 
                if ( HaveStates ) 
                {
                    throw SStateMachineException( "Too much state lists." );
                }
                HaveStates = true;
                MachineStatesProxy = TheStates.GetStates();
                return *this; 
            }

            SFuncStateMachineProxy & operator << 
                ( const SFuncTransitionsProxy<SState,SEvent> &  TheTransitions )
            { 
                if ( !HaveStates ) 
                {
                    throw SStateMachineException( "States must be defined "
                                                  "before the transitions." );
                }
            
                if ( MachineStatesProxy.size() != 
                         TheTransitions.GetStates().size() ) 
                {
                    throw SStateMachineException( "Transition states do not "
                                                  "coincide to the states list." );
                }

                    // Check that the event is unique
                if ( std::find( MachineEventsProxy.begin(), 
                                MachineEventsProxy.end(), 
                                TheTransitions.GetEvent() ) 
                         != MachineEventsProxy.end() ) 
                {
                    throw SStateMachineException( "Events must be unique." );
                }
                    
                    // Check the transition validity
                CheckTransitionValidity( TheTransitions.GetStates().begin(), 
                                         TheTransitions.GetStates().end()      );

                MachineEventsProxy.push_back( TheTransitions.GetEvent() );
                Transitions.push_back( TheTransitions.GetStates() );
                return *this; 
            }
                
                
                //
                // Check that the transitions lead to a registered state
                //
            void CheckTransitionValidity( 
                    typename std::vector<Bundle>::const_iterator  First, 
                    typename std::vector<Bundle>::const_iterator  Last )
            {
                for ( ; First != Last; ++First )
                {
                    if ( First->Manipulator == -1 )  // It's not a manipulator
                    {
                        if ( std::find( MachineStatesProxy.begin(), 
                                        MachineStatesProxy.end(), First->NewState ) 
                                 == MachineStatesProxy.end() ) 
                        {
                            throw SStateMachineException( "Transition state "
                                                          "is not valid." );
                        }
                    }
                }
            }       

            const SStatesList &  GetStates( void ) const
            { return MachineStatesProxy; }

            const std::vector<SBundlesList> & GetTransitions( void ) const
            { return Transitions; }

            const SEventsList &  GetEvents( void ) const
            { return MachineEventsProxy; }
    };

    //! @endcond



        /*! @brief Finite state machine template which bounds functions to the transitions.
            @param SState User defined state type.
            @param SEvent User defined event type
            @param SUnknownEventStrategy User defined strategy of reacting on the 
                                         unknown event. Default strategy is 
                                         throwing an exception.
         */
    template < typename SState, 
               typename SEvent, 
               typename SUnknownEventStrategy = SThrowStrategy<SEvent> > 
    class SFuncStateMachine
    {
        public:
        //! @cond SKIP
        typedef SFuncBundle< SState, SEvent >   Bundle;

        typedef SState                          StateType;
        typedef SEvent                          EventType;
        typedef std::vector<StateType>          SStatesList;
        typedef std::vector<EventType>          SEventsList;
        typedef std::vector<SStatesList>        TransitionsType;
        typedef std::deque<EventType>           EventsQueueType;
        typedef std::vector< Bundle >           SBundlesList;
        //! @endcond

        private:
            StateType                   CurrentState;          // Current machine state
            SStatesList                 States;                // A list of the registered states
            SEventsList                 Events;                // A list of the registered events
            std::vector<SBundlesList>   Transitions;           // A table of transitions between states
            SUnknownEventStrategy       UnknownEventStrategy;  // Unknown event strategy
            EventsQueueType             EventsQueue;           // Internal events queue to support events 
                                                               //   that were generated in the 
                                                               //   transition functions
            bool                        InProcess;             // To be sure that we are processing 
                                                               //   the events
            int                         CurrentStateIndex;     // Index of column in a transition 
                                                               //   table (0 - based)
            StateType                   InitialState;          // Start machine state

        public:
                /*! @brief The only way to construct a state machine is to 
                           provide a complete description. 
                    @param StartState Finite state machine start state
                    @param ProxyMachine The machine transition table
                    @exception SStateMachineException if any problem is found in the machine description.
                 */
            SFuncStateMachine( const StateType &  StartState, 
                               const SFuncStateMachineProxy<StateType,
                                                            EventType> &  ProxyMachine ) :
                CurrentState( StartState ), 
                States( ProxyMachine.GetStates() ),
                Events( ProxyMachine.GetEvents() ), 
                Transitions( ProxyMachine.GetTransitions() ), 
                InProcess( false ),
                CurrentStateIndex( -1 ), 
                InitialState( StartState )
            {
                    // Check that the start state is valid
                typename SStatesList::iterator  k( std::find( States.begin(), 
                                                              States.end(), 
                                                              StartState ) );
                if ( k == States.end() ) 
                {
                    throw SStateMachineException( "Start state is invalid." );
                }
                CurrentStateIndex = std::distance( States.begin(), k );
            }


                /*! @brief Reset the machine to the initial state. 
                           Warning: no transition functions will be called if any.
                    @return A reference to the start state
                 */
            SFuncStateMachine & Reset( void ) 
            {
                CurrentState = InitialState; 
                CurrentStateIndex = GetStateIndex( CurrentState );
                InProcess = false;
                EventsQueue.clear();
                return *this;
            }


                /*! @brief Returns the current machine state.
                    @return The current state.
                 */
            StateType GetCurrentState( void ) const 
            { return CurrentState; }


                /*! @brief Accepts an event. First the transition will be executed and 
                           then the bound functions (if any) will be called.
                    @param Event An event that should be processed.
                    @return A reference to the machine itself.
                    @exception In case of any exception (default Unknown even strategy,
                               user functor exception etc) the machine will be reset.
                 */
            SFuncStateMachine &  AcceptEvent( EventType &  Event )
            {
                EventsQueue.push_back( Event );
                if ( InProcess ) return *this;
                InProcess = true;
                while ( !EventsQueue.empty() )
                {
                    ProcessEvent( EventsQueue.front() );
                    EventsQueue.pop_front();
                }
                InProcess = false;
                return *this;
            }

                /*! @brief Another way to accept an event. First the transition will be 
                           executed and then the bound functions (if any) will be called.
                    @param Event Event An event that should be processed.
                    @return A reference to the machine itself.
                    @exception In case of any exception (default Unknown even strategy,
                               user functor exception etc) the machine will be reset.
                 */
            SFuncStateMachine & operator << ( EventType &  Event )
            { return AcceptEvent( Event ); }


                /*! @brief Process a machine manipulator.
                    @param Manipulator A manipulator to be processed.
                    @return A reference to the machine itself.
                 */
            SFuncStateMachine & operator << ( SMachineManipulator Manipulator ) 
            { 
                if ( Manipulator != ResetMachine ) 
                {
                    Reset();
                    throw SStateMachineException( "Unknown manipulator. Machine is reset." );
                }
                return Reset();
            } 
        
        private:
            void ProcessEvent( EventType &  Event )
            {   
                try
                {
                    int  EventIndex( GetEventIndex( Event ) );
                    if ( EventIndex == -1 ) return;

                    StateType      OldState( CurrentState );
            
                    if ( Transitions[EventIndex][CurrentStateIndex].Manipulator == EXCEPTION )
                    {
                        throw SStateMachineException( "Transition is forbidden. Machine is reset." );
                    }

                    if ( Transitions[EventIndex][CurrentStateIndex].Manipulator == NONE )
                    {
                        return;
                    }

                    boost::function< void ( SState &, SEvent &, SState & ) >    
                            Functor( Transitions[EventIndex][CurrentStateIndex].Callback );
                
                    CurrentState = Transitions[EventIndex][CurrentStateIndex].NewState;
                    CurrentStateIndex = GetStateIndex( CurrentState );

                    if ( Functor != 0 )     Functor( OldState, Event, CurrentState );
                }
                catch ( ... )
                {
                    Reset();
                    throw;
                }
            }
        

            int GetEventIndex( const EventType &  Event ) const
            {   
                typename SEventsList::const_iterator   k( std::find( Events.begin(), 
                                                                     Events.end(), 
                                                                     Event ) );
                if ( k == Events.end() ) 
                { 
                    UnknownEventStrategy( Event ); 
                    return -1; 
                }
                return std::distance( Events.begin(), k );
            }

            int GetStateIndex( const StateType &  State ) const
            {
                return std::distance( States.begin(), 
                                      std::find( States.begin(), 
                                                 States.end(), 
                                                 State ) );
            }

        private:
                //
                // Prevent the illegal usage
                //
            SFuncStateMachine();
            SFuncStateMachine & operator= ( const SFuncStateMachine & );
            SFuncStateMachine( const SFuncStateMachine & );
    };
    

        /*! @brief Allow writing the current machine state to an output stream.
            @param Stream Reference to the output stream.
            @param Machine A machine whose state should be outputted.
         */
    template < typename SState, 
               typename SEvent, 
               typename SUnknownEventStrategy > 
        std::ostream & operator << (std::ostream &  Stream, 
                                    const SFuncStateMachine<SState,
                                                            SEvent,
                                                            SUnknownEventStrategy> &  Machine )
    { return Stream << Machine.GetCurrentState(); }
    
    
               
}   // End of the FSM namespace

#endif

