/*******************************************************************
 * Author: Feyico
 * Date: 1970-01-01 08:00:00
 * LastEditors: Feyico
 * LastEditTime: 2021-01-05 23:04:09
 * Description: 
 * FilePath: /webserver/include/WebServer.h
 *******************************************************************/
#ifndef _WEBSERVER_H_
#define _WEBSERVER_H_

#include "HttpConnect.h"

const int MAX_FD = 65536;   //最大文件描述符

class WebServer
{
public:
    WebServer();
    ~WebServer();

    void Init();
    void Run();
    void EventListen();

public:
    

private:
    HttpConnect *http_connect;
    int m_linten_fd;
    int m_port;
};


#endif
