


		/*
		 Name: Md.Golam Sarowar
        	 ID: 2015-2-60-107
		 Course Code: CSE325(Operating System)
		 Project part-1: webserver
		 Section: 03
		*/




	#include <stdio.h>
	#include <stdlib.h>
	#include <unistd.h>
	#include <errno.h>
	#include <string.h>
	#include <fcntl.h>
	#include <signal.h>
	#include <sys/types.h>
	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	#include <stdlib.h>
	#include <pthread.h>
	#include "threadpool.h"
	#include "threadpool.c"



	#define BUFSIZE 8096
	#define ERROR 42
	#define SORRY 43
	#define LOG 44





	struct {

		char *ext;
		char *filetype;
	} extensions [] = {
		{"gif", "image/gif" },
		{"jpg", "image/jpeg"},
		{"jpeg","image/jpeg"},
		{"png", "image/png" },
		{"zip", "image/zip" },
		{"gz", "image/gz" },
		{"tar", "image/tar" },
		{"htm", "text/html" },
		{"html","text/html" },
		{"php", "image/php" },
		{"cgi", "text/cgi" },
		{"asp","text/asp" },
		{"jsp", "image/jsp" },
		{"xml", "text/xml" },
		{"js","text/js" },
		{"css","test/css" },
		{0,0}
	};






	//creating structure to pass two values in web function with one structure type variabe


	struct parameter

	{

		int socketfd1;
		int hit1;

	};






	void log(int type, char *s1, char *s2, int num)

	{

		int fd ;
		char logbuffer[BUFSIZE*2];

		switch (type) {

		case ERROR: (void)sprintf(logbuffer,"ERROR: %s:%s Errno=%d exiting pid=%d",s1, s2, errno,getpid()); break;

		case SORRY:
		(void)sprintf(logbuffer, "<HTML><BODY><H1>Web Server Sorry: %s %s</H1></BODY></HTML>\r\n", s1, s2);
		(void)write(num,logbuffer,strlen(logbuffer));
		(void)sprintf(logbuffer,"SORRY: %s:%s",s1, s2);
		break;


		case LOG: (void)sprintf(logbuffer," INFO: %s:%s:%d",s1, s2,num); break;

		}

		if((fd = open("server.log", O_CREAT| O_WRONLY | O_APPEND,0644)) >= 0) {
		(void)write(fd,logbuffer,strlen(logbuffer));
		(void)write(fd,"\n",1);
		(void)close(fd);

		}



	}






	void web(void *p)

	{

		int fd,hit;


		//converting void * type variable to structure parameter type variable m

		struct parameter *m=(struct parameter *) p;


		//assigning value of socketfd and hit to fd and hit

		fd=m->socketfd1;
		hit=m->hit1;



		int j, file_fd, buflen, len;
		long i, ret;
		char * fstr;
		static char buffer[BUFSIZE+1];


		//reading contents from file descriptor fd into buffer array in one go

		ret =read(fd,buffer,BUFSIZE);




		//checking succesfully read or not

		if(ret == 0 || ret == -1) {
			log(SORRY,"failed to read browser request","",fd);
		}




		if(ret > 0 && ret < BUFSIZE)   //return code is valid chars
			buffer[ret]=0;         //terminate the buffer

		else buffer[0]=0;





		for(i=0;i<ret;i++)
			if(buffer[i] == '\r' || buffer[i] == '\n')
				buffer[i]='*';



		//comparing strings of buffer array and "GET" with 4 characters

		if( strncmp(buffer,"GET ",4) && strncmp(buffer,"get ",4) )
			log(SORRY,"Only simple GET operation supported",buffer,fd);




		//null terminate after the second space to ignore extra stuff

		for(i=4;i<BUFSIZE;i++) {
			if(buffer[i] == ' ') {
				buffer[i] = 0;
					break;
			}

		}




		for(j=0;j<i-1;j++){
			if(buffer[j] == '.' && buffer[j+1] == '.')
				log(SORRY,"Parent directory (..) path names not supported",buffer,fd);


		}




		if( !strncmp(&buffer[0],"GET /\0",6) || !strncmp(&buffer[0],"get /\0",6) )
			(void)strcpy(buffer,"GET /index.html");



		//determining length of buffer

		buflen=strlen(buffer);
		fstr = (char *)0;





		for(i=0;extensions[i].ext != 0;i++) {
			len = strlen(extensions[i].ext);
			if( !strncmp(&buffer[buflen-len], extensions[i].ext, len)) {
				fstr =extensions[i].filetype;
				break;
			}

		}





		if(fstr == 0)
			log(SORRY,"file extension type not supported",buffer,fd);


		if(( file_fd = open(&buffer[5],O_RDONLY)) == -1)
			log(SORRY, "failed to open file",&buffer[5],fd);


		log(LOG,"SEND",&buffer[5],hit);



		(void)sprintf(buffer,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);



		//writing contents of buffer array towards client's file descriptor through established connection
		(void)write(fd,buffer,strlen(buffer));



               //writing to file descriptor
		while ( (ret = read(file_fd, buffer, BUFSIZE)) > 0 ) {

		(void)write(fd,buffer,ret);

		}




		close(fd);     //closing file descriptor

		pthread_exit(NULL);   //exiting thread



	}







	int main(int argc, char **argv)

	{

		int i,port,rc,listenfd,socketfd,hit;
		size_t length;
		static struct sockaddr_in cli_addr;
		static struct sockaddr_in serv_addr;
		struct parameter *p;                                  //declaring pointer type structure variable
		pthread_t pid[100];                                        //declaring thread type variable


 		//chacking passed arguments through main function from client

		if( argc < 3 || argc > 3 || !strcmp(argv[1], "-?") ) {
			(void)printf("usage: server [port] [server directory] &"
			"\tExample: server 80 ./ &\n\n"
			"\tOnly Supports:");




		//chacking supported file extensions

		for(i=0;extensions[i].ext != 0;i++)
		     (void)printf(" %s",extensions[i].ext);
		(void)printf("\n\tNot Supported: directories / /etc /bin /lib /tmp /usr /dev /sbin \n");
		exit(0);

		}




		//comparing argv arguments specific characters with extension

		if( !strncmp(argv[2],"/" ,2 ) || !strncmp(argv[2],"/etc", 5 ) ||
		!strncmp(argv[2],"/bin",5 ) || !strncmp(argv[2],"/lib", 5 ) ||
		!strncmp(argv[2],"/tmp",5 ) || !strncmp(argv[2],"/usr", 5 ) ||
		!strncmp(argv[2],"/dev",5 ) || !strncmp(argv[2],"/sbin",6) ){
			(void)printf("ERROR: Bad top directory %s, see server -?\n",argv[2]);
			exit(3);
		}







		if(chdir(argv[2]) == -1){
		    (void)printf("ERROR: Can't Change to directory %s\n",argv[2]);
		    exit(4);

		}





		//Signal handling which makes sure that the child process never become zombie

		(void)signal(SIGCLD, SIG_IGN);
		(void)signal(SIGHUP, SIG_IGN);




		for(i=0;i<32;i++)
		   (void)close(i);





		//writing into log file

		log(LOG,"http server starting",argv[1],getpid());





		//establishing socket with domain, type or protocol
		//AF_INET refers to domain, protocol
		//SOCK_STREAM creates way to stream sequentially

		if((listenfd = socket(AF_INET, SOCK_STREAM,0)) <0)
		log(ERROR, "system call","socket",0);





		//extracting port number from string to int port

		port = atoi(argv[1]);





		//checking if port number is valid or not

		if(port < 0 || port >60000)
		    log(ERROR,"Invalid port number try [1,60000]",argv[1],0);





		serv_addr.sin_family = AF_INET;                          //set domain, type or protocol
		serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);          // set destination IP number - localhost, 127.0.0.1
		serv_addr.sin_port = htons(port);                       //set destinaton port number






		//binding server with the socket created, Through Bind() any server is made bound to the specific socket

		if(bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) <0)
		    log(ERROR,"system call","bind",0);





		if( listen(listenfd,64) <0)
		    log(ERROR,"system call","listen",0);




 		//An infinitive loop to process requests from each client. can be said as polling
		for(hit=1; ;hit++) {

		  	length = sizeof(cli_addr);




 		//Accepting a connection with the accept() function system call.
		//This call typically blocks until a client connects with the server.


		   	if((socketfd = accept(listenfd, (struct sockaddr *)&cli_addr, &length)) < 0) {
			 	log(ERROR,"system call","accept",0);

			}




		//allocating memory

		p=malloc( sizeof(struct parameter) );


		//assigning value of socketfd and hit to *p

		p->socketfd1=socketfd;
		p->hit1=hit;




		//creating thread

		pthread_create(&pid[hit], NULL, web, (void*) p);





		}


	close(socketfd);


	}
