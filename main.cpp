#include "TCP.h"
#include <iostream>
#include <limits>

using namespace std;

bool clientLogic(TCP_Client* Client);
void readMail(TCP_Client* Client);
void delMail(TCP_Client* Client);
void sendMail(TCP_Client* Client);
void listMail(TCP_Client* Client);

int main(int argc, char* argv[]) {

    if(argc != 2)
        cout << "Usage: ./Client <PortNr.> <Address>" << endl;

    int port = stoi(argv[1]);
    string addr = argv[2];

    TCP_Client* Client = new TCP_Client(port,addr);

    __try{
        cout << "Started new session..." << endl;
        Client->openSocket();
        if(Client->connectToHost())
        {
            cout << "Connected to host: " << addr << ":" << port << endl;
        }
        bool ret = true;
        while(ret)
        {
            ret = clientLogic(Client);
        }
    }
    catch(runtime_error &e){
        cout << e.what() << endl;
        cout << "Closing Client" << endl;
    }

    delete(Client);

    return 0;
}

bool clientLogic(TCP_Client* Client)
{
    //CLIENT - LOGIK
    int option;
    cout << endl << "-----------------------------------------------------------------------------------------------" << endl;
    cout << "What do u want to do?" << endl;
    cout << "(0) QUIT | (1) SEND new Mail | (2) LIST all Mails | (3) READ Mail | (4) DELETE Mail" << endl;
    do {
        if(cin.fail())
        {
            cout << "U must enter a number!" <<endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin >> option;
    }while(cin.fail());
    switch(option)
    {
        case 0:
            cout << "Closing Client..." << endl;
            return false;
        case 1: //SEND
            sendMail(Client);
            break;
        case 2: //LIST
            listMail(Client);
            break;
        case 3: //READ
            readMail(Client);
            break;
        case 4: //DELETE
            delMail(Client);
            break;
        default:
            break;
    }

    return true;
}

void readMail(TCP_Client* Client){

}
void delMail(TCP_Client* Client){

}
void sendMail(TCP_Client* Client){
    string to= "a";
    string subject = "a";
    string msg = "a";
    string mail="";

    cout << "Empfänger angeben: (max. 8 Zeichen)" << endl;
    do{
        if(to.length()>8 || to.length()==0)
            cout << "Empfänger muss mehr als 0, aber max. 8 Zeichen lang sein" << endl;
        to = "";
        cin >> to;
    }while(to.length()>8||to.length()==0);

    cout << "Betreff angeben: (max. 80 Zeichen)" << endl;
    do{
        if(subject.length()>80 || subject.length()==0)
            cout << "Betreff muss mehr als 0, aber max. 80 Zeichen lang sein" << endl;
        to = "";
        cin >> subject;
    }while(subject.length()>80||subject.length()==0);
    cout << "Nachricht eingeben:" << endl;
    cin >> msg;

    //LOGINNAME FEHLT!!!
    mail = "SEND\n";
    mail += to + '\n';
    mail += subject + '\n';
    mail += msg + "\n.\n";

    Client->sendData(mail);
    char result[10]; int size = 10;
    memset(result,0,sizeof(char)*size);
    Client->recvData(size,result);

    if(result == "OK\n")
    {

    }
    else if(result == "ERR\n")
    {

    }
    else
    {

    }
}
void listMail(TCP_Client* Client){

}