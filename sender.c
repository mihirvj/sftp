/************************************************************
*			     sender.c
*		Simple FTP Client Implementation
*	      Authors: Mihir Joshi, Fenil Kavathia
*			    csc573 NCSU
************************************************************/

#include "sock/csock.h"
#include "config.h"
#include<pthread.h>
#include "fops/fileop.h"

#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 7735
#define CLIENT_PORT 12001
#define TIMEOUT 5

#define EXPAND_WIN() SN = (SN + 1) % WINSIZE
#define SLIDE_WIN() SF = (SF + 1) % WINSIZE; \
		AN = (AN + MSS) % (WINSIZE * MSS)


uint SF = 0; // first outstanding frame index
uint SN = 0; // next frame to be sent
uint AN = 0; // next frame to be acknowledged
uchar *buffer;

void attachHeader(uchar segment[MSS], uint seq)
{	
	int i;

// move segment downwards to make space for header
	for(i=MSS-1;i>=HEADSIZE;i--)
		segment[i] = segment[i - HEADSIZE];

#ifdef GRAN1
	printf("[log] adding seq no: %d\n", seq);
#endif

// prepend header length
	segment[3] = (char) seq & 0xFF;
	segment[2] = (char) (seq >> 8) & 0xFF;
	segment[1] = (char) (seq >> 16) & 0xFF;
	segment[0] = (char) (seq >> 24) & 0xFF;

#ifdef GRAN1
	printf("[log] header attached segment: ");

	for(i=0;i<HEADSIZE;i++)
		printf("%d, ", (int) segment[i]);

	for(i=HEADSIZE;i<MSS;i++)
		printf("%c(%d), ", segment[i], (int) segment[i]);

	printf("\n");

//	printf("[log] header attached segment: %s\n", segment);
#endif
}

void storeSegment(uchar segment[MSS])
{
	int i;
	uint bufSize = WINSIZE * MSS * 2;

	for(i=0;i<MSS;i++)
	{
		buffer[(SN * MSS + i) % bufSize] = segment[i];
	}
}

void sendSegment(int sock, uchar segment[MSS], int buf_len)
{
	segment[buf_len] = '\0';

	write_to(sock, segment, buf_len, SERVER_ADDR, SERVER_PORT);
}

void *listener(void *arg);
int isValid(uchar segment[HEADSIZE]);
void goBackN(int sock);
void printWinStats();

pthread_attr_t attr;
pthread_t threads;
pthread_mutex_t mutex;

int main()
{
	int sock;
	uchar response[HEADSIZE], nextChar, segment[MSS];
	int file, i;
	int BUFSIZE = WINSIZE * MSS, curIndex = 0;
	uint seqNo = 0;

#ifdef APP
	int debug = 0;
#endif	

	//	|_|_|(|_|_|_|_|_|_|_|)|_|_|
	//	      SF    WINDOW    SN

	/******************* Initialization ************/
	// buffer is a data structure over which window moves
	buffer = (char *) malloc(WINSIZE * MSS * 2);

	SF = 0;
	SN = 0;

	sock = get_sock();

	bind_sock(sock, CLIENT_PORT, TIMEOUT); // seconds timeout

	file = get_file_descriptor("test/sending.txt", Read);
	
	/***************** Go back N Algorithm **********/

	// start a listener thread
	pthread_mutex_init(&mutex, NULL);
	pthread_create(&threads, &attr, &listener, &sock);

	memset(segment, '\0', MSS);

	while(1)
	{
		nextChar = rdt_send(file); // read next char

#ifdef GRAN1
	printf("[log] reading char: %c\n", nextChar);
#endif

		if((int) nextChar == 0 && curIndex > 0) // nothing more to read.. phew!
		{
#ifdef GRAN1
	printf("\nSending final segment at curIndex = %d\n", curIndex);
#endif
			// send final segment
			attachHeader(segment, seqNo);

			storeSegment(segment);

			sendSegment(sock, segment, curIndex + HEADSIZE);

			strcpy(segment, "<FINMJ>");

			sendSegment(sock, segment, strlen(segment));

			break;
		}

		while((SN - SF) >= WINSIZE) // wait while window iss full
		{
#ifdef APP
	printf("[log] Win size full.. waiting\n");
	printWinStats();
#endif
			sleep(2);
		}

		segment[curIndex] = (int) nextChar; // add to buffer

		if((curIndex % (MSS - HEADSIZE - 1)) == 0 && curIndex > 0) // i've got 1 MSS data without header
		{
			/****** Make Segment *******/

			attachHeader(segment, seqNo);

			storeSegment(segment);

			sendSegment(sock, segment, MSS);

			EXPAND_WIN();

			printWinStats();

#ifdef GRAN1
	//if(seqNo > 1000)// || debug == 1)
	{
		sleep(1);
		debug = 1;
	}
#endif 
			seqNo = (seqNo + MSS) % BUFSIZE;
			curIndex = 0;

			memset(segment, '\0', MSS);
		}
		else
			curIndex = curIndex + 1;

#ifdef GRAN1
	printf("[log] cur index: %d\n", curIndex);
#endif
	}

	close(file);
	close_sock(sock);

	return 0;
}

void *listener(void *arg)
{
	uchar response[HEADSIZE];
	struct sockaddr_in serverCon;
	int bytesRead;

	int sock = * (int *) arg;

	while(1)
	{
		bytesRead = read_from(sock, response, HEADSIZE, &serverCon);

		// timeout after TIMEOUT seconds
		if(bytesRead < 0)
		{
#ifdef APP
	printf("[log] timer expired for sequence number: %d\n", AN);
#endif
			pthread_mutex_lock(&mutex);	
			goBackN(sock);
			pthread_mutex_unlock(&mutex);
		}
		else // received ack
		{
			if(isValid(response)) // check whether correct ack has been received
			{
				// purge frame and slide head pointer
				printWinStats();

				SLIDE_WIN();

				printWinStats();
			}
			else
			{
#ifdef APP
	printf("[log] incorrect header for sequence number: %d\n", AN);
#endif
				pthread_mutex_lock(&mutex);
				goBackN(sock);
				pthread_mutex_unlock(&mutex);
			}
		}
	}
		
}

int isValid(uchar segment[HEADSIZE])
{
	uint ackNo = 0;

	ackNo = (uint) segment[0];
	ackNo = ackNo << 24;

	ackNo = ackNo + (((uint) segment[1]) << 16);
	ackNo = ackNo + (((uint) segment[2]) << 8);
	ackNo = ackNo + ((uint) segment[3]);

#ifdef APP
	printf("[log] received ack for: %d\nexpected: %d\n", ackNo, AN);
#endif

	return ackNo == AN;
}

void goBackN(int sock)
{
	int prevIndex = 0;
	int count = SF;
	uchar segment[MSS];

	for(count = SF; count < SN; count++)
	{
		if(prevIndex == 0 && count > SF) // do not consider first 
		{
			sendSegment(sock, segment, MSS);
#ifdef APP
	printf("[log gobackn] sending segment: %s\n", segment);
#endif

			memset(segment, 0, MSS);
		}

		segment[prevIndex] = buffer[count];
		
		prevIndex = (prevIndex + 1) % MSS;
	}
}

void printWinStats()
{
#ifdef APP
	printf("[log stats]\nWin start index: %d\nWin final index: %d\nNext pending acknowledgement: %d\n[/log stats]\n", SF, SN, AN);
#endif
}
