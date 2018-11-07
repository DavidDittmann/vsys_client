#include "TCP.h"
#include <iostream>
#include <limits>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>

/*
OK das ist ein 2. Test
*/

using namespace std;

bool clientLogic(TCP_Client* Client, string &logenInUser,int &failedLogins,bool &isBanned);
void loginLDAP(TCP_Client* Client,string &logedInUser,int &failedLogins,bool &isBanned);
void readMail(TCP_Client* Client,const string &user,bool &isBanned);
void delMail(TCP_Client* Client,const string &user,bool &isBanned);
void sendMail(TCP_Client* Client,const string &user,bool &isBanned);
void listMail(TCP_Client* Client,const string &user);
int cinNumber(void);



int main(int argc, char* argv[]) {

    bool cancel = false;
    if(argc != 3)
    {
        cout << "Usage: ./Client <Address> <PortNr.>" << endl;
        cancel = true;
    }

    string addr = argv[2];
    long port = -1;
    port = strtol (argv[1],NULL,10);
    if(port > 65535 || port < 1024)
    {
        cout << "Usage: ./Client <Address> <PortNr.>" << endl;
        cout << "Port musst be greater than 1024 and smaller than 65535" << endl;
        cancel = true;
    }
    int p = (int) port;

    if(!cancel)
    {
        TCP_Client* Client = new TCP_Client(p,addr);

        __try{
            cout << "Started new session..." << endl;
            Client->openSocket();
            if(Client->connectToHost())
            {
                cout << "Connected to host: " << addr << ":" << port << endl;
            }
            bool ret = true;
            string user = "ERROR_USER";
            int failedLogins = 0;
            bool isBanned = false;
            while(ret && !isBanned)
            {
                ret = clientLogic(Client,user,failedLogins,isBanned);
            }
        }
        catch(runtime_error &e){
            cout << e.what() << endl;
            cout << "Closing Client" << endl;
        }

        delete(Client);
    }

    return 0;
}

bool clientLogic(TCP_Client* Client,string &logedInUser,int &failedLogins,bool &isBanned)
{
    int option=0;
    bool ret = true;
    //CLIENT - LOGIK
    if(!isBanned)
    {
        cout << endl << "--------------------------------------------------------------" << endl;
        cout << "What do u want to do?" << endl;
        cout << " (0) QUIT \n (1) SEND new Mail \n (2) LIST all Mails \n (3) READ Mail \n (4) DELETE Mail \n (5) LOGIN\n:" << endl;
        option = cinNumber();
    }

    switch(option)
    {
        case 0: {
            cout << "Closing Client..." << endl;
            string msg = "QUIT\n";
            Client->sendData(msg);
            ret = false;
            break;
        }
        case 1: //SEND
        {
            sendMail(Client, logedInUser,isBanned);
            break;
        }
        case 2: //LIST
        {
            listMail(Client, logedInUser);
            break;
        }
        case 3: //READ
        {
            readMail(Client, logedInUser,isBanned);
            break;
        }
        case 4: //DELETE
        {
            delMail(Client, logedInUser,isBanned);
            break;
        }
        case 5: // LOGIN
        {
            loginLDAP(Client, logedInUser, failedLogins,isBanned);
            break;
        }
        default:
            cout << "False Input!" << endl;
            break;
    }

    return ret;
}

void loginLDAP(TCP_Client* Client,string &logedInUser, int &failedLogins,bool &isBanned) {
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
        failedLogins++;
        if(failedLogins>=3)
        {
            cout << "3x failed login - you have been banned from the server!" <<endl;
            isBanned=true;
        }
        logedInUser="ERROR_USER";
    }
    else if(strcmp (result,"BAN\n") == 0)
    {
        cout << "You got IP-Banned!" << endl;
        isBanned=true;
    }
    else if(result!=NULL)
    {
        cout << "Other failure" << endl;
        logedInUser="ERROR_USER";
    }

}

void readMail(TCP_Client* Client,const string &user,bool &isBanned){
    if(user=="ERROR_USER"){
        cout << "Sie sind nicht eingeloggt!" << endl;
    }
    else
    {
        int msgNumber = -1;
        cout << "Welche Nachricht wollen Sie lesen?" << endl;
        msgNumber = cinNumber();

        string msg = "READ\n"+user+"\n"+to_string(msgNumber)+"\n";
        Client->sendData(msg);
        char result[4098]; int size = 4098;
        memset(result,0,sizeof(char)*size);
        Client->recvData(size,result);
        string response(result);

        if(response.find("ERR\n")!=string::npos){
            cout << "Server konnte Nachricht nicht finden" << endl;
        }
        else if(response.find("BAN\n")!=string::npos)
        {
            cout << "You got IP-Banned!" << endl;
            isBanned=true;
        }
        else if(response.find("OK\n")!=string::npos)
        {
            string msg = response.substr(response.find("OK\n")+3); //TEST RELEVANT
            cout << "Nachricht:\n" << msg << endl;
        }
        else
            cout << "Other Error" << endl;
    }
}
void delMail(TCP_Client* Client,const string &user,bool &isBanned){
    if(user=="ERROR_USER"){
        cout << "Sie sind nicht eingeloggt!" << endl;
    }
    else
    {
        int msgNumber = -1;
        cout << "Welche Nachricht wollen Sie löschen?" << endl;
        msgNumber = cinNumber();

        string msg = "DEL\n"+user+"\n"+to_string(msgNumber)+"\n";
        Client->sendData(msg);
        char result[4098]; int size = 4098;
        memset(result,0,sizeof(char)*size);
        Client->recvData(size,result);
        string response(result);

        if(response.find("ERR\n")!=string::npos){
            cout << "Server konnte Nachricht nicht finden" << endl;
        }
        else if(response.find("BAN\n")!=string::npos)
        {
            cout << "You got IP-Banned!" << endl;
            isBanned=true;
        }
        else if(response.find("OK\n")!=string::npos)
        {
            cout << "Nachricht wurde gelöscht" << endl;
        }
        else
            cout << "Other Error" << endl;
    }
}
void sendMail(TCP_Client* Client,const string &user,bool &isBanned){
    if(user=="ERROR_USER"){
        cout << "Sie sind nicht eingeloggt!" << endl;
    }
    else {
        string to = "a";
        string subject = "a";
        string msg = "";
        string mail = "", line="";


        cout << "Empfänger angeben: (max. 8 Zeichen)" << endl;
        do {
            if (to.length() > 8 || to.length() == 0)
                cout << "Empfänger muss mehr als 0, aber max. 8 Zeichen lang sein" << endl;
            to = "";
            cin >> to;
        } while (to.length() > 8 || to.length() == 0);


        cin.ignore();
        cout << "Betreff angeben: (max. 80 Zeichen)" << endl;
        do {
            if (subject.length() > 80 || subject.length() == 0)
                cout << "Betreff muss mehr als 0, aber max. 80 Zeichen lang sein" << endl;
            subject = "";
            getline(cin, subject);
        } while (subject.length() > 80 || subject.length() == 0);

        //cin.ignore();
        cout << "Nachricht eingeben:" << endl;
        bool tmp = false;
        while (getline(cin, line)) {
            if (line.length() == 0)
                break;

            msg +=line +"\n";
        }

        //LOGINNAME FEHLT!!!
        mail = "SEND\n";
        mail += user + '\n';
        mail += to + '\n';
        mail += subject + '\n';
        mail += msg + ".\n";

        cout << mail;

        Client->sendData(mail);
        char result[10];
        int size = 10;
        memset(result, 0, sizeof(char) * size);
        Client->recvData(size, result);

        if (strcmp(result, "OK\n") == 0) {
            cout << "Server OK" << endl;
        } else if (strcmp(result, "ERR\n") == 0) {
            cout << "Server ERROR" << endl;
        }else if (strcmp(result, "BAN\n") == 0) {
            cout << "You got IP-Banned!" << endl;
            isBanned=true;
        } else {
            cout << "Other error" << endl;
        }
    }
}
void listMail(TCP_Client* Client,const string &user){
    if(user=="ERROR_USER"){
        cout << "Sie sind nicht eingeloggt!" << endl;
    }
    else
    {
        string msg = "LIST\n"+user+"\n";
        Client->sendData(msg);

        char result[4098]; int size = 4098;
        memset(result,0,sizeof(char)*size);
        Client->recvData(size,result);
        string response(result);

        cout << "Nachrichten:" << endl;
        int msgCount = 1, pos=0, i=0;
        //vector <string> subjects;
        while((pos = response.find("\n")) != string::npos && i<= msgCount && msgCount != 0)
        {
            if(i == 0)
            {
                msgCount = stoi(response.substr(0,pos));
            }
            else
            {
                cout << i-1 << ": " << response.substr(0,pos) << endl;
                //subjects.push_back(response.substr(0,pos));
            }
            response.erase(0,pos+1);
            i++;
        }
    }
}

int cinNumber()
{
    int option;
    do {
        if(cin.fail())
        {
            cout << "Es muss eine pos. Zahl eingegeben werden!" <<endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        cin >> option;
    }while(cin.fail() || option < 0);
    return option;
}

