/*******************************************************************
 * Author: Feyico
 * Date: 1970-01-01 08:00:00
 * LastEditors: Feyico
 * LastEditTime: 2021-01-05 22:20:27
 * Description: http连接头文件
 * FilePath: /webserver/include/HttpConnect.h
 *******************************************************************/
#ifndef _HTTPCONNECT_H_
#define _HTTPCONNECT_H_

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

class HttpConnect
{
public:
    //读缓冲区大小（所有对象一致）
    static const int BUFFER_SIZE = 4096;

    //主机的两种可能状态
    enum CHECK_STATUS{
        CHECK_STATUS_REQUESTLINE = 0,   //当前正在分析请求行
        CHECK_STATUS_HEADER             //当前正在分析请求头部
    };

    //从状态机的三种可能状态
    enum LINE_STATUS{
        LINE_OK = 0,    //读取到一个完整的行
        LINE_BAD,       //行出错
        LINE_OPEN       //行数据尚且不完整
    };

    //服务器处理HTTP请求的结果
    enum HTTP_CODE{
        NO_REQUEST = 0,     //表示请求不完整，需要继续读取客户数据
        GET_REQUEST,        //表示获得了一个完整的客户请求
        BAD_REQUEST,        //表示客户请求有语法错误
        FORBIDDEN_REQUEST,  //表示客户对资源没有足够的访问权限
        INTERNAL_ERROR,     //表示服务内部错误
        CLOSED_CONNECTION   //表示客户端已经关闭连接了
    };
public:
    HttpConnect();
    ~HttpConnect(){};

    //初始化连接,外部调用初始化套接字地址
    void InitHttpConnect(const int& sockfd, const sockaddr_in& addr);
    //关闭连接
    void CloseHttpConnect(const int& sockfd){close(sockfd);};
    //分析HTTP请求的入口函数
    HTTP_CODE ParseContent(char* buffer, int& data_checked, CHECK_STATUS& check_state,
       int& data_readed, int& line_start);
    HTTP_CODE ParseContent(char* buffer);

private:
    int m_readed_index;     //当前读取了多少字节的客户数据
    int m_checked_index;    //当前已经分析了多少字节的客户数据
    int m_line_start;      //行在buffer中的起始位置
    CHECK_STATUS m_check_status;

    int m_sockfd;
    sockaddr_in m_addr;

private:
    //从状态机，用于解析出一行内容
    LINE_STATUS ParseLine(char* buffer, const int& data_readed, int& data_checked);
    LINE_STATUS ParseLine(char* buffer);
    //分析请求行
    HTTP_CODE ParseRequestLine(char* tmp, CHECK_STATUS& check_status);
    HTTP_CODE ParseRequestLine(char* buffer);
    //分析头部字段
    HTTP_CODE ParseHeaders(char* buffer);
};


#endif