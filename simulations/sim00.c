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
	int numCopies;
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
			/* Initialize its I am here message */
			motes[k].helloMsg.msgID = 0;
			motes[k].helloMsg.msgType = 0;
			motes[k].helloMsg.srcID = -1;
			motes[k].helloMsg.finalDestID = -1;
			motes[k].helloMsg.currSrcID = k;
			motes[k].helloMsg.currDestID = -1;
			motes[k].helloMsg.numHops = -1;
			motes[k].helloMsg.numCopies = -1;			
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
	int numCopies;
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
				motes[k].buffExpire[w] = 0;
				for( u=0; u<numNodes; u++ ){
					motes[k].msgBuff[w].nodesFound[u] = -1;
					
				}	
			}
			/* Initialize its I am here message */
			motes[k].helloMsg.msgID = 0;
			motes[k].helloMsg.msgType = 0;
			motes[k].helloMsg.srcID = -1;
			motes[k].helloMsg.finalDestID = -1;
			motes[k].helloMsg.currSrcID = k;
			motes[k].helloMsg.currDestID = -1;
			motes[k].helloMsg.numHops = -1;
			motes[k].helloMsg.numCopies = -1;			
			
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
				
				/* Block executes if node is source */
				if( motes[k].srcFlag != -1 ){
						
		
					/* Once startTime occurs, Source goes into Spray mode */
					if( startTime == currTime ){
						motes[k].status = 1;
					}			
					/* Recieved Message */	
					if( motes[k].inMsg != 0 ){
						/* Source node acts as ordinary node when status is 0, 
						 * until has message ready, status = 1 */
						if( motes[k].status == 1 ){
							motes[k].nodesSeen[ motes[k].inMsg->currSrcID ] = 0;
							/* Recieved Simple 'I am here' message */
							if( motes[k].inMsg->msgType == 0 ){
								/* Heard node, clear timer to send hello sooner */
								motes[k].sendPriority = 1;
								
							}
							/* Recieved Delivery Msg */
							else if( motes[k].inMsg->msgType == 1 ){

								if( (motes[k].inMsg->msgID == 2) && (motes[k].inMsg->currSrcID == motes[k].dstID) ){
									printf("Src got dst's ack from dst, Line 252\n");
									done = 1;
								}
								else if( (motes[k].inMsg->msgID == 2) ){
									printf("Src got dst's ack msg from relay, 256\n");
									done = 1;
								}
								else{
									/* Message was same as was sent from source, ignore */
								}
							
							}
							/* if motes[k].inMsg->msgType == 2, an ACK */
							else{
								if( motes[k].inMsg->currDestID == k ){
									if( motes[k].inMsg->currSrcID == motes[k].dstID ){
										printf("Messaging done, src got ack from dst, 268\n");
										done = 1;
									}
									/* ACK was from relay that heard message */
									else{
										motes[k].msgCopies--;
									}
								}	
								
							}
						}
					}
					else{
						
						if( motes[k].helloTimer == 0 ){
							
							if( motes[k].status == 1 ){
								motes[k].helloMsg.msgID = 1;
								motes[k].helloMsg.msgType = 1;
								motes[k].helloMsg.srcID = k;
								motes[k].helloMsg.finalDestID = motes[k].dstID;
								motes[k].helloMsg.currSrcID = k;
								motes[k].helloMsg.currDestID = -1;
								motes[k].helloMsg.numCopies = motes[k].msgCopies;
								motes[k].helloMsg.numHops = 0;	
							}
							motes[k].outMsg = &motes[k].helloMsg;
							
						}

					}	
				}
				/* Block of code for everyone else */
				else{

					bufferPtr = -1;
					u = ACK_WAIT + 1;
					for( i=0,j=0; i<numNodes; i++ ){
						if( j < 1 && ( motes[k].buffExpire[ i ] == 0 ) ){
							j++;
							bufferPtr = i;
							i = numNodes;
						}				
						if( (motes[k].buffExpire[ i ] > 0) 
								&& motes[k].buffExpire[ i ] < u ){
							bufferPtr = i;
							u = motes[k].buffExpire[ i ];
						}
					}								
					
					/* I receieved Message */
					if( motes[k].inMsg != 0 ){
						/* Dst node recieves message addressed to it, sets status to 1 */
						if( (motes[k].status == 0) && (motes[k].inMsg->finalDestID == k) ){
							motes[k].dstID = motes[k].inMsg->srcID;
							motes[k].status == 1;
							if( motes[k].inMsg->currSrcID == motes[k].inMsg->srcID )
								printf("Got message at 325 from Source \n");
							else
								printf("Got message at 325 from Relay Node: \n%d", motes[k].inMsg->currSrcID );
														
							//done = 1;
						}
						/* Code block used by Dst node when acking SRC */
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
								motes[k].buffExpire[ bufferPtr ] = ACK_WAIT;
							}
							/* Got Hello from Relay */
							else if( motes[k].inMsg->msgType == 0 ){
								motes[k].sendPriority = 1;	
							}
							/* Relay delivering message to me */
							else if( (motes[k].inMsg->msgID == 1) && (motes[k].inMsg->finalDestID == k) ){
								motes[k].msgBuff[ bufferPtr ].msgID = 1;
								motes[k].msgBuff[ bufferPtr ].msgType = 2;
								motes[k].msgBuff[ bufferPtr ].srcID = motes[k].dstID;
								motes[k].msgBuff[ bufferPtr ].finalDestID = k;
								motes[k].msgBuff[ bufferPtr ].currSrcID = k;
								motes[k].msgBuff[ bufferPtr ].currDestID = motes[k].inMsg->currSrcID;
								motes[k].msgBuff[ bufferPtr ].numHops = 0;
								motes[k].msgBuff[ bufferPtr ].nodesFound[0] = k;
								motes[k].buffExpire[ bufferPtr ] = ACK_WAIT;					
							}
							/* ACK from relay or Src */
							else if( (motes[k].inMsg->msgType == 2) && (motes[k].inMsg->currDestID == k) 
									&& (motes[k].inMsg->msgID == 2) ){

								/* Src got ACK from Dst, know DST got its original message */
								if( motes[k].inMsg->currSrcID == motes[k].dstID ){
									printf("Test done, dst got ack from source, 280\n");
									done = 1;
								}
								/* ACK was from relay that heard message */
								else{
									motes[k].msgCopies--;
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
								if( motes[k].helloMsg.finalDestID == motes[k].inMsg->currSrcID  ){
									motes[k].sendPriority = 1;	
								}
							}
							/* If message is a relay message from eithe source or relay */
							else if( motes[k].inMsg->msgType == 1 ){
								/* Relay message from a source and not a relay */
								if( motes[k].inMsg->srcID == motes[k].inMsg->currSrcID ){
									/* Change I'm Here Message to Relay message*/
									if( motes[k].helloMsg.msgID < motes[k].inMsg->msgID 
											&& motes[k].inMsg->numCopies > 0 ){
										motes[k].helloMsg.msgID = motes[k].inMsg->msgID;
										motes[k].helloMsg.msgType = 1;
										motes[k].helloMsg.srcID = motes[k].inMsg->srcID;
										motes[k].helloMsg.finalDestID = motes[k].inMsg->finalDestID;
										motes[k].helloMsg.currSrcID = k;
										motes[k].helloMsg.currDestID = motes[k].inMsg->finalDestID;
										motes[k].helloMsg.numCopies = -1;
										motes[k].helloMsg.numHops = motes[k].inMsg->numHops + 1;;		
										motes[k].helloMsg.nodesFound[1] = k;		

										motes[k].msgBuff[ bufferPtr ].msgID = motes[k].inMsg->msgID;
										motes[k].msgBuff[ bufferPtr ].msgType = 2;
										motes[k].msgBuff[ bufferPtr ].srcID = motes[k].inMsg->srcID;
										motes[k].msgBuff[ bufferPtr ].finalDestID = motes[k].inMsg->finalDestID;
										motes[k].msgBuff[ bufferPtr ].currSrcID = k;
										motes[k].msgBuff[ bufferPtr ].currDestID = motes[k].inMsg->srcID;
										motes[k].msgBuff[ bufferPtr ].numHops = 0;
										motes[k].msgBuff[ bufferPtr ].nodesFound[0] = k;
										motes[k].buffExpire[ bufferPtr ] = ACK_WAIT;
										motes[k].outMsg = &motes[k].msgBuff[ bufferPtr ];
									}
								}
								/* I don't have current relay message, add new message to deliver */
								else{
									/* Message recieved was from another relay node, ignore */
								}	
							}
							/* Got ACK from destination node, got back to saying I'm Here Hello */
							else{
								
								if( motes[k].inMsg->currSrcID == motes[k].helloMsg.finalDestID ){
									motes[k].helloMsg.msgID = 0;
									motes[k].helloMsg.msgType = 0;
									motes[k].helloMsg.srcID = -1;
									motes[k].helloMsg.finalDestID = -1;
									motes[k].helloMsg.currSrcID = k;
									motes[k].helloMsg.currDestID = -1;
									motes[k].helloMsg.numHops = -1;
									motes[k].helloMsg.numCopies = -1;			
								}
								
							}
						}
						
						
					}
					else{
						/* See if there's a message to send in buffer */
						bufferPtr = -1;
						u = ACK_WAIT + 1;
						for( i=0; i<numNodes; i++ ){
							
							if( (motes[k].buffExpire[ i ] > 0) 
									&& motes[k].buffExpire[ i ] < u ){
								bufferPtr = i;
								
							}
						}						
						if( motes[k].status == 1 ){

							/* If no message to send in buffer, send Hello */
							if( (motes[k].helloTimer == 0) && (bufferPtr == -1) ){
								motes[k].helloMsg.msgID = 2;
								motes[k].helloMsg.msgType = 2;
								motes[k].helloMsg.srcID = k;
								motes[k].helloMsg.finalDestID = motes[k].dstID;
								motes[k].helloMsg.currSrcID = k;
								motes[k].helloMsg.currDestID = -1;
								motes[k].helloMsg.numHops = 1;
								motes[k].helloMsg.nodesFound[0] = k;
								motes[k].helloMsg.numCopies = motes[k].msgCopies;

								motes[k].outMsg = &motes[k].helloMsg;		
							}
							/* Send message in buffer */
							if( bufferPtr != -1 ){
								motes[k].outMsg = &motes[k].msgBuff[ bufferPtr ];
								motes[k].buffExpire[ bufferPtr ] = 0;

							}
						}
						else{
							/* If no message to send in buffer, send Hello */
							if( (motes[k].helloTimer == 0) && (bufferPtr == -1) ){

								motes[k].outMsg = &motes[k].helloMsg;		
							}
							/* Send message in buffer */
							if( bufferPtr != -1 ){
								motes[k].outMsg = &motes[k].msgBuff[ bufferPtr ];
								motes[k].buffExpire[ bufferPtr ] = 0;

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
								}
								else if( (motes[k].inMsg->msgID == 2) ){
									printf("Src got dst's ack msg from relay, 266\n");
									done = 1;
								}
								else{
									/* Message was same as was sent from source, ignore */
								}
							
							}
							/* if motes[k].inMsg->msgType == 2, an ACK */
							else{
									if( motes[k].inMsg->currDestID == k ){
										if( motes[k].inMsg->currSrcID == motes[k].dstID ){
											printf("Messaging done, src got ack from dst, 280\n");
											done = 1;
										}
										/* ACK was from relay that heard message */
										else{
											motes[k].msgCopies--;
										}
									}	
								}
							}
						}
					}
					else{
						
						if( motes[k].helloTimer == 0 ){
							
							if( motes[k].status == 1 ){
								motes[k].helloMsg.msgID = 1;
								motes[k].helloMsg.msgType = 1;
								motes[k].helloMsg.srcID = k;
								motes[k].helloMsg.finalDestID = motes[k].dstID;
								motes[k].helloMsg.currSrcID = k;
								motes[k].helloMsg.currDestID = -1;
								motes[k].helloMsg.numCopies = motes[k].msgCopies;
								motes[k].helloMsg.numHops = 0;	
							}
							motes[k].outMsg = &motes[k].helloMsg;
							
						}

					}	
				}
				/* Block of code for everyone else */
				else{
					bufferPtr = -1;
					for( i=0,j=0; i<numNodes; i++ ){
						
						if( j < 1 && ( motes[k].buffExpire[ bufferPtr ] == 0 ) ){
							j++;
							bufferPtr = i;
							i = numNodes;
						}
						if( (motes[k].msgBuff)[ i ].msgSendPriority == 0 ){
							bufferPtr = i;
						}
					}

					/* I receieved Message */
					if( motes[k].inMsg != 0 ){
						/* Dst node recieves message addressed to it, sets status to 1 */
						if( (motes[k].status == 0) && (motes[k].inMsg->finalDestID == k) ){
							motes[k].dstID = motes[k].inMsg->srcID;
							motes[k].status == 1;
							printf("Got message at 399\n");
							done = 1;
						}
						/* Code block used by Dst node when acking SRC */
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
								buffExpire[ bufferPtr ] = ACK_WAIT;
							}
							/* Got Hello from Relay */
							else if( motes[k].inMsg->msgType == 0 ){
								motes[k].sendPriority = 1;	
							}
							/* Relay delivering message to me */
							else if( (motes[k].inMsg->msgID == 1) && (motes[k].inMsg->finalDestID == k) ){
								motes[k].msgBuff[ bufferPtr ].msgID = 1;
								motes[k].msgBuff[ bufferPtr ].msgType = 2;
								motes[k].msgBuff[ bufferPtr ].srcID = motes[k].dstID;
								motes[k].msgBuff[ bufferPtr ].finalDestID = k;
								motes[k].msgBuff[ bufferPtr ].currSrcID = k;
								motes[k].msgBuff[ bufferPtr ].currDestID = motes[k].inMsg->currSrcID;
								motes[k].msgBuff[ bufferPtr ].numHops = 0;
								motes[k].msgBuff[ bufferPtr ].nodesFound[0] = k;
								buffExpire[ bufferPtr ] = ACK_WAIT;					
							}
							/* ACK from relay or Src */
							else if( (motes[k].inMsg->msgType == 2) && (motes[k].inMsg->currDestID == k) 
									&& (motes[k].inMsg->msgID == 2) ){
								if( motes[k].inMsg->currDestID == k ){
									/* Src got ACK from Dst, know DST got its original message */
									if( motes[k].inMsg->currSrcID == motes[k].dstID ){
										printf("Test done, dst got ack from source, 280\n");
										done = 1;
									}
									/* ACK was from relay that heard message */
									else{
										motes[k].msgCopies--;
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
								if( (motes[k].helloMsg.finalDestID == motes[k].inMsg->currSrcID  ){
									motes[k].sendPriority = 1;	
								}
							}
							/* If message is a relay message from eithe source or relay */
							else if( motes[k].inMsg->msgType == 1 ){
								/* Relay message from a source and not a relay */
								if( motes[k].inMsg->srcID == motes[k].inMsg->currSrcID ){
									/* Add new Hello Message*/
									if( motes[k].helloMsg.msgID < motes[k].inMsg->msgID 
											&& motes[k].inMsg->numCopies > 0 ){
										motes[k].helloMsg.msgID = motes[k].inMsg->msgID;
										motes[k].helloMsg.msgType = 1;
										motes[k].helloMsg.srcID = motes[k].inMsg->srcID;
										motes[k].helloMsg.finalDestID = motes[k].inMsg->finalDestID;
										motes[k].helloMsg.currSrcID = k;
										motes[k].helloMsg.currDestID = motes[k].inMsg->finalDestID;
										motes[k].helloMsg.numCopies = -1;
										motes[k].helloMsg.numHops = motes[k].inMsg->numHops + 1;;		
										motes[k].helloMsg.nodesFound[1] = k;		

										motes[k].msgBuff[ bufferPtr ].msgID = motes[k].inMsg->msgID;
										motes[k].msgBuff[ bufferPtr ].msgType = 2;
										motes[k].msgBuff[ bufferPtr ].srcID = motes[k].inMsg->srcID;
										motes[k].msgBuff[ bufferPtr ].finalDestID = motes[k].inMsg->finalDestID;
										motes[k].msgBuff[ bufferPtr ].currSrcID = k;
										motes[k].msgBuff[ bufferPtr ].currDestID = motes[k].inMsg->srcID;
										motes[k].msgBuff[ bufferPtr ].numHops = 0;
										motes[k].msgBuff[ bufferPtr ].nodesFound[0] = k;

										motes[k].outMsg = &motes[k].msgBuff[ bufferPtr ];
									}
								}
								/* I don't have current relay message, add new message to deliver */
								else{
									/* Message recieved was from another relay node, ignore */
								}	
							}
							/* Got ACK from destination node, got back to saying I'm Here Hello */
							else{
								
								if( motes[k].inMsg->currDestID == motes[k].helloMsg.finalDestID ){
									motes[k].helloMsg.msgID = 0;
									motes[k].helloMsg.msgType = 0;
									motes[k].helloMsg.srcID = -1;
									motes[k].helloMsg.finalDestID = -1;
									motes[k].helloMsg.currSrcID = k;
									motes[k].helloMsg.currDestID = -1;
									motes[k].helloMsg.numHops = -1;
									motes[k].helloMsg.numCopies = -1;			
								}
								
							}
						}
						
						
					}
					else{
						/* See if there's a message to send in buffer */
						bufferPtr = -1;
						u = ACK_WAIT + 1;
						for( i=0,j=0; i<numNodes; i++ ){
							
							if( (motes[k].buffExpire[ bufferPtr ] > 0) 
									&& motes[k].buffExpire[ bufferPtr ] < u ){
								bufferPtr = i;
								
							}
						}						
						if( motes[k].status == 1 ){

							/* If no message to send in buffer, send Hello */
							if( (motes[k].helloTimer == 0) && (bufferPtr == -1) ){
								motes[k].helloMsg.msgID = 2;
								motes[k].helloMsg.msgType = 2;
								motes[k].helloMsg.srcID = k;
								motes[k].helloMsg.finalDestID = motes[k].dstID;
								motes[k].helloMsg.currSrcID = k;
								motes[k].helloMsg.currDestID = -1;
								motes[k].helloMsg.numHops = 1;
								motes[k].helloMsg.nodesFound[0] = k;
								motes[k].helloMsg.numCopies = motes[k].numCopies;

								motes[k].outMsg = &motes[k].helloMsg;		
							}
							/* Send message in buffer */
							if( bufferPtr != -1 ){
								motes[k].outMsg = &motes[k].msgBuff[ bufferPtr ];
								motes[k].buffExpire[ bufferPtr ] = 0;

							}
						}
						else{
							/* If no message to send in buffer, send Hello */
							if( (motes[k].helloTimer == 0) && (bufferPtr == -1) ){

								motes[k].outMsg = &motes[k].helloMsg;		
							}
							/* Send message in buffer */
							if( bufferPtr != -1 ){
								motes[k].outMsg = &motes[k].msgBuff[ bufferPtr ];
								motes[k].buffExpire[ bufferPtr ] = 0;

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
