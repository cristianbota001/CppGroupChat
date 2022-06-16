#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <string>
#include <stdio.h>
#include <thread>
#include <semaphore>
#include <mutex>
#include <condition_variable>

using namespace std;

bool ex_mitt = false, engine = true;

SOCKET client;

mutex mu;
condition_variable sig;

string buf_mess[1000], nome;
int buf_mess_index = 0, text_area_y = 15;

#pragma comment(lib, "ws2_32.lib")

void gotoxy(int x, int y)
{
    COORD coordinates;
    coordinates.X = x;
    coordinates.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coordinates);
}

void RecvServer(SOCKET sock);
void Connessione();
void Engine();
void Stampa_Campo();
bool First_Message();
void MittMessage();
void TextArea(string message, bool save_mess);
void DeleteTextArea(bool se);
string TextInput();



int main()
{

    Connessione();
    Stampa_Campo();
    Engine();

    system("pause");
    return 0;
}

void RecvServer(SOCKET sock) {
    char buf[4096];
    string mess;

    while (true) {
        int risp = recv(sock, buf, 4096, 0);
        if (risp > 0) {
            mess = string(buf);
            //cout << "mess: " << mess << endl;
            if (mess == "%nonew%") {
                ex_mitt = true;
            }
            if (mess.substr(0, 5) == "mess?") {

                mess = mess.substr(5, mess.length());
                string nn = mess.substr(0, mess.find("?"));
                mess = mess.substr(mess.find("?") + 1, mess.length());
                mess = nn + ">> " + mess;

                TextArea(mess, true);
                //cout << "Messaggio arrivato da " << mitt << " : " << mess.substr(5, mess.length()) << endl;
            }
            if (mess == "delmitt?") {
                engine = false;
            }
            
            sig.notify_all();
        }
    }
}

void Connessione() {
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);
    sockaddr_in dest;
    dest.sin_family = AF_INET;
    dest.sin_port = htons(8000);
    dest.sin_addr.s_addr = inet_addr("127.0.0.1");
    client = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    connect(client, (sockaddr*)&dest, sizeof(dest));
}

void Stampa_Campo() {
    Sleep(1000);
    gotoxy(50, 13);
    cout << "* * * * * * * * * * * * * * * * * *";
    for (int y = 14; y < 50; y++) {
        if (y != 48) {
            gotoxy(50, y);
            cout << "*                                 *";
        }
        else {
            gotoxy(50, y);
            cout << "* * * * * * * * * * * * * * * * * *";
        }

    }
    gotoxy(50, 50);
    cout << "* * * * * * * * * * * * * * * * * *";
}

void Engine() {

    thread t(RecvServer, client);

    while (true) {
        if (First_Message()) {
            DeleteTextArea(true);
            while (engine) {
                MittMessage();
            }
        }
        
    }
}

bool First_Message() {
    unique_lock<std::mutex> lock(mu);
    TextArea("Inserisci il tuo nome", false);
    nome = TextInput();
    string mess = "new?" + nome;
    send(client, mess.c_str(), 4096, 0);
    sig.wait(lock);
    
    if (ex_mitt == false) {
        return true;
    }
    else {
        ex_mitt = false;
        return false;
    }
}

void MittMessage() {
    string mess =  TextInput();
    string moss = "mess?" + nome + "?" + mess;
    send(client, moss.c_str(), 4096, 0);
    TextArea("Tu> " + mess, true);
}

void TextArea(string message, bool save_mess) {
    buf_mess[buf_mess_index] = message;
    buf_mess_index++;

    for (int i = 0; i < buf_mess[buf_mess_index - 1].length(); i += 30) {
        
        DeleteTextArea(false);
        gotoxy(52, text_area_y);
        cout << buf_mess[buf_mess_index - 1].substr(i, 30);
        text_area_y += 2;
        
    }

    if (!save_mess)
        buf_mess_index--;

    gotoxy(52, 49);
}

void DeleteTextArea(bool se) {

    if (text_area_y > 45 || se == true) {
        text_area_y = 15;
        gotoxy(50, 13);
        cout << "* * * * * * * * * * * * * * * * * *";
        for (int y = 14; y < 48; y++) {
            gotoxy(50, y);
            cout << "*                                 *";
        }
        gotoxy(50, 48);
        cout << "* * * * * * * * * * * * * * * * * *";
    }

}

string TextInput() {
    char mess[4096];

    gotoxy(52, 49);
    cin.getline(mess, 4096);


    gotoxy(50, 49);
    cout << "*                                 *";

    return (string(mess));
}
