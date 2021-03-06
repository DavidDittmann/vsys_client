//
// Created by david on 23.09.18.
//

#include "TCP.h"

using namespace std;

TCP_Client::TCP_Client(int port, string addr) {
    //set port/ip to server
    this->host_port = port;
    this->host_addr = addr;
    this->sock_fd = -1;
}

TCP_Client::~TCP_Client() {
    if(this->sock_fd >= 0)
        close(this->sock_fd);
}

void TCP_Client::openSocket() {
    if(this->sock_fd < 0)
    {
        //open socket ipv4/tcp
        this->sock_fd = socket(AF_INET,SOCK_STREAM,0);

        if(this->sock_fd < 0)
        {
            stringstream buf;
            buf << "Socket could not be created!";
            throw runtime_error(buf.str().c_str());
        }
    }
    else
    {
        stringstream buf;
        buf << "Socket already created!";
        throw runtime_error(buf.str().c_str());
    }
}

bool TCP_Client::connectToHost() {

    //set sock_addr struct, ipv4, port, addr
    this->host.sin_family = AF_INET;
    this->host.sin_port = htons(this->host_port);
    this->host.sin_addr.s_addr = inet_addr(this->host_addr.c_str());

    //connect local socket to addr
    int ret = connect(this->sock_fd, (struct sockaddr*) &this->host, sizeof(this->host));
    if(ret < 0)
    {
        stringstream buf;
        buf << "Connection could not be established: " << strerror(errno);
        throw runtime_error(buf.str().c_str());
    }

    return true;
}

void TCP_Client::sendData(string msg) {
    if(this->sock_fd<0)
    {
        stringstream buf;
        buf << "Cannot send - socket not initialised";
        throw runtime_error(buf.str().c_str());
    }
    else
    {
        //send msg, returns bytes written
        int ret = send(this->sock_fd, msg.c_str(), msg.length(),0);
        if(ret < 0)
        {
            stringstream buf;
            buf << "Sending msg failed: " << strerror(errno);
            throw runtime_error(buf.str().c_str());
        }
    }
}

void TCP_Client::recvData(int size,char* buffer) {
    memset(buffer,0,sizeof(char)*size);

    //Wie lange auf recv warten?
    fd_set read_fd;
    FD_ZERO(&read_fd);//clear set
    FD_SET(this->sock_fd,&read_fd);//add socket fd to set
    timeval timeout;
    timeout.tv_sec = 30;
    timeout.tv_usec = 0;

    //monitor sock_fd and block till data is received, or timeout is thrown
    int ret = select(this->sock_fd+1,&read_fd,NULL,NULL,&timeout);
    if(ret > 0)
    {
        if(FD_ISSET(this->sock_fd,&read_fd))//check if socket fd is part of the set (should be used after select)
        {
            //receive data to buf, store bytes written in ret
            ret = recv(this->sock_fd,buffer,size-1,0);
            buffer[size] = '\0';
        }
    }
    else if(ret == 0)   //Socket closed
    {
        stringstream buf;
        buf << "Could not recieve msg - socket closed";
        throw runtime_error(buf.str().c_str());
    }
    else if(ret < 0)    //Error at recv
    {
        stringstream buf;
        buf << "Recieving msg failed: " << strerror(errno);
        throw runtime_error(buf.str().c_str());
    }
}
