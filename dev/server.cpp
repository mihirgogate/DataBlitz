#include<sys/socket.h>
#include<sys/epoll.h>
#include<netinet/in.h>
#include<stdio.h>
#include<stdlib.h>
#include<netdb.h>
#include<fcntl.h>
#include<errno.h>
#include<iostream>
#include "StringParserandInvoker.h"
#include "JSONParserAndInvoker.h"


#define MAX_EVENTS 128
#define MAX_INPUT_STRING_LENGTH 1024
#define MAX_OUTPUT_STRING_LENGTH 1024
#define JSON_PORT 8080

 static int createAndBind (char *port)
{

	 int sfd=socket(AF_INET,SOCK_STREAM,0);
	 if(sfd<0)
	 {
		 printf("Error in creating socket\n");
		 abort();
	 }

	 sockaddr_in selfAddress;
	 selfAddress.sin_family=AF_INET;
	 selfAddress.sin_addr.s_addr=INADDR_ANY;
	 selfAddress.sin_port=htons(atoi(port));

	 int ret = bind(sfd,(const struct sockaddr *)&selfAddress,(unsigned int)sizeof(selfAddress));
	 if(ret<0)
	 {
		 printf("Error in binding socket\n");
		 abort();
	 }
	 return sfd;


}


 static int makeSocketNonBlocking (int sfd)
 {
   int flags= fcntl (sfd, F_GETFL, 0);
   if (flags == -1)
     {
	   printf("Error getting socket flag");
       return -1;
     }

   flags = flags | O_NONBLOCK;

   int ret = fcntl (sfd, F_SETFL, flags);
   if (ret == -1)
     {
	   printf("Error setting socket flag for non-blocking");
       return -1;
     }

   return 0;
 }



int server_main (char *port,StringParserandInvoker* stringParserAndInvoker,JSONParserAndInvoker* jsonParserAndInvoker)
{
  int sfd, s;
  int efd;
  struct epoll_event event;
  struct epoll_event *events;
  char ip[MAX_INPUT_STRING_LENGTH];
  const char* op;
  int k=0;



  sfd = createAndBind (port);
  if (sfd < 0)
	  exit (EXIT_FAILURE);

  s = makeSocketNonBlocking (sfd);
  if (s < 0)
	  exit (EXIT_FAILURE);

  s = listen (sfd, SOMAXCONN);
  if (s<0)
    {
      printf("Error in listen system call");
      exit (EXIT_FAILURE);
    }

  efd = epoll_create1 (0);
  if (efd == -1)
    {
	  printf("Error in epoll_create system call");
	  exit (EXIT_FAILURE);
    }

  event.data.fd = sfd;
  event.events = EPOLLIN | EPOLLET;
  s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
  if (s == -1)
    {
	  printf("Error in setting epoll event flags");
	  exit (EXIT_FAILURE);
    }

  events = (epoll_event*)calloc ((unsigned int)MAX_EVENTS, (unsigned int)sizeof(event));

 /*start the event loops*/
  while (1)
    {
      int n, i;

      n = epoll_wait (efd, events, MAX_EVENTS, -1);
      for (i = 0; i < n; i++)
	{
	  if ((events[i].events & EPOLLERR) || (events[i].events & EPOLLHUP) || (!(events[i].events & EPOLLIN)))
	    {
	      //printf ("Error in epoll\n"); A client disconneted unexpectedly.
	      close (events[i].data.fd);
	      continue;
	    }

	  else if (sfd == events[i].data.fd)
	    {
              /* client connect */
              while (1)
                {
                  struct sockaddr in_addr;
                  socklen_t in_len;
                  int infd;
                  char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                  in_len = sizeof(in_addr);
                  infd = accept (sfd, &in_addr, &in_len);
                  if (infd == -1)
                    {
                      if ((errno == EAGAIN) || (errno == EWOULDBLOCK))
                        {
                          break;
                        }
                      else
                        {
                          printf("Error in accept system call");
                          break;
                        }
                    }


                  s = getnameinfo ((const sockaddr *)&in_addr, (unsigned int)in_len,hbuf, (unsigned int)sizeof(hbuf),sbuf,(unsigned int)sizeof(sbuf),NI_NUMERICHOST | NI_NUMERICSERV);

                  if (s == 0)
                    {
                      printf("Accepted connection on descriptor %d ""(host=%s, port=%s)\n", infd, hbuf, sbuf);
                    }

                  /* make the socket non blocking and add it to the list of sockets being monitored */
                  s = makeSocketNonBlocking (infd);
                  if (s == -1)
                  {
                	  exit(1);
                  }

                  event.data.fd = infd;
                  event.events = EPOLLIN | EPOLLET;
                  s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);
                  if (s == -1)
                    {
                      printf("Error setting epoll flags");
                      return 1;
                    }
                }
              continue;
            }
          else
            {
              int done = 0;

              while (1)
                {
                  ssize_t count;

                  for(k=0;k<MAX_INPUT_STRING_LENGTH;k++) {
                	  ip[k]='\0';
                  }

                  count = read (events[i].data.fd, (char*)ip, (unsigned int)(MAX_INPUT_STRING_LENGTH));
                  if (count == -1)
                    {
                      /* If errno == EAGAIN, that means we have read all
                         data. So go back to the main loop. */
                      if (errno != EAGAIN)
                        {
                          printf ("Error in reading data from client\n");
                          done = 1;
                        }
                      break;
                    }
                  else if (count == 0)
                    {
                      /* End of file. The remote has closed the
                         connection. */
                      done = 1;
                      break;
                    }

                  /*
                   *Find the results of the command
                   */

                  string ipAsString = ip;
                  printf("Input =  %s\n",ip);
                  fflush(stdout);
                  string opAsString;
                  //if listening on port JSON_PORT then use the JSON parse
                  if(atoi(port)==JSON_PORT) {
                	  printf("Using the JSON parser as listening on port %d\n",JSON_PORT);
                	  fflush(stdout);
                	  opAsString = jsonParserAndInvoker->operate(ipAsString).c_str();
                  }
                  else { //for all ports other than JSON_PORT use the String parser
                	  printf("Using the String parser as listening on a port other than %d",JSON_PORT);
                	  fflush(stdout);
                	  opAsString = stringParserAndInvoker->operate(ipAsString);
                	  //append a \n so telnet output is clean. TODO: remove when u find a better way
                	  opAsString =opAsString.append("\n");
                  }

                  op=opAsString.c_str();
                  printf("Output=%s\n",op);
                  fflush(stdout);
                  write(events[i].data.fd, (const char*)(op), (unsigned int)(opAsString.length()));

                }

              if (done)
                {
                  printf ("Closed connection on descriptor %d\n",events[i].data.fd);

                  /* Close the descriptor will make epoll remove it*/
                  close (events[i].data.fd);
                }
            }
        }
    }

  free (events);

  close (sfd);

  return EXIT_SUCCESS;
}
