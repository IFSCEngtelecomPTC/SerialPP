/* 
 * File:   Serial.cpp
 * Author: msobral
 * 
 * Created on 9 de Março de 2016, 13:15
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/select.h>
#include <iostream>
#include <fstream>

using std::cout;
using std::endl;

#include "Serial.h"

#define MAX_SERIAL_BYTES 64

Serial::Serial() {
}

Serial::Serial(const Serial& orig) {
}

Serial::~Serial() {
    //close(tty_fd);
    //char * path = ttyname(0);
    //if (not path) throw SerialException("invalid tty");
    //init(path, B115200);
}

Serial::Serial(const string& path) {
    init(path, B9600);
}

Serial::Serial(const string& path, int rate) {
    init(path, rate);
}

void Serial::init(const string& path, int rate) {
  struct termios tio;

  tty_fd=open(path.c_str(), O_RDWR|O_NOCTTY);
  if (tty_fd < 0) throw -10;
  
  tcgetattr(tty_fd, &tio);
  
  tio.c_iflag = 0;
  tio.c_oflag = 0;  
  //tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
  tio.c_cflag = 0x8bd;
  tio.c_lflag = 0;
  
  tio.c_cc[0] = 3;
  tio.c_cc[1] = 0x1c;
  tio.c_cc[2] = 0x7f;
  tio.c_cc[3] = 0x15;
  tio.c_cc[4] = 4;
  tio.c_cc[5] = 0;
  tio.c_cc[6] = 0;
  tio.c_cc[7] = 0;
  tio.c_cc[8] = 0x11;
  tio.c_cc[9] = 0x13;
  tio.c_cc[10] = 0x1a;
  tio.c_cc[11] = 0;
  tio.c_cc[12] = 0x12;
  tio.c_cc[13] = 0xf;
  tio.c_cc[14] = 0x17;
  tio.c_cc[15] = 016;
  for (int i=16; i < 32; i++) tio.c_cc[i] = 0;
  
  //cfmakeraw(&tio);
  cfsetospeed(&tio,rate);            // 115200 baud
  cfsetispeed(&tio,rate);            // 115200 baud
       
  tcsetattr(tty_fd,TCSANOW,&tio);  
  
  long flag;
  ioctl(tty_fd, F_GETFL, &flag);
  flag |= O_NONBLOCK;
  ioctl(tty_fd, F_SETFL, &flag);
  
  timeout = 0;
}

bool Serial::cca() {
    // se a serial estiver com bytes no buffer, então está
    // recebendo algo ... 
    fd_set r;
    struct timeval tv = {0,0};
    
    FD_ZERO(&r);
    FD_SET(tty_fd, &r);
    
    // faz um poll na serial
    int n = select (tty_fd+1, &r, NULL, NULL, &tv);
    return (n < 1); // sem bytes para ler
}

int Serial::write(const char* buffer, uint16_t len) {
    //if (not cca()) return 0;
    
    int pos = 0;
    while (pos < len) {        
        int bytes = len - pos;
        if (bytes > MAX_SERIAL_BYTES) bytes = MAX_SERIAL_BYTES;
        int sent = ::write(tty_fd, buffer+pos, bytes);    
        pos += sent;
        ::tcdrain(tty_fd);
    }
    return pos;
}

void Serial::set_timeout(int tout) {
    timeout = tout;
    
    if (timeout) {
        t.tv_sec = timeout / 1000;
        t.tv_usec = 1000*(timeout % 1000);
    }
    //cout << "set_timeout: "<< tout <<", sec=" << t.tv_sec << ", usec=" << t.tv_usec << endl;
    
}

int Serial::read(char* buffer, uint16_t len, bool block) {
    if (block) {
        fd_set r;
        
        FD_ZERO(&r);
        FD_SET(tty_fd, &r);
        timeval * ptr = NULL;
        if (timeout) {
            ptr = &t;
        }
        //cout << "t: sec=" << t.tv_sec << ", usec=" << t.tv_usec << endl;
        int n = select (tty_fd+1, &r, NULL, NULL, ptr);
        if (n <= 0) return 0;
    }
    int n = ::read(tty_fd, buffer, len);
    
    return n;
}

char Serial::read_byte() {
    char c;
    
    read(&c, 1, true);
    return c;
}

int Serial::write(const vector<char> &buffer) {
    return write(buffer.data(), buffer.size());
}

vector<char> Serial::read(uint16_t len, bool block) {
    vector<char> buffer(len, 0);

    auto n = read(buffer.data(), len, block);
    buffer.resize(n);

    return buffer;
}
