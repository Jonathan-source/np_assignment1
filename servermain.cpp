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

		// Variables regarding calculation.
		char *randType = randomType();
		int iRandNum[2]{0},
			iResult = 0,
			iCmprResult = 0;

		double 	dRandNum[2] {0.0},
			   	dResult = 0.0,
				dCmprResult = 0.0,
				dDelta = 0, 
				dEpsilon = 0.0001;

		bool isDouble = false;

//==================================================================================================================================================
//															T C P	S E R V E R 
//==================================================================================================================================================
	
	// Server variables
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
		printf("Manual: %s <Server Port> \n", argv[0]);
		return -1;
	}
	int SERVER_PORT = atoi(argv[1]);


//============== 1. Create a socket using socket(). ================================


    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
		perror("[Server]: socket was not created.");
		exit(1);
    }
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
	{
		perror("[Server]: socket was not bound to a socket address. Server setup failed.");
		exit(2);
	}
	else
		printf("[Server]: socket was bound to a socket address. Server setup complete.\n");


//============== 3. Listen for connections using listen(). ========================

	
	if(listen(sockfd, 1) < 0)
	{	
		perror("[Server]: listen was not performed.");
		exit(3);
	}
	else
		printf("[Server]: waiting for connection on port: %i...\n\n", SERVER_PORT);


//============== 4. Accept a connection using accept(). ===========================


	clientLen = sizeof(clientAddr);

	// THIS IS THE GAME-LOOP / SERVER-LOOP
	while(1)
	{	
		if((connfd = accept(sockfd, (struct sockaddr *) &clientAddr, &clientLen)) < 0)
		{
			perror("[Server]: could not connect with the client.");
			exit(4);
		}
		
		if(send(connfd, "TEXT TCP 1.0 server.\n", 50, 0) < 0)
		{
			perror("[Server]: could not write to socket.");
			exit(5);
		}
		printf("[Server]: client %s:%d connected, waiting for confirmation...\n", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);

		if(recv(connfd, recvBuff, sizeof(recvBuff), 0) < 0)
		{
			perror("[Client]: could not read from socket.");
			exit(3);
		}
		printf("[Client]: %s\n", recvBuff);


	//==================================================================================================================================================
	//														S E N D   A N D   R E C E I V E
	//==================================================================================================================================================

		// Clear buffers.
		memset(sendBuff, '\0', sizeof(sendBuff));
		memset(recvBuff, '\0', sizeof(recvBuff));


		// Reset variables.
		randType = randomType();
		iRandNum[2]={0};
		iResult = 0;
		iCmprResult = 0;
		dRandNum[2]={0.0};
		dResult = 0.0;
		dCmprResult = 0.0;
		dDelta = 0;
		dEpsilon = 0.0001;
		isDouble = false;

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
	 	 	printf("[Server]: %s %8.8g %8.8g = %8.8g \n", randType, dRandNum[0], dRandNum[1], dResult);

	 	 	// Store calculation data in sendBuffer.
	 	 	sprintf(sendBuff, "%s %8.8g %8.8g = %8.8g \n",randType, dRandNum[0], dRandNum[1], dResult);
	  	}
	  	// Integer calculation
	  	else {
			// Perform calculation.
			iResult = calcResult(iRandNum[0], iRandNum[1], randType);

			// Print result.
	    	printf("[Server]: %s %d %d = %d \n",randType, iRandNum[0], iRandNum[1], iResult);

	    	// Store calculation data in sendBuffer.
	    	sprintf(sendBuff, "%s %d %d = %d \n",randType, iRandNum[0], iRandNum[1], iResult);
		}


		// Send the calculation information to the client.
		if(send(connfd, sendBuff, sizeof(sendBuff), 0) < 0)
		{
			perror("[Server]: could not write to socket.");
			exit(6);
		}

		// ACK
		if(recv(connfd, recvBuff, sizeof(recvBuff), 0) < 0)
		{
			perror("[Client]: could not read from socket.");
			exit(3);
		}
		printf("[Client]: %s\n", recvBuff);
		memset(recvBuff, '\0', sizeof(recvBuff));


		// Receive calculation performed by client.
		if(recv(connfd, recvBuff, sizeof(recvBuff), 0) < 0)
		{
			perror("[Server]: could not receive data.");
			exit(7);
		}
		printf("[Client]: %s \n", recvBuff);


		// Check match.
		memset(sendBuff, '\0', sizeof(sendBuff));

		// Float value
		if(randType[0] == 'f') 
		{
			sscanf(recvBuff, "%lf ", &dCmprResult);
			dDelta = (dResult - dCmprResult);
			if(dDelta < 0) {
				dDelta *= -1.0;
			} 
			if (dDelta <= dEpsilon)
			{
				// Send OK to client.
				if(send(connfd, "OK", 2, 0) < 0)
				{
					perror("[Server]: could not write to socket.");
					exit(8);
				}
			} 
			else if(dDelta > dEpsilon)
			{
				// Send "Did not match" to client.
				if(send(connfd, "ERROR", 5, 0) < 0)
				{
					perror("[Server]: could not write to socket.");
					exit(9);
				}
			}

		} 
		// Integer value
		else
		{
			sscanf(recvBuff, "%d", &iCmprResult);
			if(iResult == iCmprResult) 
			{
				// Send OK to client.
				if(send(connfd, "OK", 2, 0) < 0)
				{
					perror("[Server]: could not write to socket.");
					exit(10);
				}
			} 
			else 
			{
				// Send "Did not match" to client.
				if(send(connfd, "ERROR", 5, 0) < 0)
				{
					perror("[Server]: could not write to socket.");
					exit(11);
				}
			}
		}

			// Disconnect current client.
			printf("[Server]: communication process complete.\n");
			printf("[Server]: client %s:%d disconnected.\n", inet_ntoa(clientAddr.sin_addr), clientAddr.sin_port);
			printf("[Server]: waiting for new connection on port: %i...\n\n", SERVER_PORT);

			// Close socket.
			

			memset(sendBuff, '\0', sizeof(sendBuff));
			memset(recvBuff, '\0', sizeof(recvBuff));
	}




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