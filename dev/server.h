/*
 * server.h
 *
 *  Created on: Oct 5, 2012
 *      Author: mihirgogate
 */

#ifndef SERVER_H_
#define SERVER_H_

int server_main (char* port,StringParserandInvoker* stringParserAndInvoker);
static int makeSocketNonBlocking (int sfd);
static int createAndBind (char *port);

#endif /* SERVER_H_ */
