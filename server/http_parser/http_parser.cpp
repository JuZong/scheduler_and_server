#include "http_parser.h"
#include <string.h>
#include <unistd.h>
#include <assert.h>
bool Http_parser::start_parse(const char *str)
{
	char *buff = new char[strlen(str)];
	if(buff == NULL)
	{
		return false;
	}
	assert(buff != NULL);
	strcpy(buff,str);

	char *p = strtok(buff,"\r\n");
	if(p == NULL)
	{
		return false;
	}
	assert(p != NULL);
	string tmp = p;

	//从字符串tmp 下标0开始，查找字符' ' ,返回' ' 在tmp 中的下标,找不到' '会返回-1
	string::size_type i = tmp.find(' ',0);

	//npos 是这样定义的：
	//static const size_type npos = -1;
	if(i == string::npos)
	{
		m_mothd = "NONE";
	}
	// string& assign ( const string& str, size_t pos, size_t n  );
	// 将str的内容从位置pos起的n个字符作为原字串的新内容赋给原字串
	m_mothd.assign(tmp,0,i);

	string::size_type j = tmp.find(' ',i+1);
	if(j == string::npos)
	{
		m_file_name = "NONE";
	}
	else
	{
		m_file_name.assign(tmp,i+1,j-i-1);
	}
	m_protocol.assign(tmp,j+1,tmp.size()-j-1);
	delete []buff;
	return true;
}

string Http_parser::get_file_name()
{
	if(m_file_name.compare("/") == 0)
	{
		return string("index.html");
	}
	return m_file_name;
}

bool Http_parser::is_file_available()
{
	string prefix("../doc");
	prefix.append(m_file_name);

	//access系统调用判断文件是否存在，并且可读，-1表示error.
	return (access(prefix.c_str(),F_OK|R_OK) != -1);
}

bool Http_parser::is_source_available()
{
	
}
	
