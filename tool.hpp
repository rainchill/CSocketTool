#ifndef TOOL_H
#define TOOL_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

/**
 * @description: 发送指定的字节数
 * @param {int} fd 通信的文件描述符(套接字)
 * @param {char} *msg 待发送的原始数据
 * @param {int} size 待发送的原始数据的总字节数
 * @return {int} 函数调用成功返回发送的字节数, 发送失败返回-1
 */
int writen(int fd, const char *msg, int size)
{
    const char *buf = msg;
    int count = size;
    while (count > 0)
    {
        int len = send(fd, buf, count, 0);
        if (len == -1)
        {
            close(fd);
            return -1;
        }
        else if (len == 0)
        {
            continue;
        }
        buf += len;
        count -= len;
    }
    return size;
}

/**
 * @description: 接收指定的字节数
 * @param {int} fd 通信的文件描述符(套接字)
 * @param {char} *buf 存储待接收数据的内存的起始地址
 * @param {int} size 指定要接收的字节数
 * @return {int} 函数调用成功返回发送的字节数, 发送失败返回-1
 */
int readn(int fd, char *buf, int size)
{
    char *pt = buf;
    int count = size;
    while (count > 0)
    {
        int len = recv(fd, pt, count, 0);
        if (len == -1)
        {
            return -1;
        }
        else if (len == 0)
        {
            return size - count;
        }
        pt += len;
        count -= len;
    }
    return size;
}

/**
 * @description: 客户端向指定ip和端口的服务器发送并建立连接
 * @param {char} ip 服务器的ipv4地址
 * @param {int} port 服务器的端口
 * @return {int} 返回的socket, 失败返回-1
 */
int client_start(const char *ip, int port)
{
    char err_fun[20] = "[client_start] ";
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client == -1)
    {
        std::cout << err_fun << "Error: socket" << std::endl;
        return -1;
    }
    // connect
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);
    if (connect(client, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cout << err_fun << "Error: connect" << std::endl;
        return -1;
    }
    return client;
}

/**
 * @description: 服务器启动socket，地址默认绑定全部网卡的ip socket->bind->listen
 * @param {int} port 服务器启动的端口
 * @param {int} queue_num listen监听队列的大小, 一般5-20
 * @return {int} 服务器socket, 失败返回-1
 */
int server_start(int port, int queue_num)
{
    char err_fun[20] = "[server_start] ";
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd == -1)
    {
        std::cout << err_fun << "Error: socket" << std::endl;
        return -1;
    }
    // bind
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8000);
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(listenfd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
    {
        std::cout << err_fun << "Error: bind" << std::endl;
        return -1;
    }
    // listen
    if (listen(listenfd, queue_num) == -1)
    {
        std::cout << err_fun << "Error: listen" << std::endl;
        return -1;
    }
}

#endif