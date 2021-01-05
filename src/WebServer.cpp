/*******************************************************************
 * Author: Feyico
 * Date: 1970-01-01 08:00:00
 * LastEditors: Feyico
 * LastEditTime: 2021-01-05 23:08:43
 * Description: 
 * FilePath: /webserver/src/WebServer.cpp
 *******************************************************************/
#include "WebServer.h"
//RAII原则：资源在对象构造时申请，在析构时释放
WebServer::WebServer()
{
    http_connect = new HttpConnect[MAX_FD];
}

WebServer::~WebServer()
{
    delete[] http_connect;
}

void WebServer::EventListen()
{
    //网络编程的一些基础
    m_linten_fd = socket(PF_INET, SOCK_STREAM, 0);
    
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;               //IP地址家族
    address.sin_addr.s_addr = htonl(INADDR_ANY);//填写IP
    address.sin_port = htons(m_port);           //填写端口

    int ret = bind(m_linten_fd, (struct sockaddr *)&address, sizeof(address));
    assert(ret != -1);
    ret = listen(m_linten_fd, 5);
    assert(ret != -1);
}