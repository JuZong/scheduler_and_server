#include "server.h"
#include "../log/log.h"
#include <stdlib.h>//atoi
#include <iostream>
#include <string.h>
#include <string>
using namespace std;//对string有用
Logger *log = Logger::get_log("../../logfile/server.lg");
int main(int argc, char **argv)
{
	if (argc < 3)
	{
		cout<<"argv num error"<<endl;
		return -1;			 
	}
	string ip = argv[1];
	int port = atoi(argv[2]);
	Server ser(ip, port);
	ser.run();

	return 0;
}

