#ifndef HTTP_PARSER_H
#define HTTP_PARSER_H
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include "../redis/redis.h"

using namespace std;
class Http_parser
{
public:
	bool start_parse(const char *str);// 从json串中拿到http头
	string get_mothd()//解析是GET 还是 POST
	{
		return m_mothd;
	}
	string get_file_name();//GET
	string get_source_name();//POST
	string get_protocol()//得到协议
	{
		return m_protocol;
	}
	bool is_file_available();
	bool is_source_available();

private:
	string m_mothd;
	string m_file_name;
	string m_source_name;
	string m_protocol;
};

#endif
