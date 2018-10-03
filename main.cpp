#include "TCP.h"
#include <iostream>
#include <limits>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>


using namespace std;

bool clientLogic(TCP_Client* Client);
void loginLDAP(TCP_Client* Client,string &logedInUser);
void readMail(TCP_Client* Client,const string &user);
void delMail(TCP_Client* Client,const string &user);
void sendMail(TCP_Client* Client,const string &user);
void listMail(TCP_Client* Client,const string &user);




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
    string logedInUser="ERROR_USER";
    cout << endl << "-----------------------------------------------------------------------------------------------" << endl;
    cout << "What do u want to do?" << endl;
    cout << "(0) QUIT | (1) SEND new Mail | (2) LIST all Mails | (3) READ Mail | (4) DELETE Mail | (5) LOGIN" << endl;
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
            sendMail(Client,logedInUser);
            break;
        case 2: //LIST
            listMail(Client,logedInUser);
            break;
        case 3: //READ
            readMail(Client,logedInUser);
            break;
        case 4: //DELETE
            delMail(Client,logedInUser);
            break;
        case 5: // LOGIN
            loginLDAP(Client,logedInUser);
            break;
        default:
            cout << "False Input!" << endl;
            break;
    }

    return true;
}

void loginLDAP(TCP_Client* Client,string &logedInUser) {
    string username="";
    cout << "Enter FHTW username (max 8 letters):";
    do{
        cin >> username;
    }while(username.length()>8);

    cin.clear();cin.ignore();

    struct termios oflags, nflags;
    char password[64];

    /* disabling echo */
    tcgetattr(fileno(stdin), &oflags);
    nflags = oflags;
    nflags.c_lflag &= ~ECHO;
    nflags.c_lflag |= ECHONL;

    if (tcsetattr(fileno(stdin), TCSANOW, &nflags) != 0) {
        perror("tcsetattr");
        //return EXIT_FAILURE;
    }

    printf("Enter password: ");
    fgets(password, sizeof(password), stdin);
    password[strlen(password) - 1] = 0;

    /* restore terminal */
    if (tcsetattr(fileno(stdin), TCSANOW, &oflags) != 0) {
        perror("tcsetattr");
        //return EXIT_FAILURE;
    }

    string message = "";
    message += "LOGIN\n";
    message += username + "\n";
    message += password; message += "\n";

    Client->sendData(message);
    char result[10]; int size = 10;
    memset(result,0,sizeof(char)*size);
    Client->recvData(size,result);

    if(strcmp (result,"OK\n") == 0)
    {
        cout << "Server OK" << endl;
        logedInUser = username;
    }
    else if(strcmp (result,"ERR\n") == 0)
    {
        cout << "Server ERR" << endl;
        logedInUser="ERROR_USER";
    }
    else
    {
        cout << "Other error" << endl;
        logedInUser="ERROR_USER";
    }
}

void readMail(TCP_Client* Client,const string &user){

}
void delMail(TCP_Client* Client,const string &user){

}
void sendMail(TCP_Client* Client,const string &user){
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
        subject = "";
        cin >> subject;
    }while(subject.length()>80||subject.length()==0);
    cout << "Nachricht eingeben:" << endl;
    cin >> msg;

    //LOGINNAME FEHLT!!!
    mail = "SEND\n";
    mail += user + '\n';
    mail += to + '\n';
    mail += subject + '\n';
    mail += msg + "\n.\n";

    Client->sendData(mail);
    char result[10]; int size = 10;
    memset(result,0,sizeof(char)*size);
    Client->recvData(size,result);

    if(strcmp (result,"OK\n") == 0)
    {
        cout << "Server OK" << endl;
    }
    else if(strcmp (result,"ERR\n") == 0)
    {
        cout << "Server ERROR" << endl;
    }
    else
    {
        cout << "Other error" << endl;
    }
}
void listMail(TCP_Client* Client,const string &user){

}


