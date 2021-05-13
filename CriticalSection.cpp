#include<windows.h>
#include<windowsx.h>
#include<process.h>
#include <time.h>
#include<iostream>
#include<conio.h>
using namespace std;
#define DATA_SIZE 256

typedef struct msg_block_tag { /* Message block */
	volatile DWORD		f_ready, f_stop;
	/* ready state flag, stop flag	*/
	volatile DWORD sequence; /* Message block sequence number */
	volatile DWORD nCons, nLost;
	//time_t timestamp;
	CRITICAL_SECTION mguard;	/* Guard the message block structure */
	DWORD	checksum; /* Message contents checksum		*/
	DWORD	data[DATA_SIZE]; /* Message Contents		*/
} MSG_BLOCK;
/*	One of the following conditions holds for the message block 	*/
/*	  1)	!f_ready || f_stop				*/
/*		nothing is assured about the data OR		*/
/*	  2)	f_ready && data is valid			*/
/*			 && checksum and timestamp are valid	*/
/*  Also, at all times, 0 <= nLost + nCons <= sequence		*/

/* Single message block, ready to fill with a new message 	*/
MSG_BLOCK mblock = { 0, 0, 0, 0, 0 };

static unsigned __stdcall produce(void*);
static unsigned __stdcall produce1(void*);
static unsigned __stdcall produce2(void*);
static unsigned __stdcall produce3(void*);
static unsigned __stdcall consume(void*);
void MessageFill(MSG_BLOCK*);
void MessageDisplay(MSG_BLOCK*);

int main()
{
	DWORD Status;
	UINT ThId;
	HANDLE produce_h, consume_h, produce1_h, produce2_h, produce3_h;

	/* Initialize the message block CRITICAL SECTION */
	InitializeCriticalSection(&mblock.mguard);

	/* Create the two threads */
	produce_h = (HANDLE)_beginthreadex(NULL, 0, produce, NULL, 0, &ThId);
	if (produce_h == NULL)
		cout << "Cannot create producer thread" << endl;
	consume_h = (HANDLE)_beginthreadex(NULL, 0, consume, NULL, 0, &ThId);
	if (consume_h == NULL)
		cout << "Cannot create consume thread" << endl;
	produce1_h = (HANDLE)_beginthreadex(NULL, 0, produce1, NULL, 0, &ThId);
	if (produce1_h == NULL)
		cout << "Cannot create producer thread#1" << endl;
	produce2_h = (HANDLE)_beginthreadex(NULL, 0, produce1, NULL, 0, &ThId);
	if (produce1_h == NULL)
		cout << "Cannot create producer thread#2" << endl;
	produce3_h = (HANDLE)_beginthreadex(NULL, 0, produce1, NULL, 0, &ThId);
	if (produce1_h == NULL)
		cout << "Cannot create producer thread#3" << endl;


	/* Wait for the producer and consumer to complete */

	Status = WaitForSingleObject(consume_h, INFINITE);
	if (Status != WAIT_OBJECT_0)
		cout << "Failed waiting for consumer thread" << endl;
	Status = WaitForSingleObject(produce_h, INFINITE);
	if (Status != WAIT_OBJECT_0)
		cout << "Failed waiting for produser threads" << endl;
	Status = WaitForSingleObject(produce1_h, INFINITE);
	if (Status != WAIT_OBJECT_0)
		cout << "Failed waiting for produser thread#1" << endl;
	Status = WaitForSingleObject(produce2_h, INFINITE);
	if (Status != WAIT_OBJECT_0)
		cout << "Failed waiting for produser thread#2" << endl;
	Status = WaitForSingleObject(produce3_h, INFINITE);
	if (Status != WAIT_OBJECT_0)
		cout << "Failed waiting for produser thread#3" << endl;


	DeleteCriticalSection(&mblock.mguard);

	cout << "Producer and consumer threads have terminated\n";
	cout << "Messages produced: " << mblock.sequence << " Consumed: " << mblock.nCons << " Known Lost: " << mblock.nLost << " " << endl;
	_getch();
	return 0;
}

unsigned __stdcall produce(void* arg)
/* Producer thread - Create new messages at random intervals */
{
	srand((DWORD)time(NULL)); /* Seed the random # generator 	*/
	while (!mblock.f_stop) {
		/* Random Delay */
		Sleep(rand() / 100);

		/* Get the buffer, fill it */

		EnterCriticalSection(&mblock.mguard);
		__try {
			if (!mblock.f_stop) {
				mblock.f_ready = 0;
				MessageFill(&mblock);
				mblock.f_ready = 1;
				mblock.sequence++;
			}
		}
		__finally { LeaveCriticalSection(&mblock.mguard); }
	}
	return 0;
}
unsigned __stdcall produce1(void* arg)
/* Producer thread - Create new messages at random intervals */
{
	srand((DWORD)time(NULL)); /* Seed the random # generator 	*/
	while (!mblock.f_stop) {
		/* Random Delay */
		Sleep(rand() / 100);

		/* Get the buffer, fill it */

		EnterCriticalSection(&mblock.mguard);
		__try {
			if (!mblock.f_stop) {
				mblock.f_ready = 0;
				MessageFill(&mblock);
				mblock.f_ready = 1;
				mblock.sequence++;
			}
		}
		__finally { LeaveCriticalSection(&mblock.mguard); }
	}
	return 0;
}

unsigned __stdcall produce2(void* arg)
/* Producer thread - Create new messages at random intervals */
{
	srand((DWORD)time(NULL)); /* Seed the random # generator 	*/
	while (!mblock.f_stop) {
		/* Random Delay */
		Sleep(rand() / 100);

		/* Get the buffer, fill it */

		EnterCriticalSection(&mblock.mguard);
		__try {
			if (!mblock.f_stop) {
				mblock.f_ready = 0;
				MessageFill(&mblock);
				mblock.f_ready = 1;
				mblock.sequence++;
			}
		}
		__finally { LeaveCriticalSection(&mblock.mguard); }
	}
	return 0;
}

unsigned __stdcall produce3(void* arg)
/* Producer thread - Create new messages at random intervals */
{
	srand((DWORD)time(NULL)); /* Seed the random # generator 	*/
	while (!mblock.f_stop) {
		/* Random Delay */
		Sleep(rand() / 100);

		/* Get the buffer, fill it */

		EnterCriticalSection(&mblock.mguard);
		__try {
			if (!mblock.f_stop) {
				mblock.f_ready = 0;
				MessageFill(&mblock);
				mblock.f_ready = 1;
				mblock.sequence++;
			}
		}
		__finally { LeaveCriticalSection(&mblock.mguard); }
	}
	return 0;
}
unsigned __stdcall consume(void* arg)
{
	DWORD ShutDown = 0;
	CHAR command;
	/* Consume the NEXT message when prompted by the user */
	while (!ShutDown) { /* This is the only thread accessing stdin, stdout */
		cout << endl << "**Enter 'c' for consume; 's' to stop: " << endl;
		cin >> command;//>>extra;
		if (command == 's') {
			EnterCriticalSection(&mblock.mguard);
			ShutDown = mblock.f_stop = 1;
			LeaveCriticalSection(&mblock.mguard);
		}
		else if (command == 'c') { /* Get a new buffer to consume */
			EnterCriticalSection(&mblock.mguard);
			__try {
				if (mblock.f_ready == 0)
					cout << "No new messages. Try again later" << endl;
				else {
					MessageDisplay(&mblock);
					mblock.nCons++;
					mblock.nLost = mblock.sequence - mblock.nCons;
					mblock.f_ready = 0; /* No new messages are ready */
				}
			}
			__finally { LeaveCriticalSection(&mblock.mguard); }
		}
		else {
			cout << "Illegal command. Try again." << endl;
		}
	}
	return 0;
}

void MessageFill(MSG_BLOCK* mblock)
{
	/* Fill the message buffer, and include checksum and timestamp	*/
	/* This function is called from the producer thread while it 	*/
	/* owns the message block mutex					*/

	DWORD i;

	mblock->checksum = 0;
	for (i = 0; i < DATA_SIZE; i++) {
		mblock->data[i] = rand();
		mblock->checksum ^= mblock->data[i];
	}
	//mblock->timestamp = time(NULL);
	return;
}

void MessageDisplay(MSG_BLOCK* mblock)
{
	/* Display message buffer, timestamp, and validate checksum	*/
	/* This function is called from the consumer thread while it */
	/* owns the message block mutex	*/
	DWORD i, tcheck = 0;

	for (i = 0; i < DATA_SIZE; i++)
		tcheck ^= mblock->data[i];
	cout << endl << "Message number " << mblock->sequence << " generated at: " << GetTickCount()/*(&(mblock->timestamp))*/ << endl;

	cout << "First and last entries: " << mblock->data[0] << " " << mblock->data[DATA_SIZE - 1] << endl;
	if (tcheck == mblock->checksum)
		cout << "GOOD ->Checksum was validated." << endl;
	else
		cout << "BAD  ->Checksum failed. message was corrupted." << endl;

	return;
}
