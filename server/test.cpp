

#include "redis.h"

int main()
{
	//构造对象
	Redis *r = new Redis();

	//连接redis
	if(!r->connect("127.0.0.1", 6379))
	{
		printf("connect error!\n");
		return 0;
	}
	//获取值
	cout<<"Get the name is "<< r->get("name")<<endl;

	//删除键值对
	r->decr("name");

	//获取值
	cout<<"Get the name is "<<r->get("name")<<endl;

	//++操作
	r->incr("name");
	
	cout<<"Get the name is "<< r->get("name")<<endl;
	
	delete r;
	return 0;
}
