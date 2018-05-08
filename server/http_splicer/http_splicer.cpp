#include "http_splicer.h"
#include <string>
#include <string.h>
#include <stdio.h>

using namespace std;

void Http_splicer::start_splice()
{
	m_response = "";
}

void Http_splicer::splice_statues_line(string _protocol,int _status_code)
{
	m_response.append(_protocol);
	m_response.append(" ");
	string buff;
	switch(_status_code)
	{
		case 200:
			buff = "200 OK \r\n";
			break;
		case 404:
			buff = "404 Not Found\r\n";
			break;
		case 403:
			buff = "403 Forbidden\r\n";
			break;
	}
	m_response.append(buff);
}

void Http_splicer::splice_msg_header()
{
	m_response.append("Server: JuZong-effective-Serve\r\n");
	m_response.append("Connection-Type: text/html\r\n");
	m_response.append("Connection: close\r\n");	
}

void Http_splicer::splice_content_length(int file_size)
{
	char buff[1024] = "";
	sprintf(buff,"Content-Length: %d\r\n",file_size);
	m_response.append(buff);
}

void Http_splicer::splice_empty_line()
{
	m_response.append("\r\n");
}

string& Http_splicer::get_result()
{
	return m_response;
}
