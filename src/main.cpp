/*******************************************************************
 * Author: Feyico
 * Date: 1970-01-01 08:00:00
 * LastEditors: Feyico
 * LastEditTime: 2021-01-05 22:01:32
 * Description: webserver
 * FilePath: /webserver/src/main.cpp
 *******************************************************************/
#include "HttpConnect.h"

using namespace std;

int main(int argc, char *argv[])
{
    //初始化数据库连接池
    //初始化
    //1、利用有限状态机实现一个http连接
	if (argc < 2)
    {
        printf("usage: %s ip_address port_number \n", basename(argv[0]));
        return 1;
    }

    const char *ip = argv[1];
    int port = atoi(argv[2]);

	HttpConnect httpConn;

    return 0;
}