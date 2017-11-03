/* tcpServer.c - TCP server example */
/* 
DESCRIPTION 
This file contains the server-side of the VxWorks TCP example code. The example code demonstrates the usage of several BSD 4.4-style socket routine calls. 
*/

/* includes */ 
#include "vxWorks.h" 
#include "sockLib.h" 
#include "inetLib.h" 
#include "taskLib.h" 
#include "stdioLib.h" 
#include "strLib.h" 
#include "ioLib.h" 
#include "fioLib.h" 
#include "tcpExample.h"

/* function declarations */

VOID tcpServerWorkTask (int sFd, char * address, u_short port);

/**************************************************************************** 
* 
* tcpServer - accept and process requests over a TCP socket 
* 
* This routine creates a TCP socket, and accepts connections over the socket 
* from clients.  Each client connection is handled by spawning a separate 
* task to handle client requests. 
* 
* This routine may be invoked as follows: 
*        -> sp tcpServer 
*        task spawned: id = 0x3a6f1c, name = t1 
*        value = 3829532 = 0x3a6f1c 
*        -> MESSAGE FROM CLIENT (Internet Address 150.12.0.10, port 1027): 
*        Hello out there 
* 
* RETURNS: Never, or ERROR if a resources could not be allocated. 
*/

STATUS tcpServer (void) 
    { 
    struct sockaddr_in  serverAddr;    /* server's socket address */ 
    struct sockaddr_in  clientAddr;    /* client's socket address */ 
    int                 sockAddrSize;  /* size of socket address structure */ 
    int                 sFd;           /* socket file descriptor */ 
    int                 newFd;         /* socket descriptor from accept */ 
    int                 ix = 0;        /* counter for work task names */ 
    char                workName[16];  /* name of work task */

    /* set up the local address */

    sockAddrSize = sizeof (struct sockaddr_in); 
    bzero ((char *) &serverAddr, sockAddrSize); 
    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_len = (u_char) sockAddrSize; 
    serverAddr.sin_port = htons (SERVER_PORT_NUM); 
    serverAddr.sin_addr.s_addr = htonl (INADDR_ANY);

    /* create a TCP-based socket */

    if ((sFd = socket (AF_INET, SOCK_STREAM, 0)) == ERROR) 
        { 
        perror ("socket"); 
        return (ERROR); 
        }

    /* bind socket to local address */

    if (bind (sFd, (struct sockaddr *) &serverAddr, sockAddrSize) == ERROR) 
        { 
        perror ("bind"); 
        close (sFd); 
        return (ERROR); 
        }

    /* create queue for client connection requests */

    if (listen (sFd, SERVER_MAX_CONNECTIONS) == ERROR) 
        { 
        perror ("listen"); 
        close (sFd); 
        return (ERROR); 
        }

    /* accept new connect requests and spawn tasks to process them */

    FOREVER 
        { 
        if ((newFd = accept (sFd, (struct sockaddr *) &clientAddr, 
            &sockAddrSize)) == ERROR) 
            { 
            perror ("accept"); 
            close (sFd); 
            return (ERROR); 
            }

        sprintf (workName, "tTcpWork%d", ix++); 
        if (taskSpawn(workName, SERVER_WORK_PRIORITY, 0, SERVER_STACK_SIZE, 
            (FUNCPTR) tcpServerWorkTask, newFd, 
            (int) inet_ntoa (clientAddr.sin_addr), ntohs (clientAddr.sin_port), 
            0, 0, 0, 0, 0, 0, 0) == ERROR) 
            { 
            /* if taskSpawn fails, close fd and return to top of loop */ 

            perror ("taskSpawn"); 
            close (newFd); 
            } 
        } 
    } 

/**************************************************************************** 
* 
* tcpServerWorkTask - process client requests 
* 
* This routine reads from the server's socket, and processes client 
* requests.  If the client requests a reply message, this routine 
* will send a reply to the client. 
* 
* RETURNS: N/A. 
*/ 

VOID tcpServerWorkTask 
    ( 
    int                 sFd,            /* server's socket fd */ 
    char *              address,        /* client's socket address */ 
    u_short             port            /* client's socket port */ 
    ) 
    { 
    struct request      clientRequest;  /* request/message from client */ 
    int                 nRead;          /* number of bytes read */ 
    static char         replyMsg[] = "Server received your message"; 

    /* read client request, display message */ 

while ((nRead = fioRead (sFd, (char *) &clientRequest, 
        sizeof (clientRequest))) > 0) 
        { 
        printf ("MESSAGE FROM CLIENT (Internet Address %s, port %d):\n%s\n", 
                address, port, clientRequest.message); 

        if (clientRequest.reply) 
            if (write (sFd, replyMsg, sizeof (replyMsg)) == ERROR) 
                perror ("write"); 
        } 

    if (nRead == ERROR)                 /* error from read() */ 
        perror ("read"); 

    close (sFd);                        /* close server socket connection */ 
    }