#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


int main(int argc,char** argv)
{
	//第一步：创建一个套接字
	int sock = socket(AF_INET,SOCK_STREAM,0);
	/*
	socket 函数的参数解读：
	第一个参数表示地址家族。基本上都是AF_INET，表示使用英特网地址家族；
	第二个参数表示套接字类型，通常有两种，即SOCK_ATREAM和SOCK_DGRAM，前者表示流式套接字，用于基于TCP协议的通信，后者表示数据报式套接字，用于基于UDP协议的通信；
	第三个参数表示使用的通信协议编号，0表示使用默认协议；
	socket函数的返回值为新套接字的描述符，如果失败，返回值为-1。
	*/

	if(sock == -1)
	{
		perror("socket");
		return 1;
	}

	/*
	//第2步，绑定地址（IP+Port）（显示绑定）
	//可有可无
	struct sockaddr_in myaddr;
	myaddr.sin_family = AF_INET; //指定地址家族
	myaddr.sin_addr.s_addr = INADDR_ANY;	//指定使用本机任意地址（0.0.0.0）
	//myaddr.sin_addr.s_addr = inet_addr("具体ID"); //指定使用本机某个具体的IP地址
	myaddr.sin_port = 8888; //指定使用的端口号为8888

	if(-1 == bind(sock,(struct sockaddr*)&myaddr,sizeof(myaddr)))
	{
		perror("bind");
		return 1;
	}
	*/

	/*
	inet_addr函数的作用：将字符串形式的IP地址转换为无符号32位整数形式（网络字节序）
	htons函数的作用：将某个短整数（short）从主机（host）字节序转换为网络（network）字节序。
	*/


	//第3步：连接服务器
	struct sockaddr_in srv_addr;
	srv_addr.sin_family = AF_INET; //指定地址家族
	srv_addr.sin_addr.s_addr = inet_addr(argv[1]);	//指定目标服务器的IP地址
	srv_addr.sin_port = htons(atoi(argv[2])); //指定目标服务器的端口号

	if(-1 == connect(sock,(struct sockaddr*)&srv_addr,sizeof(srv_addr)))
	{
		//连接服务器失败
		perror("connect");
		return 1;
	}
	 
	//第4步：收发数据
	char data[300];
	int ret = recv(sock,data,sizeof(data),0);  //最后一个参数为0表示采用默认的方式接收数据，返回值为实际成功接收的字节数，小于或等于其第三个参数
	if(ret == -1)
	{
		perror("recv");
		return 1;
	}
	else
	{
		printf("服务器说：%s\n",data);
	}

	//int ret = read(sock_conn,data,sizeof(data));  //和recv函数等效
	int fd = open(data,O_CREAT | O_WRONLY | O_APPEND,0777);
	if(fd == -1)
		{
			perror("open");
			exit(1);
		}
	char buff[1024] = {0};
	while(1)
	{
		ret = recv(sock,buff,sizeof(buff),0);
		if(ret <= 0)
		{
			perror("recv");
			break;
		}
		write(fd,buff,ret);
	}
	close(fd);



	//第5步：断开连接（如果不需要继续和目标服务器端的话）
	close(sock);

	return 0;
}
