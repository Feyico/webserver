/*******************************************************************
 * Author: Feyico
 * Date: 1970-01-01 08:00:00
 * LastEditors: Feyico
 * LastEditTime: 2021-01-05 22:21:16
 * Description: 利用有限状态机实现http连接
 * FilePath: /webserver/src/HttpConnect.cpp
 *******************************************************************/
#include "HttpConnect.h"

/* 为了监护问题，我们没有给客户端发送一个完整的HTTP应答报文，
 * 而只是根据服务器的处理结果发送如下成功或失败信息 */
static const char* szret[] = {"I get a correct result\n",
    "Somethin wrong\n"};

HttpConnect::HttpConnect():m_readed_index(0), m_checked_index(0), m_line_start(0), m_check_status(CHECK_STATUS_REQUESTLINE)
{
}

void HttpConnect::InitHttpConnect(const int& sockfd, const sockaddr_in& addr){
    this->m_addr = addr;
    this->m_sockfd = sockfd;
    return ;
}

HttpConnect::HTTP_CODE HttpConnect::ParseContent(char* buffer){
    LINE_STATUS line_status = LINE_OK;  //记录当前读取行的状态
    HTTP_CODE ret_code = NO_REQUEST;    //记录HTTP请求的处理结果

    //主状态机，用于从buffer中取出所有完整的行
    while (LINE_OK == (line_status = ParseLine(buffer))){
        char* buffer_tmp = buffer + m_line_start;  //m_line_start是行在buffer中的起始位置
        m_line_start = m_checked_index;      //记录下一行的起始位置

        //m_check_status记录主状态机当前的状态
        switch (m_check_status){
        case CHECK_STATUS_REQUESTLINE:  //第一个状态，分析请求行
            {
                ret_code = ParseRequestLine(buffer_tmp);
                if (BAD_REQUEST == ret_code)
                    return ret_code;
                
                break;
            }
        case CHECK_STATUS_HEADER:       //第二个状态，分析头部字段
            {
                ret_code = ParseHeaders(buffer_tmp);
                if (BAD_REQUEST == ret_code || GET_REQUEST == ret_code)
                    return ret_code;
                
                break;
            }
        default:
            return INTERNAL_ERROR;
        }
    }

    //若没有读取到一个完整的行，则表示还需要继续读取客户数据才能进一步分析
    if (LINE_OPEN == line_status)
        return NO_REQUEST;
    else
        return BAD_REQUEST;
}

//从状态机，用于解析出一行内容
HttpConnect::LINE_STATUS HttpConnect::ParseLine(char* buffer){
    char tmp;//用于暂存字节

    /* checked_index指向buffer(应用程序的读缓冲区)中当前正在分析的字节，
     * read_index指向buffer中客户数据的尾部的下一字节，buffer中第0~checked_index
     * 字节都已经分析完毕，第checked_index~(read_index - 1)字节由下面的循环挨个分析 */
    for (; m_checked_index < m_readed_index; ++m_checked_index)
    {
        tmp = buffer[m_checked_index];//获取当前要分析的字节
        if ('\r' == tmp){
            /* 如果"\r"字符是目前buffer中的最后一个已经被读入的客户数据，
             * 那么分析没有读取到一个完整的行，返回LINE_OPEN以表示还需要继续
             * 读取客户数据才能进一步分析 */
            if ((m_checked_index + 1) == m_readed_index){
                return LINE_OPEN;
            }
            else if ('\n' == buffer[m_checked_index + 1]){// 如果下一个字符是“\n”,则说明我们成功读取到一个完整的行
                buffer[m_checked_index++] = '\0';
                buffer[m_checked_index++] = '\0';
                return LINE_OK;
            }
            // 否则的话，说明客户发送的HTTP请求存在语法问题
            return LINE_BAD;
        }
        else if ('\n' == tmp){//如果当前的字节是"\n", 即换行符，则也说明可能读取到一个完整的行
            if ((m_checked_index > 1) && ('\n' == buffer[m_checked_index - 1])){
                buffer[m_checked_index-1] = '\0';
                buffer[m_checked_index++] = '\0';
                return LINE_OK;
            }

            return LINE_BAD;
        }
    }
    //如果所有内容分析完了也没有遇到'\r'，则说明有还需要继续读取客户数据才能进一步分析
    return LINE_OPEN;
}
//分析请求行
HttpConnect::HTTP_CODE HttpConnect::ParseRequestLine(char* buffer){
    //strpbrk检索字符串 str1 中第一个匹配字符串 str2 中字符的字符，不包含空结束字符。也就是说，依次检验字符串 str1 中的字符，当被检验字符在字符串 str2 中也包含时，则停止检验，并返回该字符位置
    char* url = strpbrk(buffer, " \t");
    // 如果请求航中没有空白字符或“\t”字符，则HTTP请求必有问题 
    if (!url)
        return BAD_REQUEST;

    *url++ = '\0';

    char* method = buffer;
    if (strcasecmp(method, "GET") == 0) // 仅支持GET方法 
        std::cout << "The request method is GET\n";
    else
        return BAD_REQUEST;

    /*strspn() 函数用来计算字符串url中连续有几个字符都属于字符串"\t"
    *返回字符串url开头连续包含字符串\t内的字符数目。
    *所以，如果url所包含的字符都属于\t，那么返回url的长度；如果url的第一个字符不属于\t，那么返回 0 */
    url += strspn(url, " \t");
    char* version = strpbrk(url, " \t");
    if (!version)
        return BAD_REQUEST;

    *version++ = '\0';
    version += strspn(version, " \t");
    // 仅支持HTTP/1.1 
    //strcasecmp()用来比较参数s1 和s2 字符串，比较时会自动忽略大小写的差异
    if (strcasecmp(version, "HTTP/1.1") != 0)
        return BAD_REQUEST;

    // 检查URL是否合法 
    if (strncasecmp(url, "http://", 7) == 0)
    {
        url += 7;
        url = strchr(url, '/');
    }

    if (!url || url[0] != '/')
    {
        return BAD_REQUEST;
    }

    //printf("The request URL is %s\n", url);
    std::cout << "The request URL is " << url << std::endl;
    // HTTP 请求行处理完毕，状态转移到头部字段的分析
    m_check_status = CHECK_STATUS_HEADER;
    return NO_REQUEST;
}
//分析头部字段
HttpConnect::HTTP_CODE HttpConnect::ParseHeaders(char* buffer){
    // 遇到一个空行，说明我们得到了一个正确的HTTP请求
    if (buffer[0] == '\0')
    {
        return GET_REQUEST;
    }
    else if (strncasecmp(buffer, "Host:", 5) == 0) // 处理"HOST"头部字段 
    {
        buffer += 5;
        buffer += strspn(buffer, " \t");
        printf("the request host is: %s\n", buffer);
    }
    else // 其他头部字段暂不处理 
    {
        printf("I can not handle this header\n");
    }

    return NO_REQUEST;
}