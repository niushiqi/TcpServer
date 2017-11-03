#include "sock_api.h"

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
   	int sockfd, new_sockfd;
    int size;
    char revdata[255];
    int ret = -1;
  
    struct sockaddr_in saddr;  
    struct sockaddr_in caddr;  
  
    size = sizeof(struct sockaddr_in);  

    sys_console_printf("init! \r\n");
    
    /*初始化sockaddr_in saddr*/  
    bzero(&saddr, sizeof(saddr));   //先清零saddr  
    saddr.sin_family = AF_INET;     //AF_INET表示IPv4的类型  
    saddr.sin_port = VOS_AtoI(argv[0]);   //绑定8888端口  
    saddr.sin_addr.s_addr = inet_addr(argv[1]);   //绑定本机端口  

    sys_console_printf("port:%d, ip:%s->%x \r\n", saddr.sin_port, argv[1], saddr.sin_addr.s_addr);
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

    recv(new_sockfd, revdata, 255, 0);

    sys_console_printf("data:%s \r\n", revdata);
    sys_console_printf("send! \r\n");
  
    send(new_sockfd, "connect sucess!", sizeof("connect sucess"), 0);  

    sys_console_printf("close! \r\n");

    close(new_sockfd);
  
	return CMD_SUCCESS;	
}
