##Spray and Wait Simulation
###Description:
This simulation implements the Spray and Wait Routing Protocol.  Protocol Description:

* Randomly selected source node selects a random node to send message to
* Source node hands out L copies of the message to relay nodes it encounters
* Source will deliver the message directly if it encounters the destination node
* Relay nodes will hang on to message until they encounter the destinatioin node
* Destination node will send an ack to notify source if it find source directly
* If destination node recieves message from a relay, it will use spray and wait to send the ack back to the original source
* Each test ends when the source hears an acknowledgement from either the relay or the destination node

###Setting up and Compiling:

The C source code uses four macros for easy tweaking of the configuration simulation variables:

* **SIZE_MAX 50** - Maximum size allowed for number of nodes and length and width of simulation grid (if using gdb for debugging it will throw an error if this set too large, this is due to Gdb memory stack limits.)
* **HELLO_WAIT 3** - The timer interval used by nodes in how long to wait before sending a hello "I am here" message.
* **START_TIME 40** - How long for source to wait before it starts spraying, used to ensure system is in stable state before test begins.
* **ACK_WAIT 10** - How long nodes wait to send a buffered message.
* **SEEN_TIME_MAX 50** - Max counter value for how long its been since a node has seen a specific node. Set to zero everytime it sees it.

To compile simply run the makefile:

**make** - Use this command for compilation

**make clean** - For cleaning up object and executable files

###Testing:

To run the simulation use the following command line arguments:
(I didn't test very many combinations of these arguements, there may be edge case errors that exist through weird combinations)

**./sim00 "RANGE"   "# of Trials"   "# of Copies"   "# of Nodes"   "Grid Length"**

1. **Range** - How far can nodes hear and send?  Accepts a range of 1 or more
2. **Number of Trials** - 1 or greater
3. **Copies** - Number of copies for source to spray, no more than number of nodes minus source
4. **Number of nodes** - 1 or more
5. __"*Length*" of Testing Grid__ - 2 or more (Won't accept nodes greater than what the area of the grid)

###Code Structure:

* Five arguments are read in from command line
* Initialization:
 * Source node is randomlly selected
 * The destination node is randomly selected
 * The hello timer is randomly initalized
* The outermost loop tracks each test run through
* A timing loop tracks time by iterations of that loop
* Grid movement
 * Nodes move in random selected directions for each iteration
 * The movement on the grid allows for multiple nodes to be in the same grid position
* Nodes use carrier sense: 
 * Nodes scan an area sourrounding its location on the grid with a radius equal to the range parameter
 * They won't hear anything if there is a collision
 * Nodes won't talk if the medium is busy
* Spraying:
 * After time interval reaches the designated start time the source will begin spraying
 * The source node will change its routine hello message to a delivery message
* The Spray message contains: 
 * the ID of the final destination node
 * the number of copies the source has left to give
 * Nodes will ignore the message if they are not the destination node and if there are no copies left


###Output:

* The outcome tracking:
 * How the Destination node heard the original message
 * How the Source heard back from the Destination node
* A grid depicting the locations of the nodes
 * A node's location is shown by its ID
 * Nodes will not be displayed if they are stacked on the same space as another node
* Number of Hops
 * Number of Hops for Destination to recieve message
 * Number of Hops for Source to hear back
* Average Timing Iterations:
 * Iterations for the destination to hear the message
 * Iterations for the source to hear back
 
###Assumptions and Simplifications:

* If the source does not hear an ack from a relay that took a copy, it will retain the same number of copies.  The relay waits for no ack reponse from the source
* There is no expiration timer for waiting to get an ack
* Nodes that carry a message send it in one simple hello, there is no handshake before sending messages.


###To do:

* There seems to be a random seeding issue where results between tests in one batch are the same while batches differ between eachother
* I did not have time to implement simulated traffic, where every node starts a test with a random chance to send a message and random start time for doing that
* The nodes use a message buffer for storing messages if the medium is very busy, an expiration timer eradicates messages when it times out.  Perhaps this should have been better used for waiting for ack replies.
