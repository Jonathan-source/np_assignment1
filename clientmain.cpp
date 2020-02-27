#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/types.h> 		
#include <sys/socket.h>		 
#include <netinet/in.h>	
/* You will to add includes here */


// Included to get the support library
#include <calcLib.h>

int calcResult(const int &value1, const int &value2, const char *type);
double calcResult(const double &value1, const double &value2, const char *type);
void perror(char const * msg){
	exit(EXIT_FAILURE);
}

int main(int argc, char* argv[])
{

//==================================================================================================================================================
//															T C P	C L I E N T
//==================================================================================================================================================

	int sockfd;
	struct sockaddr_in serverAddr;
	char sendBuff[256];
	char recvBuff[256];

	// Clear buffers.
	memset(sendBuff, '\0', sizeof(sendBuff));
	memset(recvBuff, '\0', sizeof(recvBuff));

	if(argc != 3) 
	{
		fprintf(stderr,"ERROR, incorrect arguments.\n");
		printf("Manual: %s <IP Address> <Server Port> \n", argv[0]);
		return EXIT_FAILURE;
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		perror("could not create socket.");
	}
	printf("Socket was created.\n");


	// Socket address information needed for binding.
	memset(&serverAddr, '\0', sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[2]));			// Convert to network standard order.
	serverAddr.sin_addr.s_addr = inet_addr(argv[1]);

	if(connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
	{
		perror("could not connect to server.");
	}
	printf("Successfully connected to the server.\n");

	if(recv(sockfd, recvBuff, sizeof(recvBuff), 0) < 0)
	{
			perror("could not read from socket.");
	}
	printf("%s\n", recvBuff);
	
	// ACK
	if(send(sockfd, "OK\n", 2, 0) < 0)
	{
		perror("could not write to socket.");
	}

//==================================================================================================================================================
//														S E N D   A N D   R E C E I V E
//==================================================================================================================================================

	// Clear buffers.
	memset(sendBuff, '\0', sizeof(sendBuff));
	memset(recvBuff, '\0', sizeof(recvBuff));

	// Variables regarding calculator.
  	char command[10];
	int iValue[2] {0}, 
		iResult = 0;
	double 	dValue[2] {0.0},
			dResult = 0.0;

	// Receive calculations from server.
	if(recv(sockfd, recvBuff, sizeof(recvBuff), 0) < 0)
	{
			perror("could not read from socket.");
	}
	printf("%s\n", recvBuff);


	// Check command for float or integer calculation.
	// Float calculation.	
	if(recvBuff[0] == 'f') 
	{
		// Scan and store data.
		sscanf(recvBuff, "%s%lf%lf", command, &dValue[0], &dValue[1]);

		// Perform calculation.
		dResult = calcResult(dValue[0], dValue[1], command);
		
		// Print result.
		printf("%8.8g\n", dResult);

		// Store result in sendBuff.
		sprintf(sendBuff, "%lf", dResult);
	} 
	// Integer calculation.
	else {
		// Scan and store data.
		sscanf(recvBuff, "%s%d%d", command, &iValue[0], &iValue[1]);

		// Perform calculation.
		iResult = calcResult(iValue[0], iValue[1], command);

		// Print result.
		printf("%d\n", iResult);

		// Store result in sendBuff.
		sprintf(sendBuff, "%d", iResult);
	}

	// Send the result back to the client.
	if(send(sockfd, sendBuff, strlen(sendBuff), 0) < 0)
	{
		perror("could not write to socket.");
	}

	memset(recvBuff, '\0', sizeof(recvBuff));
	if(recv(sockfd, recvBuff, sizeof(recvBuff), 0) < 0)
	{
		perror("could not read from socket.");
	}
	printf("%s\n", recvBuff);


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