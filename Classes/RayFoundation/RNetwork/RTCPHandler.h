/**
 * RTCPHandler.h
 * Threaded TCP connection handler.
 * Author Kucheruavyu Ilya (kojiba@ro.ru)
 * 6/16/15 Ukraine Kharkiv
 *  _         _ _ _
 * | |       (_|_) |
 * | | _____  _ _| |__   __ _
 * | |/ / _ \| | | '_ \ / _` |
 * |   < (_) | | | |_) | (_| |
 * |_|\_\___/| |_|_.__/ \__,_|
 *          _/ |
 *         |__/
 **/

#ifndef __R_TCP_HANDLER__
#define __R_TCP_HANDLER__

#include "RSocket.h"
#include "RayFoundation/RThread/RThreadPool.h"


#define RTCPHandlerListenerQueueSize  50
#define RTCPHandlerCheckCleanupAfter  20

struct RTCPHandler;

protocol(RTCPDelegate)
    RThreadFunction delegateFunction;
endOf(RTCPDelegate)



protocol(RTCPDataStruct)
    RSocket             *socket;
    RTCPDelegate        *delegate;
    struct RTCPHandler  *handler;
endOf(RTCPDataStruct)



class(RTCPHandler)
    REnumerateDelegate   predicate;
    RThread              runningThread;

    RTCPDelegate        *delegate;
    RSocket             *listener;
    rbool                terminateFlag;
    RThreadPool         *threads;
    RArray              *arguments;
endOf(RTCPHandler)

constructor(RTCPHandler));
destructor(RTCPHandler);

printer(RTCPHandler);

getter(delegate, RTCPDelegate *, RTCPHandler);
setter(delegate, RTCPDelegate *, RTCPHandler);

method(void, start,      RTCPHandler),    pointer context);
method(void, terminate,  RTCPHandler));

#endif /*__R_TCP_HANDLER__*/