/* event.cpp */
/**
 * Copyright - See the COPYRIGHT that is included with this distribution.
 * EPICS pvDataCPP is distributed subject to a Software License Agreement found
 * in file LICENSE that is included with this distribution.
 */
#include <cstddef>
#include <cstdlib>
#include <cstddef>
#include <string>
#include <cstdio>
#include <stdexcept>

#include <memory>
#include <vector>
#include <epicsThread.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include "noDefaultMethods.h"
#include "pvType.h"
#include "lock.h"
#include "event.h"
#include "showConstructDestruct.h"

namespace epics { namespace pvData { 

static Mutex globalMutex;
static volatile int64 totalConstruct = 0;
static volatile int64 totalDestruct = 0;
static bool notInited = true;
static String alreadyOn("already on list");


static int64 getTotalConstruct()
{
    Lock xx(&globalMutex);
    return totalConstruct;
}

static int64 getTotalDestruct()
{
    Lock xx(&globalMutex);
    return totalDestruct;
}

static void init()
{
     Lock xx(&globalMutex);
     if(notInited) {
        notInited = false;
        ShowConstructDestruct::registerCallback(
            String("event"),
            getTotalConstruct,getTotalDestruct,0,0);
     }
} 

Event::~Event() {
    epicsEventDestroy(id);
    id = 0;
    Lock xx(&globalMutex);
    totalDestruct++;
}


Event::Event(bool full)
: id(epicsEventCreate(full?epicsEventFull : epicsEventEmpty))
{
    init();
    Lock xx(&globalMutex);
    totalConstruct++;
}

void Event::signal()
{
    if(id==0) throw std::logic_error(String("event was deleted"));
    epicsEventSignal(id);
}

bool Event::wait ()
{
    if(id==0) throw std::logic_error(String("event was deleted"));
    epicsEventWaitStatus status = epicsEventWait(id);
    return status==epicsEventWaitOK ? true : false;
}

bool Event::wait ( double timeOut )
{
    if(id==0) throw std::logic_error(String("event was deleted"));
    epicsEventWaitStatus status = epicsEventWaitWithTimeout(id,timeOut);
    return status==epicsEventWaitOK ? true : false;
}

bool Event::tryWait ()
{
    if(id==0) throw std::logic_error(String("event was deleted"));
    epicsEventWaitStatus status = epicsEventTryWait(id);
    return status==epicsEventWaitOK ? true : false;
}

}}