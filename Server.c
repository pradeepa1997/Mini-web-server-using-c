#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<string.h>
#include<sys/stat.h>
#include <fcntl.h>
#include <sys/sendfile.h>

#define B_SIZE 2048



int main(int argc,char * argv[]){
    //port number is taken in command line
    if(argc<2){
        printf("Please insert a port number\nYou should give a port number as command line argument\n");
        return -1;
    }
     
    //define variables 
    int server_socket_filedes,client_socket_filedes,port_no,on=1;
    char buffer[B_SIZE];
    struct sockaddr_in server_addrress,client_address;
    socklen_t client_len;

    //create a socket;returns a socket  file descriptor here
    server_socket_filedes=socket(AF_INET,SOCK_STREAM,0);
    if(server_socket_filedes<0){ //if socket isn't created successfully, it returns minus value otherwise relevent file descriptor	
        printf("!!!!!!!!!!!   Did not open a socket  !!!!!!!!!!\n");
        return -1; 
    }
    printf("-----------Socket created------------\n\n");
   
    
    
    // there can be garbage values assign to the created variables
    //using this function,garbage values can be clear and set the value as null for the given variable.
    // Name and size of the varible that you want to clear,passed as parameters here
    bzero((char*)&server_addrress,sizeof(server_addrress));
   
    port_no=atoi(argv[1]);  //port number is taken as string value above.using atoi function the sring value cast to int

    server_addrress.sin_family=AF_INET; //assign values to sever structure 
    server_addrress.sin_addr.s_addr=INADDR_ANY;

    server_addrress.sin_port=htons(port_no); //htons function translates a short integer from host byte order to network byte order

    //bind the socket into our gieven port
    int bindreturn=bind(server_socket_filedes,(struct sockaddr*)&server_addrress,sizeof(server_addrress));
    if(bindreturn==-1){
        printf("OOpsss!!!!!!!!\nDid Not Bind the Sokcet\n");  //if bind failed it returns -1 otherwise 0
        return -1;
    }
    printf("--------------Socket binded-------------\n\n\n");

    //here it waits for the client to approach the server to make a connection.10 define how many pending connections willbe queued for processing 
    int lis=listen(server_socket_filedes,10);
    if(lis==-1){
        // If listen function not succesfully execute it returns -1 otherwise 0
        printf("------------Somthing going wrong-------------\n\n");
        close(server_socket_filedes);
        return -1;
    }
    printf("Server is Listning in port :%d\n\n\n",port_no);

    client_len=sizeof(client_address);//take the length of the client request variable
    while(1){
	    //accept function accept the connection and create new file descriptor for came new socket
            client_socket_filedes=accept(server_socket_filedes,(struct sockaddr*)&client_address,&client_len);
        // if creating connection unsuccess accept return -1 otherwise relevent file descriptor
            if(client_socket_filedes==-1){
                printf("Did Not accept a connection\n\n\n"); 
                continue;
            }
            printf("\n\n!!!!!!!!!!!!!New client has connect!!!!!!!!!!\n\n"); //when client_len==0

	    //fork is used for creating a new process, which is called child process
        //it runs concurrently with process that makes fork() call(parent process)      	    
        //it returns an integer value
	    //pid value is zero in child procce and positive in parent process which is child process process id
            //it will be negative if function doesn't work well		
            int pid=fork();

            int readval;
           
            if(pid==0){ //child process
                // we dont need server socket in our new created child process therefore it close 
                close(server_socket_filedes);
                // buffer clearing
                bzero(buffer,B_SIZE);
                // read the data which sent by client
                readval=read(client_socket_filedes,buffer,B_SIZE-1);
               
                if(readval<0){
                    printf("------------Did Not read data----------\n\n\n");
                    return -1;
                }
                // print client's request
                printf("Client requested:%s\n",buffer);
                        
                
                
                // according to the requests;relevant data is sent to the client
                if(!strncmp(buffer,"GET / HTTP/1.1",strlen("GET / HTTP/1.1"))){
                    
                        int readfile=open("web/index.html",O_RDONLY);
                        sendfile(client_socket_filedes,readfile,NULL,10000);
                        close(readfile);

                }else if(!strncmp(buffer,"GET /index.jpeg HTTP/1.1",strlen("GET /index.jpeg HTTP/1.1"))){
                    
                        int readfile=open("web/index.jpeg",O_RDONLY);
                        sendfile(client_socket_filedes,readfile,NULL,15000);
                        close(readfile);

                }else if(!strncmp(buffer,"GET /hi HTTP/1.1",strlen("GET /hi HTTP/1.1"))){
                
                        int readfile=open("web/hi.html",O_RDONLY);
                        sendfile(client_socket_filedes,readfile,NULL,10000);
                        close(readfile);

                }else if(!strncmp(buffer,"GET /style.css HTTP/1.1",strlen("GET /style.css HTTP/1.1"))){
                        
                        int readfile=open("web/style.css",O_RDONLY);
                        sendfile(client_socket_filedes,readfile,NULL,10000);
                        close(readfile);

                }else if(!strncmp(buffer,"GET /main.js HTTP/1.1",strlen("GET /main.js HTTP/1.1"))){
                        
                        int readfile=open("web/main.js",O_RDONLY);
                        sendfile(client_socket_filedes,readfile,NULL,10000);
                        close(readfile);
                }else{
                    // if clients request data which is not available at our server
                        int readfile=open("web/notFound.html",O_RDONLY);
                        sendfile(client_socket_filedes,readfile,NULL,10000);
                        close(readfile);
                       
                }
            //    Finally  clear buffer,close client socket and finally terminated our child process because its no need anymore
                
                bzero(buffer,B_SIZE);
                printf("Data send to client\n");
                close(client_socket_filedes);
                return -1;
                

            }else if(pid>0){
                // this block executed in our parent process .In the parent no need any more a client therfore we close our client socket here 
                close(client_socket_filedes);
            }else{
            //if the fork function returns minus value that means it doesn't run well so that we close the client but no data is sent to the client 
                close(client_socket_filedes);
            }
            
            
    }
    // finally close the server socket here
    close(server_socket_filedes);
    return 0;
}
