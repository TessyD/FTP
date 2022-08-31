#include "server.h"
#include "success.c"
int newSocket;
int count = 0;
char file[1024]; // to store contents from client in server
char file_name[100];
char pwdbuf[256];
char list[1024];
int UserStatus = 0;
//struct stat st = {0};

void split(char *pathaname);
int changeDirectory(char *directory);
int show_currentDirectory();
int ListFilesInDirectory();
int makeDirectory(char *directory);
int write_file(char *filename, int sockfd);
int retr_file(char *filename);
void success();

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("error");
    }
    printf("\e]2;FTP-Server\a");

    int ret, portNumber;
    struct sockaddr_in serverAddr; // Server Socket Address Structure
    struct sockaddr_in newAddr;    // Client Socket Address Structure
    socklen_t addr_size;           // Value Result Argument.

    int sockfd;
    char buffer[MAXLINE];
    char buflist[MAXLINE];
    pid_t childpid;
    

    /*================================================SOCKET-CREATION================================================*/
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Error in connection.\n");
        exit(1);
    }

    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    sscanf(argv[1], "%d", &portNumber); // IPv4
    serverAddr.sin_port = htons((uint16_t)portNumber);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Server IP-address of '127.0.0.1'
    printf("Server Socket is created with IP-address %s.\n", inet_ntoa(serverAddr.sin_addr));

    ret = bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    if (ret < 0)
    {
        printf("Error in binding.\n");
        exit(1);
    }
    printf("Bind to port %d\n", portNumber);

    /*===============================================PASSIVE-LISTENING===============================================*/
    if (listen(sockfd, 10) == 0)
    {
        printf("Listening....\n");
    }
    else
    {
        printf("Error in binding.\n");
    }
    while (1)
    {

        /*================================================ACCEPT-CONNECTIONS================================================*/
        newSocket = accept(sockfd, (struct sockaddr *)&newAddr, &addr_size); // accept a new connection
        if (newSocket < 0)
        {
            exit(1);
        }

        printf(">> Connection accepted from %s:%d\n", inet_ntoa(newAddr.sin_addr), ntohs(newAddr.sin_port));
        printf(">> But Client is not yet Logged In.\n");
        if ((childpid = fork()) == 0)
        {
            close(sockfd);

            while(1)
            {
                recv(newSocket, buffer, 1024, 0);
                if (strncmp(buffer, "USER", 4) == 0)
                {
                    UserStatus = 1;
                    printf(">> Client - %s has Successfully Logged In.\n");
                    char msg[256] = "User has successfully logged in";
                    send(newSocket, msg, strlen(msg), 0); // send the user logged in message to client
                    success();
                    // if(UserStatus == 0)
                    //     break;
                }
                else{
                    char msg1[256] = "user has to login to perform the operations";
                    send(newSocket, msg1, strlen(msg1), 0);
                }
            }
            bzero(buffer, sizeof(buffer));
        }
    }
    close(newSocket);
    return 0;
}


