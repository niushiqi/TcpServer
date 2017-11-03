#include "sock_api.h"
#include "vos/vospubh/vos_byteorder.h"

int efm_tcp_server_port = 0;
long efm_tcp_server_ip = 0;
VOID tcpServerWorkTask()
{
    int sockfd, new_sockfd;
    int size;
    char revdata[255];
    int ret = -1;
  
    struct sockaddr_in saddr;  
    struct sockaddr_in caddr;  
  
    size = sizeof(struct sockaddr_in);  

    sys_console_printf("init! \r\n");
    sys_console_printf("port:%d, ip:%x \r\n", efm_tcp_server_port, efm_tcp_server_ip);
    
    /*初始化sockaddr_in saddr*/  
    bzero(&saddr, sizeof(saddr));   //先清零saddr  
    saddr.sin_family = AF_INET;     //AF_INET表示IPv4的类型  
    saddr.sin_port = VOS_HTONS(efm_tcp_server_port);   //绑定8888端口  
    saddr.sin_addr.s_addr = VOS_HTONL(efm_tcp_server_ip);   //绑定本机端口  

    sys_console_printf("port:%d, ip:%x \r\n", saddr.sin_port, saddr.sin_addr.s_addr);
    sys_console_printf("socket! \r\n");
  
    sockfd = socket(AF_INET, SOCK_STREAM, 0);//返回-1表示失败；  

    sys_console_printf("sockfd:%d \r\n", sockfd);
    sys_console_printf("bind! \r\n");
    
    bind(sockfd, (struct sockaddr*)&saddr, size);//返回-1表示失败  

    sys_console_printf("listen! \r\n");
    
    ret = listen(sockfd, 5);  //返回-1表示失败

    sys_console_printf("ret:%d \r\n", ret);
    sys_console_printf("accept! \r\n");
    
    new_sockfd = accept(sockfd, (struct sockaddr*)&caddr, &size);//返回-1表示失败  

    sys_console_printf("new_sockfd:%d \r\n", new_sockfd);
    sys_console_printf("recv! \r\n");

    if(recv(new_sockfd, revdata, 255, 0) > 0)
    {
        sys_console_printf("data:%s \r\n", revdata);
        sys_console_printf("send! \r\n");
      
        send(new_sockfd, "connect sucess!", sizeof("connect sucess"), 0);  
    }

    sys_console_printf("close! \r\n");

    close(new_sockfd);
    close(sockfd);
}


DEFUN( efm_tcp_server,
        efm_tcp_server_cmd,
        "efm tcp server <1-10000> <A.B.C.D>",
        "aaa\n"
        "bbb\n"
        "ccc\n"
        "ddd\n"
        "eee\n"
        )
{
    LONG lArg[ 10 ] = {0};

    efm_tcp_server_port = VOS_AtoI(argv[0]);
    efm_tcp_server_ip = inet_addr(argv[1]);
    
    if(VOS_TaskCreate("tcpserver", 100, tcpServerWorkTask, lArg) == -1)
    {
        sys_console_printf("task error !\r\n");
    }
  
	return CMD_SUCCESS;	
}

