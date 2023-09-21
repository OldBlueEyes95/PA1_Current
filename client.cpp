/*
	Original author of the starter code
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date: 2/8/20
	
	Please include your Name, UIN, and the date below
	Name: Noah Batchelor
	UIN:  431005166
	Date: 9/24/2023
*/
#include "common.h"
#include "FIFORequestChannel.h"

#include <vector>
// #include <string> // only for use in new channel function (Section D)

using namespace std;


int main (int argc, char *argv[]) {
	int opt;
	// * tutorial: default to -1 (invalid) to determine if fields have been modified
	int p = -1;
	double t = -1.0;
	int e = -1;
	vector<FIFORequestChannel *> channels; // for New Channel Function
	
	bool new_chan = false; // * tutorial
	
	int m = MAX_MESSAGE; // * added from tutorial vid
	
	// -- parses command line arguments into usable C types
	string filename = "";
	// -- colon after arg means arg has a value; c is boolean, so no value needed
	while ((opt = getopt(argc, argv, "p:t:e:f:m:c")) != -1) { // * changed b/c tutorial
		switch (opt) {
			case 'p': // -- patient
				p = atoi (optarg);
				break;
			case 't': // -- time
				t = atof (optarg);
				break;
			case 'e': // -- ECG 1 or 2
				e = atoi (optarg);
				break;
			case 'f': // -- filename
				filename = optarg;
				break;
			case 'm': // -- buffer cap (specifies how many bytes (char) can be sent to/from server/client; aka determines how many chunks to split file into)
				m = atoi(optarg); // * tutorial
				break;
			case 'c': // -- new channel requested
				new_chan = true;
				break;
		}
	}
	
	// {} Section 0: Run Server from Client
	{
		// + stuff from tutorial video
		// give arguments for the server
		// server needs './server', '-m', '<val for -m arg>', 'NULL'
		// do fork
		// In child, run execvp using the server arguments
		// Lab 1a/b have info on how to run exec in a child
		
		// TODO use int m to set second argument (how?)
		// code template from lab demo
		char* args[] = {(char*) "./server", "-m", "not sure what this is", NULL}; //"arg1", "arg2",
		int ret = execvp(args[0], args);
		if (ret == -1) {
			perror("execvp");
			exit(EXIT_FAILURE);
		}
	}
	// {} END Section 0

    FIFORequestChannel default_chan("control", FIFORequestChannel::CLIENT_SIDE); // -- this is the control channel, as managed by the FIFOR.C. object
	channels.push_back(&default_chan); // add control channel to list of valid channels
	
	// {} Section D: New Channel Function
	// * from tutorial
	if (new_chan) {
		// send new channel request to server (step 1 of diagram)
		MESSAGE_TYPE nc = NEWCHANNEL_MSG;
        default_chan.cwrite(&nc, sizeof(MESSAGE_TYPE)); // copied from quit at bottom
		// step 2 of diagram
		// create variable to hold the name (can use `char *` or `string`)
		// cread the response from the server
		// call the FIFORequestChannel constructor with the name from the server (make sure to call new so that channel persists outside of if block scope)
		// ! I can see a potential issue: having them on the heap means memory to clean up, but the first channel isn't on the heap; need to ensure no memory leaks and currently memory from the channels vector is not cleaned up
		// step 3 of diagram
		// two ways: copy and paste the bottom into here OR create a structure to hold channels (array or vec)
		// push new channel into vector of channels
	}
	
	FIFORequestChannel chan = *(channels.back()); // ensure that most recent channel is used
	
	// {} END Section D
	
	// {} Section A: Single Point Function
	// * this 
	// * only want to run Section A if args (p, t, e) have all been specified
	// example data point request
    char buf[MAX_MESSAGE]; // 256
    datamsg x(1, 0.0, 1); //TODO replace hardcoded args with (p, t, e)
	
	// -- Copy message into buffer
	memcpy(buf, &x, sizeof(datamsg));
	// -- Send buffer to channel
	chan.cwrite(buf, sizeof(datamsg)); // question
	double reply;
	// -- reply (the ECG value) returned as a double
	chan.cread(&reply, sizeof(double)); //answer
	cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;
	// {} END Section A
	
	
	// {} Section B: Many Lines Function
	// * else, if (p) is specified, request 1000 lines of code
	// need to loop over first 1000 lines
	// send request for ECG1
	// send request for ECG2
	// write line to recieved/x1.csv
	// (functionally the same as 2000 individual requests plus a file write)
	// can open file however we chose (fstream, file, fopen, etc.), just have to make sure result is identical to original
	// {} END Section B
	
	
	// {} Section C Arbitrary File Function
    // sending a non-sense message, you need to change this
	filemsg fm(0, 0); // -- (0,0) is a request for a file size
	string fname = "teslkansdlkjflasjdf.dat"; // * this has been hardcoded, will replace with option recieved from getopt loop
	
	// --     message specifier + filename + null terminator
	int len = sizeof(filemsg) + (fname.size() + 1);
	char* buf2 = new char[len];
	memcpy(buf2, &fm, sizeof(filemsg));
	strcpy(buf2 + sizeof(filemsg), fname.c_str());
	// -- this message requests the file length from the server
	chan.cwrite(buf2, len);  // I want the file length;
	
	// * new code from tutorial
	int64_t filesize = 0; // to read the file length into
	chan.cread(&filesize, sizeof(int64_t));
	cout << "File length is: " << filesize << " bytes\n"; // attempted to copy output message // TODO hopefully they didn't use endl
	
	char * buf3 = nullptr; // -- response buffer
	// TODO set size to buff capacity (aka m)
	
	
	// loop over the segments in the file in chunks of filesize/m (m === buff_capcity)
	// create filemsg instance (can reuse buf2)
	filemsg * file_req = (filemsg *) buf2; // pointer cast is safe since buf2 originally held a filemsg
	file_req->offset = NULL; // TODO set offset in the file
	file_req->length = NULL; // TODO set length of segment, be careful to use min of segment len and len of file remaining
	// send the request (buf2)
	chan.cwrite(buf2, len);  // reuse this line from earlier
	// receive the response
	// cread into buf3 (length file_req = len (same length as specified in `file_req->length =`))	
	// write buf3 into file: received/filename

	delete[] buf2;
	delete[] buf3;
	
	// {} END Section C
	
	
	// TODO if necessary delete new channels
	/*
	for chan in channels (except the first one):
		delete the channel
	*/
	
	// closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite(&m, sizeof(MESSAGE_TYPE));
}
