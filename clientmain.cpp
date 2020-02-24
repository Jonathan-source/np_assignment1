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
void perror(char const * msg);

int main(int argc, char* argv[])
{

//==================================================================================================================================================
//															T C P	C L I E N T
//==================================================================================================================================================

	int sockfd;
	struct sockaddr_in serverAddr;
	char sendBuff[256];
	char recvBuff[256];

	if(argc != 2) 
	{
		fprintf(stderr,"ERROR, no port provided\n");
		printf("\nManual: %s <Server Port> \n", argv[0]);
		return -1;
	}

	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		perror("[Client]: could not create socket.");
	else
		printf("[Client]: socket was created.\n");

	
	memset(&serverAddr, '\0', sizeof(serverAddr));

	// Socket address information needed for binding.
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(atoi(argv[1]));										// Convert to network standard order.


	if(connect(sockfd, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) < 0)
		perror("[Client]: could not connect to server.");
	else
		printf("[Client]: successfully connected to the server.\n\n");

	if(recv(sockfd, recvBuff, sizeof(recvBuff), 0) < 0)
			perror("[Client]: could not read from socket.");
		else
			printf("[Server]: %s\n", recvBuff);

//==================================================================================================================================================
//														S E N D   A N D   R E C E I V E
//==================================================================================================================================================

	// Clear buffers.
	memset(sendBuff, '\0', sizeof(sendBuff));
	memset(recvBuff, '\0', sizeof(recvBuff));

	// Variables regarding calculator.
  	char command[10];
	int iValue[2], iResult;
	double dValue[2], dResult;

	// Receive calculations from server.
	if(recv(sockfd, recvBuff, sizeof(recvBuff), 0) < 0)
			perror("[Client]: could not read from socket.");
		else
			printf("[Server]: %s \n", recvBuff);



	// Check command for float or integer calculation.
	// Float calculation	
	if(recvBuff[0] == 'f') {
		// Scan and store data.
		sscanf(recvBuff,"%s %lf %lf", command, &dValue[0], &dValue[1]);

		// Perform calculation.
		dResult = calcResult(dValue[0], dValue[1], command);
		
		// Print result.
		sprintf(sendBuff, "%s %8.8g %8.8g = %8.8g \n",command, dValue[0], dValue[1], dResult);
	} 
	// Integer calculation
	else {
		// Scan and store data.
		scanf(recvBuff,"%s %d %d", command, &iValue[0], &iValue[1]);

		// Perform calculation.
		iResult = calcResult(iValue[0], iValue[1], command);

		// Print result.
		sprintf(sendBuff, "%s %d %d = %d \n",command, iValue[0], iValue[1], iResult);
	}


	// Send the calculation back to the client.
	if(send(sockfd, sendBuff, sizeof(sendBuff), 0) < 0)
		perror("[Server]: could not write to socket.");


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