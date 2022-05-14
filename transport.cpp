#include "transport.h"

/********** DEBUG SETUP **********/
#define ENABLE_SDEBUG
#define DEBUG_PREFIX "Transport: "
#include "utils/singleton.h"
#include "utils/screenlogger.h"
/*********************************/


Transport::Transport()
{
    mThread.Start();
    CONNECT(mServer.CLIENT_CONNECTED, Transport, OnClientConnected);
}

void Transport::Open(uint16_t port)
{
    if (OpenStatus_t::NoError == mServer.Open(port))
    {
        mThread.RegisterWaitable(&mServer);
        SDEB("Server listening on port: %d", port);
    }
}

void Transport::Send(char *buf, uint32_t size, void *cookie)
{
    if (cookie && reinterpret_cast<TCP_Socket*>(cookie) == mSocket)
    {
        ssize_t sent = mSocket->Write(buf, size);
        SDEB("Sent %d bytes", sent);
    }
}

void Transport::OnClientConnected()
{
    TCP_Socket* socket = mServer.NextPendingConnection();
    if (!mSocket && socket)
    {
        SDEB("Client Connected");
        mSocket = socket;
        mThread.RegisterWaitable(mSocket);
        CONNECT(mSocket->READY_READ, Transport, OnReadyRead);
        CONNECT(mSocket->SIGNAL_ERROR, Transport, OnError);
        CONNECTED.Emit(mSocket);
    }
    else
    {
        delete socket;
    }

}

void Transport::OnReadyRead(Socket *)
{
    ssize_t read = mSocket->Read(mInBuffer, sizeof(mInBuffer));
    READ.Emit(mInBuffer, read);
}

void Transport::OnError(Socket *)
{
    mThread.DeleteLater(mSocket);
    SERR("Disconnected");
//    mThread.StopLater();
}
