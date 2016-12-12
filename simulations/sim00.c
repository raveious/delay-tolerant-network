/*
 * sim00.c
 * Spray and Wait
 * 
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h> 
#include <time.h>       

#define SIZE_MAX 50
#define HELLO_WAIT 3
#define START_TIME 2
#define ACK_WAIT 10
#define SEEN_TIME_MAX 50


/*
 *  Message Struct
 * 
 *  int msgID: 0 = Original Source Message
 *  		   1 = Ack reply from Dst that it got message
 *  int msgType: 0 = Simple Hello
 *  int msgType: 1 = Delivery Hello Msg
 *  int msgType: 2 = ACK
 */

struct message {
	int msgID;
	int msgType;
	int srcID;
	int finalDestID;
	int currDestID;
	int currSrcID;
	int numHops;
	
	int nodesFound[SIZE_MAX];
};

struct node {
	int id;
	int status; /* What is my status? Source node: 0 - nothing to send,
				 * 								   1 - Time to Spray */
	int nodesSeen[SIZE_MAX]; /* Last heard nodes, incrementing counter */
	int srcFlag; /* Am I the source node? */
	int dstID; /* Who's the final Destination node I'm trying to contact? */
	int sendPriority; /* If priority message to send, use for sending on next cycle */
	
	int msgToSend[SIZE_MAX]; /* Status flag of messages in buffer, 
							  * 0 no message, 1 low priority, 2 high priority */
	
	int buffExpire[SIZE_MAX]; /* When do outgoing messages expire? (Heard no ACK) */ 
	int msgCopies; /* How many copies to spray */
	
	int x; /* location coordinates on grid */
	int y;
	int z;
	
	int helloTimer; /* How long before I say hello? */
	struct message *inMsg; /* Store incomming message recieved */
	struct message *outMsg; /* Store outgoing message for other to hear */
	struct message helloMsg; /* Hello message to send */
	struct message msgBuff[SIZE_MAX]; /* Buffer of outgoing messages */
	
};


/*
 * Arguments: "Range", "Number of Runs", "Number of Copies", "Number of Nodes", "Grid Width"
 *
 *
 */

int main(int argc, char *argv[]) {				

	int numNodes;
	int simSize;
	int range;
	int totalTrials;
	int i, j, k;
	int done;
	int run;
	int direction;
	int x, y, z, u, v, w;
	int moved;
	int moteIndex;
	int nodesHeard;	
	int startTime = START_TIME;
	int srcCopies;
	int srcID;
	int dstID;	
	int currTime;
	int bufferPtr = 0;

	int testGrid[SIZE_MAX][SIZE_MAX][SIZE_MAX] = {-1};
	int avgNumHops;
	struct node motes[SIZE_MAX];
	
	/* Check number of arg */
	if(argc != 6 ) {
		fprintf(stderr, "Insufficient number of arguments specified. Exiting.\n");
		exit(-1);
	}													
	
	range = atoi( argv[1] );
	totalTrials = atoi( argv[2] );
	srcCopies = atoi( argv[3] );
	numNodes = atoi( argv[4] );
	simSize = atoi( argv[5] );	
	
	if( numNodes < 1 || simSize < 2 || range < 1 || totalTrials < 1 || ((simSize * simSize) <= numNodes) 
			|| (srcCopies < 1) || (srcCopies >= (numNodes )) ){
		fprintf(stderr, "Invalid Arguement. Exiting.\n");
		exit(-1);
	}
	run = 0;
	/* Initialize */	
	while( run < totalTrials ){
		
		srandom( time(NULL) );
		// Initialize Simulation Grid, -1 means no nodes at grid space
		for( i=0; i<simSize; i++ ){
			for( j=0; j<simSize; j++ ){
				for( k=0; k<SIZE_MAX; k++ ){
					testGrid[i][j][k] = -1;
				}					
			}	
		}
		
		// Load nodes across grid, initialize location variables for each node
		k = 0;
		for( i=0; i<simSize && k<numNodes; i=i+2 ){
			for( j=0; j<simSize && k<numNodes; j=j+2 ){
				motes[k].x = i;
				motes[k].y = j;
				motes[k].z = 0;			
				testGrid[i][j][0] = k++;
			}	
		}

		if( k<numNodes ){
		
			for( i=0; i<simSize && k<numNodes; i++ ){
				for( j=0; j<simSize && k<numNodes; j++ ){

					if( (testGrid[i][j][0] == -1) && k<numNodes ){
						motes[k].x = i;
						motes[k].y = j;
						motes[k].z = 0;				
						testGrid[i][j][0] = k++;
					}
				}	
			}		
		
		}		
		
		// Finish Initializing Node Variables
		for( k=0; k<numNodes; k++ ){
			motes[k].helloTimer = random() % HELLO_WAIT;			
			motes[k].srcFlag = -1;	
			motes[k].dstID = -1;	
			motes[k].id = k;
			motes[k].status = 0;
			motes[k].sendPriority = 0;
			motes[k].msgCopies = srcCopies;		
			for( w=0; w<numNodes; w++ ){
				motes[k].nodesSeen[w] = SEEN_TIME_MAX;
				motes[k].msgToSend[w] = 0;
				motes[k].buffExpire[w] = 0;
				for( u=0; u<numNodes; u++ ){
					motes[k].msgBuff[w].nodesFound[u] = -1;
					
				}	
			}
			
		}		

		
		// Random selection of Source and Destination nodes
		// Initialize Source node with number of copies 
		srcID = random() % numNodes;
		motes[srcID].srcFlag = srcID;
		do{
			dstID = random() % numNodes;
		} while( srcID == dstID );		
		motes[srcID].dstID = dstID;
		
		currTime = 0;
		done = 0;
		while( !done ){
			
			/* Talk */
			for( k=0; k<numNodes; k++ ){
				nodesHeard = 0;
				/* Listen within Range*/
				/* MAC protocol - Carrier Sense */
				for( i=(motes[k].x - range); i<(range*2+1); i++ ){
					for( j=(motes[k].y - range); j<(range*2+1); j++ ){
						if( (i < simSize) && (i >= 0) && (j < simSize) && (j >= 0) ){
							w = 0;
							while( testGrid[i][j][w] != -1 ){
								if( (testGrid[i][j][w] != k) && (motes[ testGrid[i][j][w] ].outMsg != 0) ){
									motes[k].inMsg = motes[ testGrid[i][j][w] ].outMsg;
									nodesHeard++;
								}	
								w++;
							}
							
						}
					}
				}
				if( nodesHeard != 1 ){
					motes[k].inMsg = 0;
				}
				/* Review Any Recieved messages OR Send any Pending message */
				/************* BEGIN ROUTING PROTOCOL *************/
				motes[k].outMsg = 0;
				
				if( motes[k].srcFlag != -1 ){
						
					/* Find free space in buffer for storing messages */
					for( i=0; i<numNodes; i++ ){
						if(  motes[k].msgToSend[i] == 0 ){
							bufferPtr = i;
							i = numNodes;
						}
					}
					/* If no free space, replace low priority message */
					if(  motes[k].msgToSend[bufferPtr] != 0 ){
						for( i=0;i<numNodes; i++ ){
							if(  motes[k].msgToSend[i] != 2 ){
								bufferPtr = i;
								i = numNodes;
							}
						}
					}			
						
					if( startTime == currTime ){
						motes[k].status = 1;
					}			
					/* Recieved Message */	
					if( motes[k].inMsg != 0 ){
						/* Source node acks as ordinary node when status is 0, 
						 * until has message ready, status = 1 */
						if( motes[k].status == 1 ){
							motes[k].nodesSeen[ motes[k].inMsg->currSrcID ] = 0;
							/* Recieved Simple 'I am here' message */
							if( motes[k].inMsg->msgType == 0 ){
								
								if( motes[k].inMsg->currSrcID == motes[k].dstID ){
									motes[k].msgBuff[ bufferPtr ].msgID = 1;
									motes[k].msgBuff[ bufferPtr ].msgType = 1;
									motes[k].msgBuff[ bufferPtr ].srcID = k;
									motes[k].msgBuff[ bufferPtr ].finalDestID = motes[k].dstID;
									motes[k].msgBuff[ bufferPtr ].currSrcID = k;
									motes[k].msgBuff[ bufferPtr ].currDestID = motes[k].dstID;
									motes[k].msgBuff[ bufferPtr ].numHops = 1;
									motes[k].msgBuff[ bufferPtr ].nodesFound[0] = k;
									motes[k].msgBuff[ bufferPtr ].nodesFound[1] = motes[k].dstID;
									motes[k].msgToSend[ bufferPtr ] = 2;
								}
								else{
									if( motes[k].msgCopies != 0 ){			
										motes[k].msgBuff[ bufferPtr ].msgID = 1;
										motes[k].msgBuff[ bufferPtr ].msgType = 1;
										motes[k].msgBuff[ bufferPtr ].srcID = k;
										motes[k].msgBuff[ bufferPtr ].finalDestID = motes[k].dstID;
										motes[k].msgBuff[ bufferPtr ].currSrcID = k;
										motes[k].msgBuff[ bufferPtr ].currDestID = motes[k].inMsg->currSrcID;
										motes[k].msgBuff[ bufferPtr ].numHops = 1;
										motes[k].msgBuff[ bufferPtr ].nodesFound[0] = k;
										motes[k].msgToSend[ bufferPtr ] = 1;
										
									}
								}
							}
							/* Recieved Hello Delivery */
							else if( motes[k].inMsg->msgType == 1 ){

								if( (motes[k].inMsg->msgID == 2) && (motes[k].inMsg->currSrcID == motes[k].dstID) ){
									printf("Src got dst's ack from dst\n");
									done = 1;
								}
								else if( (motes[k].inMsg->msgID == 2) ){
									printf("Src got dst's ack msg from relay\n");
									done = 1;
								}
								else{
									/* Message was same as was sent from source, ignore */
								}
							
							}
							/* if motes[k].inMsg->msgType == 2 */
							else {
								for( i=0; i<numNodes; i++){
									if( (motes[k].msgToSend)[ i ] > 0 ){
										if( (motes[k].msgBuff)[ i ].currDestID == motes[k].inMsg->currSrcID ){
											if( motes[k].inMsg->currSrcID == motes[k].dstID ){
												printf("Messaging done, src got ack from dst\n");
												done = 1;
											}
											else{
												motes[k].msgCopies--;
												(motes[k].msgToSend)[ i ] = 0;
												i = numNodes;
												motes[k].buffExpire[ i ] = 0;
											}
										}
									}
								}
							}
						}
					}
					else{
						
						bufferPtr = -1;
						
						for( i=0,j=0; i<numNodes; i++ ){
							
							if( j < 1 && ((motes[k].msgToSend)[ i ] == 1) 
									&& ( motes[k].buffExpire[ i ] >= 0 ) ){
								j++;
								bufferPtr = i;
							}
							if( (motes[k].msgToSend)[ i ] == 2 ){
								bufferPtr = i;
								i = numNodes;
							}
						}
						if( (motes[k].helloTimer == 0) && (bufferPtr == -1) ){
							
							if( motes[k].status == 0 ){
								motes[k].helloMsg.msgID = 0;
								motes[k].helloMsg.msgType = 0;
								motes[k].helloMsg.srcID = -1;
								motes[k].helloMsg.finalDestID = -1;
								motes[k].helloMsg.currSrcID = k;
								motes[k].helloMsg.currDestID = -1;
								motes[k].helloMsg.numHops = -1;	
							}
							else{
								motes[k].helloMsg.msgID = 1;
								motes[k].helloMsg.msgType = 1;
								motes[k].helloMsg.srcID = k;
								motes[k].helloMsg.finalDestID = motes[k].dstID;
								motes[k].helloMsg.currSrcID = k;
								motes[k].helloMsg.currDestID = -1;
								motes[k].helloMsg.numHops = 0;	
							}
							motes[k].outMsg = &motes[k].helloMsg;
							
						}
						if( bufferPtr > -1 ){
							motes[k].outMsg = &motes[k].msgBuff[ bufferPtr ];
							motes[k].buffExpire[ bufferPtr ] = ACK_WAIT;
					
						}
					}	
				}
				else{
					
					for( i=0; i<numNodes; i++ ){
						if( motes[k].msgToSend[i] == 0 ){
							bufferPtr = i;
							i = numNodes;
						}
					}
					if( motes[k].msgToSend[bufferPtr] != 0 ){
						for( i=0;i<numNodes; i++ ){
							if( motes[k].msgToSend[i] != 2 ){
								bufferPtr = i;
								i = numNodes;
							}
						}
					}		
					/*  */
					if( motes[k].inMsg != 0 ){
						/* Dst node recieves message addressed to it, sets status to 1 */
						if( (motes[k].status == 0) && (motes[k].inMsg->finalDestID == k) ){
							motes[k].dstID = motes[k].inMsg->srcID;
							motes[k].status == 1;
							printf("Got message at 399\n");
							done = 1;
						}
						/* Block used by Dst node when acking SRC */
						if( (motes[k].status == 1) ){
							motes[k].nodesSeen[ motes[k].inMsg->currSrcID ] = 0;
							if( motes[k].inMsg->currSrcID == motes[k].dstID ){
								motes[k].msgBuff[ bufferPtr ].msgID = 2;
								motes[k].msgBuff[ bufferPtr ].msgType = 2;
								motes[k].msgBuff[ bufferPtr ].srcID = k;
								motes[k].msgBuff[ bufferPtr ].finalDestID = motes[k].dstID;
								motes[k].msgBuff[ bufferPtr ].currSrcID = k;
								motes[k].msgBuff[ bufferPtr ].currDestID = motes[k].dstID;
								motes[k].msgBuff[ bufferPtr ].numHops = 1;
								motes[k].msgBuff[ bufferPtr ].nodesFound[0] = k;
								motes[k].msgBuff[ bufferPtr ].nodesFound[1] = motes[k].dstID;
								motes[k].msgToSend[ bufferPtr ] = 2;
							}
							/* Got Hello from Relay */
							else if( motes[k].inMsg->msgType == 0 ){
								if( motes[k].msgCopies != 0 ){			
									motes[k].msgBuff[ bufferPtr ].msgID = 2;
									motes[k].msgBuff[ bufferPtr ].msgType = 1;
									motes[k].msgBuff[ bufferPtr ].srcID = k;
									motes[k].msgBuff[ bufferPtr ].finalDestID = motes[k].dstID;
									motes[k].msgBuff[ bufferPtr ].currSrcID = k;
									motes[k].msgBuff[ bufferPtr ].currDestID = motes[k].inMsg->currSrcID;
									motes[k].msgBuff[ bufferPtr ].numHops = 1;
									motes[k].msgBuff[ bufferPtr ].nodesFound[0] = k;
									motes[k].msgToSend[ bufferPtr ] = 1;
									
								}					
							}
							/* Relay delivering message for me */
							else if( (motes[k].inMsg->msgID == 1) && (motes[k].inMsg->finalDestID == k) ){
								motes[k].msgBuff[ bufferPtr ].msgID = 1;
								motes[k].msgBuff[ bufferPtr ].msgType = 2;
								motes[k].msgBuff[ bufferPtr ].srcID = motes[k].dstID;
								motes[k].msgBuff[ bufferPtr ].finalDestID = k;
								motes[k].msgBuff[ bufferPtr ].currSrcID = k;
								motes[k].msgBuff[ bufferPtr ].currDestID = motes[k].inMsg->currSrcID;
								motes[k].msgBuff[ bufferPtr ].numHops = 0;
								motes[k].msgBuff[ bufferPtr ].nodesFound[0] = k;
								motes[k].msgToSend[ bufferPtr ] = 1;					
							}
							/* ACK from relay, it will pass message on */
							else if( (motes[k].inMsg->msgType == 2) && (motes[k].inMsg->currDestID == k) && (motes[k].inMsg->msgID == 2) ){
								for( i=0; i<numNodes; i++){
									if( (motes[k].msgToSend)[ i ] > 0 ){
										if( (motes[k].msgBuff)[ i ].currDestID == motes[k].inMsg->currSrcID ){
											if( motes[k].inMsg->currSrcID == motes[k].dstID ){
												printf("Got message at 450\n");
												done = 1;
											}
											else{
												motes[k].msgCopies--;
												(motes[k].msgToSend)[ i ] = 0;
												i = numNodes;
												motes[k].buffExpire[ i ] = 0;
											}
										}
									}
								}
							}
							else{
								/* Message had ID of 2 from relay, ignore, 
								 * it's a copy of what I already sent out */
							}
							
						}
						else{
							motes[k].nodesSeen[ motes[k].inMsg->currSrcID ] = 0;
							if( motes[k].inMsg->msgType == 0 ){
								/* If hello sent from destination node, say hello back sooner */
								if( (motes[k].msgToSend)[ 0 ] == 2 ){
									if( (motes[k].msgBuff)[ 0 ].finalDestID == motes[k].inMsg->currSrcID  ){
										motes[k].sendPriority = 1;	
									}
								}

							}
							/* If message is a relay message */
							else if( motes[k].inMsg->msgType == 1 ){
								/* Relay message from a source and not a relay */
								if( (motes[k].msgToSend)[ 0 ] == 2 && (motes[k].inMsg->srcID == motes[k].inMsg->currSrcID) ){
									/* If destination node is ACKING back to source, replace relay message with new */
									if( motes[k].msgBuff[ 0 ].msgID < motes[k].inMsg->msgID  ){
										motes[k].msgBuff[ 0 ].msgID = motes[k].inMsg->msgID;
										motes[k].msgBuff[ 0 ].msgType = 1;
										motes[k].msgBuff[ 0 ].srcID = motes[k].inMsg->srcID;
										motes[k].msgBuff[ 0 ].finalDestID = motes[k].inMsg->finalDestID;
										motes[k].msgBuff[ 0 ].currSrcID = k;
										motes[k].msgBuff[ 0 ].currDestID = motes[k].inMsg->finalDestID;
										motes[k].msgBuff[ 0 ].numHops = motes[k].inMsg->numHops + 1;
										motes[k].msgBuff[ 0 ].nodesFound[0] = k;
										motes[k].msgToSend[ 0 ] = 2;							
									}
								}
								/* I don't have current relay message, add new message to deliver */
								else if( motes[k].inMsg->srcID == motes[k].inMsg->currSrcID ){
										/* Build hello relay message */
										motes[k].msgBuff[ 0 ].msgID = motes[k].inMsg->msgID;
										motes[k].msgBuff[ 0 ].msgType = 1;
										motes[k].msgBuff[ 0 ].srcID = motes[k].inMsg->srcID;
										motes[k].msgBuff[ 0 ].finalDestID = motes[k].inMsg->finalDestID;
										motes[k].msgBuff[ 0 ].currSrcID = k;
										motes[k].msgBuff[ 0 ].currDestID = motes[k].inMsg->finalDestID;
										motes[k].msgBuff[ 0 ].numHops = motes[k].inMsg->numHops + 1;
										motes[k].msgBuff[ 0 ].nodesFound[1] = k;
										motes[k].msgToSend[ 0 ] = 2;		
										/*  Built ACK to let source know I got message */
										motes[k].msgBuff[ 1 ].msgID = motes[k].inMsg->msgID;
										motes[k].msgBuff[ 1 ].msgType = 2;
										motes[k].msgBuff[ 1 ].srcID = motes[k].inMsg->srcID;
										motes[k].msgBuff[ 1 ].finalDestID = motes[k].inMsg->finalDestID;
										motes[k].msgBuff[ 1 ].currSrcID = k;
										motes[k].msgBuff[ 1 ].currDestID = motes[k].inMsg->srcID;
										motes[k].msgBuff[ 1 ].numHops = 1;
										motes[k].msgBuff[ 1 ].nodesFound[0] = k;
										motes[k].msgToSend[ 1 ] = 2;	
								}
								else{
									/* Message recieved was from another relay node, ignore */
								}	
							}
							/* Got ACK from destination node, remove relay message from buffer */
							else{
								if( motes[k].inMsg->currDestID == motes[k].dstID ){
									(motes[k].msgToSend)[ i ] = 0;
								}
								
							}
						}
						
						
					}
					else{
						if( motes[k].status == 1 ){
							bufferPtr = 0;
							
							for( i=0,j=0; i<numNodes; i++ ){
								
								if( j < 1 && ((motes[k].msgToSend)[ i ] == 1) 
										&& ( motes[k].buffExpire[ bufferPtr ] == 0 ) ){
									j++;
									bufferPtr = i;
								}
								if( (motes[k].msgToSend)[ i ] == 2 ){
									bufferPtr = i;
									i = numNodes;
								}
							}
							if( (motes[k].helloTimer == 0) && (bufferPtr == 0) ){
								motes[k].helloMsg.msgID = 0;
								motes[k].helloMsg.msgType = 0;
								motes[k].helloMsg.srcID = -1;
								motes[k].helloMsg.finalDestID = -1;
								motes[k].helloMsg.currSrcID = k;
								motes[k].helloMsg.currDestID = -1;
								motes[k].helloMsg.numHops = -1;	

								motes[k].outMsg = &motes[k].helloMsg;		
							}
							if( bufferPtr != 0 ){
								motes[k].outMsg = &motes[k].msgBuff[ bufferPtr ];
								motes[k].buffExpire[ bufferPtr ] = ACK_WAIT;

							}
						}
						else{
							if( (motes[k].helloTimer == 0) && (((motes[k].msgToSend)[ 0 ] == 0) 
									|| ((motes[k].msgToSend)[ 1 ] == 0)) ){
								motes[k].helloMsg.msgID = 0;
								motes[k].helloMsg.msgType = 0;
								motes[k].helloMsg.srcID = -1;
								motes[k].helloMsg.finalDestID = -1;
								motes[k].helloMsg.currSrcID = k;
								motes[k].helloMsg.currDestID = -1;
								motes[k].helloMsg.numHops = -1;	

								motes[k].outMsg = &motes[k].helloMsg;		

							}
							else if( (motes[k].msgToSend)[ 1 ] != 0 ){
								(motes[k].outMsg)->msgID = motes[k].msgBuff[ 1 ].msgID;
								(motes[k].outMsg)->msgType = (motes[k].msgBuff)[ 1 ].msgType;
								(motes[k].outMsg)->srcID = (motes[k].msgBuff)[ 1 ].srcID;
								(motes[k].outMsg)->finalDestID = (motes[k].msgBuff)[ 1 ].finalDestID;
								(motes[k].outMsg)->currSrcID = (motes[k].msgBuff)[ 1 ].currSrcID;
								(motes[k].outMsg)->currDestID = (motes[k].msgBuff)[ 1 ].currDestID;
								(motes[k].outMsg)->numHops = (motes[k].msgBuff)[ 1 ].numHops;			
								motes[k].sendPriority = 1;				
							}
							else{
								if( (motes[k].msgToSend)[ 0 ] != 0 ){
									(motes[k].outMsg)->msgID = (motes[k].msgBuff)[ 0 ].msgID;
									(motes[k].outMsg)->msgType = (motes[k].msgBuff)[ 0 ].msgType;
									(motes[k].outMsg)->srcID = (motes[k].msgBuff)[ 0 ].srcID;
									(motes[k].outMsg)->finalDestID = (motes[k].msgBuff)[ 0 ].finalDestID;
									(motes[k].outMsg)->currSrcID = (motes[k].msgBuff)[ 0 ].currSrcID;
									(motes[k].outMsg)->currDestID = (motes[k].msgBuff)[ 0 ].currDestID;
									(motes[k].outMsg)->numHops = (motes[k].msgBuff)[ 0 ].numHops;
									i=0;
									while( ((motes[k].msgBuff)[ 0 ].nodesFound[i] != -1) && i<numNodes ){
										((motes[k].outMsg)->nodesFound)[i] = (motes[k].msgBuff)[ 0 ].nodesFound[i];
									}
								
								}

							}
						}
					}	
				}	
				/* Decrement timer for sending out next hello */
				if( motes[k].sendPriority == 1 ){
					motes[k].helloTimer = 0;
					motes[k].sendPriority = 0;
				}
				else{
					if( motes[k].helloTimer == 0 ){
						motes[k].helloTimer = HELLO_WAIT;
					}
					else{
						motes[k].helloTimer--;
					}		
				}
					
				
				for( i=0; i<numNodes; i++ ){
					
					if( motes[k].nodesSeen[ i ] < SEEN_TIME_MAX ){
						motes[k].nodesSeen[ i ]++;
					}
						
					if( motes[k].buffExpire[ i ] > 0 ){
						motes[k].buffExpire[ i ]--;
					}	
				}
				motes[k].inMsg = 0;
				
				/************* END OF PROTOCOL *************/
				if( done == 1 ){
					k = numNodes;
				}
			}

			/* Walk */
			for( k=0; k<numNodes; k++ ){
				moved = 0;
				
				do{
					x = motes[k].x;
					y = motes[k].y;
					z = motes[k].z;
					u = x;
					v = y;
					w = z;
					direction = random() % 4;
					
					if( direction == 0 ){
						x--;
					}
					else if( direction == 1 ){
						y--;
					}
					else if( direction == 2 ){
						x++;
					}
					else{
						y++;
					}

					if( (x < simSize) && (x >= 0) && (y < simSize) && (y >= 0) ){
						i = 0;						
						while( testGrid[x][y][i] > -1 ){
							i++;
						}
						
						motes[k].x = x;
						motes[k].y = y;
						motes[k].z = i;
						testGrid[x][y][i] = k;
						
						if( w != (numNodes - 1) ){
							moteIndex = testGrid[u][v][w+1];
							
							while( (moteIndex != -1) && (w != (numNodes - 1) ) ){
								motes[ moteIndex ].z = w;		
								testGrid[u][v][w] = moteIndex;
								w++;
								if( w+1 < numNodes ){
									moteIndex = testGrid[u][v][w+1];
								}	
							}
							testGrid[u][v][w] = -1;
						}
						else{
							testGrid[u][v][w] = -1;
						}

						moved = 1;
					}
					
				}while( !moved );
				
			}	
			
			for( i=0; i<simSize; i++ ){
				for( j=0; j<simSize; j++ ){
					
					printf("%d ", testGrid[i][j][0]);	
					
				}
				printf("\n\n");
			
			}
			for( i=0; i<numNodes; i++ ){
				printf( "Node: %d, x: %d, y: %d, z: %d\n", i, motes[i].x, motes[i].y, motes[i].z);
			}			

			currTime++;
		}
		run++;		
		
	}
	return 0;
}
