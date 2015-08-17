/**
 * RTCPHandler.c
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

#include "RTCPHandler.h"

rbool inactiveContextDeleter(pointer context, pointer object, size_t iterator) {
    if(((RTCPDataStruct*)object)->socket == nil) {
        return yes;
    }
    return no;
}

constructor(RTCPHandler)) {
    object = allocator(RTCPHandler);
    if(object != nil) {
        object->delegate = nil;
        object->listener = makeRSocket(nil, SOCK_STREAM, IPPROTO_TCP);
        if(object->listener != nil) {
            object->threads = c(RThreadPool)(nil);
            if(object->threads != nil) {
                object->arguments = makeRArray();
                if(object->arguments != nil) {

                    $(object->arguments, m(setDestructorDelegate, RArray)), RFree);
                    // init predicate
                    object->predicate.virtualEnumerator = inactiveContextDeleter;
                    object->predicate.context           = object;
                }
            }
        }
    }
    return object;
}

destructor(RTCPHandler) {
    deleter(object->listener,  RSocket);
    deleter(object->threads,   RThreadPool);
    deleter(object->arguments, RArray);
}

getterImpl(delegate, RTCPDelegate *, RTCPHandler)
setterImpl(delegate, RTCPDelegate *, RTCPHandler)

printer(RTCPHandler) {
    RPrintf("RTCPHandler %p -----------\n", object);
    RPrintf("\tRunning thread tuid %u\n", (unsigned) RThreadIdOfThread(&object->runningThread));
    p(RThreadPool)(object->threads);
    RPrintf("RTCPHandler %p -----------\n\n", object);
}

method(void, start, RTCPHandler), pointer context) {
    if(object->delegate != nil
            && object->delegate->delegateFunction != nil) {
        object->runningThread = currentThread();

        object->terminateFlag = no;
        $(object->threads,  m(setDelegateFunction, RThreadPool)), object->delegate->delegateFunction);
        $(object->listener, m(listen,              RSocket)),     RTCPHandlerListenerQueueSize);

        while(!object->terminateFlag) {
            RTCPDataStruct *argument = allocator(RTCPDataStruct);
            if(argument != nil) {

                argument->handler  = object;
                argument->delegate = object->delegate;
                argument->context  = context;
                argument->socket   = $(object->listener, m(accept, RSocket)));

                if(argument->socket != nil) {
                    $(object->arguments, m(addObject,  RArray)),      argument);

                    if(object->arguments->count != 0
                       && (object->arguments->count % RTCPHandlerCheckCleanupAfter) == 0) {
                        $(object->arguments, m(deleteWithPredicate, RArray)), &object->predicate);
                    }
                    $(object->threads,   m(addWithArg, RThreadPool)), argument, yes);
                } else {
                    deallocator(argument);
                }
            } elseError(
                    RError("RTCPHandler. Can't allocate thread argument.", object)
            );
        }
    } elseWarning(
            RWarning("RTCPHandler. Delegate or delgate function is nil. What do you want to start?!", object)
    );
}

method(void, terminate,  RTCPHandler)) {
    RThreadCancel(&object->runningThread);
    $(object->threads, m(cancel, RThreadPool)));
    $(object->arguments, m(flush, RArray)));
}