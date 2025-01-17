#include "comstofuzzer.h"
#include "common.h"

int FuzzerCommunicator::SendCommand(char command)
{
    int sleeptime = 100;
    int maxsleeptime = 10000;
    char reply;

    while (1)
    {
        char reply;
        if (TryConnectToServer())
        {
            send(sock, &command, 1, 0);
            // printf("[TINYINST WIRA] Sent command: %c\n", command);

            if (!Read(sock, &reply, 1))
            {
                DisconnectFromServer();
                return 0;
            }
            if (reply == 'K')
            {
                return 1;
            }
            DisconnectFromServer();
        }
        Sleep(sleeptime);
        sleeptime *= 2;
        if (sleeptime > maxsleeptime)
            FATAL("Could not connect to server after 10 seconds\n");
    }
    return 0;
}

int FuzzerCommunicator::SendDebuggerAttached()
{
    char command = 'A';
    return SendCommand(command);
}

int FuzzerCommunicator::SendDebuggerDetached()
{
    char command = 'D';
    return SendCommand(command);
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

    // printf("Connecting to server.\n");

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
    {
        FATAL("Could not create socket\n");
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

    // printf("\tConnected\n");

    return 1;
}

int FuzzerCommunicator::DisconnectFromServer()
{
    if (closesocket(sock) == SOCKET_ERROR)
    {
        FATAL("Could not close socket\n");
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

    // printf("Initialized FuzzerCommuniator --> WSAStartup\n");
}

FuzzerCommunicator::~FuzzerCommunicator()
{
    WSACleanup();
}