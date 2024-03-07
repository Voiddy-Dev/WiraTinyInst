#pragma once

#include <iostream>
#include "winsock.h"

class Communicator
{
public:
    int Read(SOCKET sock, void *buffer, size_t len);
    int Write(SOCKET sock, const char *buffer, size_t len);
};

class FuzzerCommunicator : public Communicator
{
public:
    FuzzerCommunicator();
    ~FuzzerCommunicator();
    int SendDebuggerAttached();
    int SendDebuggerDetached();
private:
    int SendCommand(char command);
    int TryConnectToServer();
    int DisconnectFromServer();

    std::string host = "127.0.0.1";
    unsigned short port = 7777;

    SOCKET sock;
};