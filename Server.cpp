#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <map>
#include <thread>

using namespace std;

map <string, SOCKET> clients{};

#pragma comment(lib, "Ws2_32.lib")

void ClientThread(SOCKET sock) {

    char buf[4096];
    string mess, nome, mitt;
    SOCKET mitt_sock;

    while (true) {
        int risp = recv(sock, buf, 4096, 0);

        if (risp > 0) {
            mess = string(buf);
            cout << "mess -> " << mess << endl;
            if (mess.substr(0, 4) == "new?") {
                mess = mess.substr(4, mess.length());
                if (clients.find(mess) != clients.end()) {
                    mess = "%nonew%";
                    send(sock, mess.c_str(), 4096, 0);
                }
                else {
                    nome = mess;
                    clients[nome] = sock;
                    mess = "%oknew%";
                    send(sock, mess.c_str(), 4096, 0);
                }
                
            }

        }
        if (mess.substr(0, 5) == "mess?") {
            for (const auto& p : clients) {
                if (p.first != nome) {
                    SOCKET mitt_sock = p.second;
                    send(mitt_sock, mess.c_str(), 4096, 0);
                }
            }
        }
    
           
    }
    
}


int main()
{
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    sockaddr_in server, client;

    

    SOCKET s = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8000);

    /*
    unsigned long arg = 1;
    if (ioctlsocket(s, FIONBIO, &arg) == SOCKET_ERROR)
    {
        WSACleanup();
        closesocket(s);
        return -1;
    }
    */
    
    

    // bind
    bind(s, (sockaddr*)&server, sizeof(server));
    //listen
    listen(s, 3);
    //Accept
    sockaddr_in from; // for the client
    int fromlen = sizeof(from);

    while (true) {
        SOCKET clientS = accept(s, (sockaddr*)&from, &fromlen);
        thread t(ClientThread, clientS);
        t.detach();
        cout << "Nuova connessione" << endl;
    }

    

    system("pause");
    return 0;
}