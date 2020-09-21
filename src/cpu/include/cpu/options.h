#pragma once

#include <util/util.h>

class MipsInterface {
    u32 mode = 0;
    u32 version = 0;
    u32 interrupt = 0;
    u32 interruptMask = 0;
};

class RamRegisters {
    u32 config = 0;
    u32 id = 0;
    u32 delay = 0;
    u32 mode = 0;
    u32 internal = 0;
    u32 row = 0;
    u32 rasInterval = 0;
    u32 minInterval = 0;
    u32 addrSelect = 0;
    u32 manufacturer = 0;
};

class RamInterface {
    u32 mode = 0;
    u32 config = 0;
    u32 currentLoad = 0;
    u32 select = 0;
    u32 refresh = 0;
    u32 latency = 0;
    u32 readError = 0;
    u32 writeError = 0;
};

class SignalRegisters {
    u32 memAddress = 0;
    u32 dramAddress = 0;
    u32 readLength = 0;
    u32 writeLength = 0;
    u32 statusRegister = 0;
    u32 dmaFull = 0;
    u32 dmaBusy = 0;
    u32 semaphore = 0;
};

class ParallelDom {
    u32 latency;
    u32 pulseWidth;
    u32 pageSize;
    u32 release;
};

class ParallelInterface {
    u32 dramAddress;
    u32 cartAddress;
    u32 readLength;
    u32 writeLength;
    u32 status;
    ParallelDom dom1;
    ParallelDom dom2;
};
