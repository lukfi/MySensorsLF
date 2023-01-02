#pragma once

#include "threads/iothread.h"
#include "net/tcp_server.h"

class Transport
{
public:
    Transport();
    void Open(uint16_t port);
    void Join()
    {
        mThread.Join();
    }
    void Send(char* buf, uint32_t size, void* cookie);

    Signal<void(TCP_Socket*)> CONNECTED;
    Signal<void(char*, uint32_t)> READ;
    Signal<void()> DISCONNECTED;
private:
    void OnClientConnected();
    void OnReadyRead(Socket*);

    void OnError(Socket*);
    LF::threads::IOThread mThread;
    TCP_Server mServer;
    TCP_Socket* mSocket {nullptr};
    char mInBuffer[64];
};
