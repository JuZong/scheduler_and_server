#ifndef HTTP_SPLICER_H
#define HTTP_SPLICER_H

#include <string>
using namespace std;

class Http_splicer
{
public:
	void start_splice();

	//协议和状态码
	void splice_statues_line(string _protocol,int _status_code);
	
	//server connection
	void splice_msg_header();
	//如果是GET,报头中封装页面文件的长度
	void splice_content_length(int file_size);
	//报头最后一行的\r\n
	void splice_empty_line();
	
	//返回最后组装的http报头
	string& get_result();
private:
	string m_response;
};
#endif
