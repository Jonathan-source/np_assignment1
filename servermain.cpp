#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h> 		
#include <sys/socket.h>		 
#include <netinet/in.h>	

// Included to get the support library
#include <calcLib.h>

using namespace std;

int calcResult(const int &value1, const int &value2, const char *type);
double calcResult(const double &value1, const double &value2, const char *type);
void perror(char const * msg);



int main(int argc, char *argv[])
{

	initCalcLib();


//==================================================================================================================================================
//															T C P	S E R V E R 
//==================================================================================================================================================
	
	int sockfd;
	struct sockaddr_in serverAddr;

	int connfd;
	struct sockaddr_in clientAddr;
	socklen_t clientLen;

	char sendBuff[256];
    char recvBuff[256];

    if(argc != 2) 
	{
		fprintf(stderr,"ERROR, no port provided\n");
		printf("\nManual: %s <Server Port> \n", argv[0]);
		return -1;
	}
	int SERVER_PORT = atoi(argv[1]);


//============== 1. Create a socket using socket(). ================================


    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("[Server]: socket was not created.");
	else
		printf("[Server]: socket was created.\n");

	
//============== 2. Bind the socket to a socket address using bind(). =============

	
	printf("[Server]: setting up server...\n");	

	memset(&serverAddr, 0, sizeof(serverAddr));  		// Zero out structure.

	// Socket address information needed for binding.
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(SERVER_PORT);			// Convert to network byte order.
	serverAddr.sin_addr.s_addr = INADDR_ANY; 			// "0.0.0.0". 

	if(bind(sockfd, (struct sockaddr * ) &serverAddr, sizeof(serverAddr)) < 0)
		perror("[Server]: socket was not bound to a socket address. Server setup failed.");
	else
		printf("[Server]: socket was bound to a socket address. Server setup complete.\n");


//============== 3. Listen for connections using listen(). ========================

	
	if(listen(sockfd, SOMAXCONN) < 0)	
		perror("[Server]: listen was not performed.");
	else
		printf("[Server]: waiting for connection on port: %i...\n\n", SERVER_PORT);


//============== 4. Accept a connection using accept(). ===========================


	clientLen = sizeof(clientAddr);
		
	if((connfd = accept(sockfd, (struct sockaddr *) &clientAddr, &clientLen)) < 0)
		perror("[Server]: could not connect with the client.");
	
	if(send(connfd, "TEXT TCP 1.0 server.", 50, 0) < 0)
		perror("[Server]: could not write to socket.");


//==================================================================================================================================================
//														S E N D   A N D   R E C E I V E
//==================================================================================================================================================

	// Clear buffers.
	memset(sendBuff, '\0', sizeof(sendBuff));
	memset(recvBuff, '\0', sizeof(recvBuff));


	// Variables regarding calculator.
	char *randType = randomType();
	int iRandNum[2], iResult;
	double dRandNum[2], dResult;
	bool isDouble = false;

	// Check if random command should execute a float or integer calculation.
	if(randType[0] == 'f') {
		dRandNum[0] = randomFloat();
		dRandNum[1] = randomFloat();
		isDouble = true;
	} 
	else {
		iRandNum[0] = randomInt();
		iRandNum[1] = randomInt();
	}


	// Float calculation
	if(isDouble)
	{
		// Perform calculation.
	 	dResult = calcResult(dRandNum[0], dRandNum[1], randType);
	  	
	  	// Print result.
 	 	printf("%s %8.8g %8.8g = %8.8g \n", randType, dRandNum[0], dRandNum[1], dResult);

 	 	// Store calculation data in sendBuffer.
 	 	sprintf(sendBuff, "%s %8.8g %8.8g = %8.8g \n",randType, dRandNum[0], dRandNum[1], dResult);
  	}
  	// Integer calculation
  	else {
		// Perform calculation.
		iResult = calcResult(iRandNum[0], iRandNum[1], randType);

		// Print result.
    	printf("%s %d %d = %d \n",randType, iRandNum[0], iRandNum[1], iResult);

    	// Store calculation data in sendBuffer.
    	sprintf(sendBuff, "%s %d %d = %d \n",randType, iRandNum[0], iRandNum[1], iResult);
	}

	// Send the calculation information to the client.
	if(send(connfd, sendBuff, sizeof(sendBuff), 0) < 0)
		perror("[Server]: could not write to socket.");


	// Receive calculation performed by client.
	if(recv(connfd, recvBuff, sizeof(recvBuff), 0) < 0)
		perror("[Server]: could not receive data.");
	else
		printf("[Client]: %s \n", recvBuff);



	// Check match.




	// Send.




	return 0;
}




int calcResult(const int &value1, const int &value2, const char *type)
{
	int result = 0;

		if(strcmp(type, "add") == 0) {
	      result = value1 + value2;
	    } 
	    else if (strcmp(type, "sub") == 0) {
	      result = value1 - value2;
	    } 
	    else if (strcmp(type, "mul") == 0) {
	      result = value1 * value2;
	    } 
	    else if (strcmp(type, "div") == 0) {
	      result = value1 / value2;
	    }

	return result;  
}

double calcResult(const double &value1, const double &value2, const char *type)
{	
	double result = 0;

		if(strcmp(type, "fadd") == 0) {
	      result = value1 + value2;
	    } 
	    else if (strcmp(type, "fsub") == 0) {
	      result = value1 - value2;
	    } 
	    else if (strcmp(type, "fmul") == 0) {
	      result = value1 * value2;
	    } 
	    else if (strcmp(type, "fdiv") == 0) {
	      result = value1 / value2;
	    } 

	return result;  
}