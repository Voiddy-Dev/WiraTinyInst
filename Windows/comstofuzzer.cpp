#include "comstofuzzer.h"
#include "common.h"

int FuzzerCommunicator::SendDebuggerAttached()
{
    int sleeptime = 1000;
    int maxsleeptime = 10000;
    char reply;
    char command = 'A';
    while (1)
    {
        if (TryConnectToServer())
        {
            // send an 'A' for attached
            send(sock, &command, 1, 0);
            printf("Sent 'A' for attached\n");

            if (!Read(sock, &reply, 1))
            {
                DisconnectFromServer();
            }
            else
            {
                if (reply == 'K')
                {
                    break;
                }
                DisconnectFromServer();
            }
        }
        Sleep(sleeptime);
        sleeptime *= 2;
        if (sleeptime > maxsleeptime)
            sleeptime = maxsleeptime;
    }

    return 1;
}

int Communicator::Read(SOCKET sock, void *buf, size_t size)
{
    int ret;
    size_t to_read = size;
    char *p = (char *)buf;
    // printf("starting read\n");
    while (to_read)
    {
        int read_chunk;
        if (to_read > 0x100000)
        {
            read_chunk = 0x100000;
        }
        else
        {
            read_chunk = (int)to_read;
        }
        ret = recv(sock, p, read_chunk, 0);
        // printf("read returned %ld\n", ret);
        if (ret > 0)
        {
            to_read -= ret;
            p += ret;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

int Communicator::Write(SOCKET sock, const char *buf, size_t size)
{
    int ret;
    size_t to_write = size;
    const char *p = (char *)buf;
    while (to_write)
    {
        int write_chunk;
        if (to_write > 0x100000)
        {
            write_chunk = 0x100000;
        }
        else
        {
            write_chunk = (int)to_write;
        }
        ret = send(sock, p, write_chunk, 0);
        // printf("read returned %ld\n", ret);
        if (ret > 0)
        {
            to_write -= ret;
            p += ret;
        }
        else
        {
            return 0;
        }
    }
    return 1;
}

int FuzzerCommunicator::TryConnectToServer()
{
    struct sockaddr_in server;

    sock = INVALID_SOCKET;

    printf("Connecting to server.\n");

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        printf("Could not create socket\n");
        return 0;
    }

    // printf("Socket created.\n");

    server.sin_addr.s_addr = inet_addr(host.c_str());
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    // Connect to remote server
    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        return 0;
    }

    return 1;
}

int FuzzerCommunicator::DisconnectFromServer()
{
    if (closesocket(sock) == SOCKET_ERROR)
    {
        printf("Could not close socket\n");
        return 0;
    }
    return 1;
}

FuzzerCommunicator::FuzzerCommunicator()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData))
    {
        FATAL("WSAStartup failed");
    }
}

FuzzerCommunicator::~FuzzerCommunicator()
{
    WSACleanup();
}