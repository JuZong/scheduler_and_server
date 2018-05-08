#include "consistent_hash.h"
#include <string>
#include <cstdio>
string HashCon::Md5Work(const string &str)
{
	MD5 md5(str);
	return string(md5.md5());
}
void HashCon::AddHaNode(int fd, string ip,short port,int count)
{
	ServerNode *node = new ServerNode(fd);
	HacNode.push_back(node);
	AddVirtual(ip, port,node,count);
}
void HashCon::DelVirNode(ServerNode *mSer,int count)
{
	vector<string>::iterator it=mSer->mVirNode.begin();
	for(;it!=mSer->mVirNode.end()&&count>0;it++,count--)
	{
		map<string,HaItem*>::iterator it1 =HaC.find(*it);
		if(it1!=HaC.end())
		{
			delete it1->second;
			HaC.erase(it1);
		}
		mSer->mVirNode.erase(it);
	}
}
HaItem::HaItem(ServerNode *mSer,string str,int mfd)
{
	mSerNode=mSer;
	Md5=str;
	fd=mfd;
}
void HashCon::AddVirtual(string ip,short port,ServerNode *node,int count)
{
		int i=node->mVirNode.size();
		int n=count+i;
		char b[10]="";
		for(;i<n;i++)
		{
			sprintf(b,"%d",i+port);
			string str=Md5Work(ip+b);
			HaItem *p=new HaItem(node,str,node->mSerfd);
			node->mVirNode.push_back(str);
			HaC.insert(make_pair(str,p));
		}
}
int HashCon::GetServer(string ip,short port)
{
		char buff_port[10]="";
		sprintf(buff_port,"%d",port);
		string str=buff_port+ip;
		string str_md5=Md5Work(str);
		map<string,HaItem*>::iterator it=HaC.upper_bound(str_md5);

		if(it!=HaC.end())
		{
			return it->second->fd;
		}
		map<string,HaItem*>::iterator it1=HaC.begin();
		return it1->second->fd;


}
void HashCon::DelHaNode(int fd)
{
	vector<ServerNode*>::iterator it=HacNode.begin();
	for(;it!=HacNode.end();it++)
	{
		if((*it)->mSerfd==fd)
		{
			//进行虚拟节点的删除:
			for(int i=0;i<(*it)->mVirNode.size();i++)
			{

					map<string,HaItem*>::iterator it2=HaC.find((*it)->mVirNode[i]);//删除
					delete(it2->second);
					HaC.erase(it2);
			}	
			delete (*it);
			HacNode.erase(it);
			break;
		}
	
	}
}



