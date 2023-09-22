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
#include <string>
#include <fstream>

using namespace std;

/**
 * @brief because there wasn't a (safe) library way to do this
 * 
 * @param src C string to copy from
 * @param dst empty C string to copy to
 * @param size size of src and later dst
 */
void c_str_copy(const char * src, char * dst, int size) {
	if (dst != nullptr) {
		cout << "Copy failed, dst was not empty.\n";
	}
	else {
		dst = new char[size];
		int i;
		for (i = 0; i < size; ++i) {
			dst[i] = src[i];
		}
	}
}


int main (int argc, char *argv[]) {
	cout << "Starting up client main...\n"; //Test print//
	
	int opt;
	// CL arguments
	bool new_chan = false; // * tutorial
	int p = -1; // * tutorial: default to -1 (invalid) to determine if fields have been modified
	double t = -1.0;
	int e = -1;
	int m = MAX_MESSAGE; // * added from tutorial vid
	vector<FIFORequestChannel *> channels; channels.reserve(2); // for New Channel Function
	
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
		/*
		Upon successful completion, fork() shall return 0 to the child process and shall return the process ID of the child process to the parent process.
		Both processes shall continue to execute from the fork() function. Otherwise, -1 shall be returned to the parent process, no child process shall be created, and errno shall be set to indicate the error.
		from `https://pubs.opengroup.org/onlinepubs/9699919799/functions/fork.html`
		*/
		int pid = fork();
		
		if (pid == 0) { // if we're in the child
			// use int m to set second argument (how?)
			string m_val_string = to_string(m);
			const char * m_val_const = m_val_string.c_str();
			char * m_val = nullptr;
			c_str_copy(m_val_const, m_val, m_val_string.size()+1);
			cout << "m_val: `" << m_val << "`\n"; //Test print//
			
			// code template from lab demo
			char path[] = "./server";
			char flag[] = "-m";
			char* args[] = {path, flag, m_val, NULL}; //"arg1", "arg2", // (char*) 
			int ret = execvp(args[0], args);
			if (ret == -1) {
				perror("execvp");
				exit(EXIT_FAILURE);
			}
			
			delete[] m_val; // delete memory allocated in c_str_copy
		}
	}
	// {} END Section 0
	
	//? I don't know the internals of the Channel, so this might have to go before Server is run from Client (Section 0) instead of after
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
	
	if ((p != -1) && (t != -1.0) && (e != -1)) { // only want to run Section A if args (p, t, e) have all been specified
		// {} Section A: Single Point Function
		// [] Currently passes basic testing
		// example data point request
		char buf[MAX_MESSAGE]; // 256
		datamsg x(p, t, e);
		
		// -- Copy message into buffer
		memcpy(buf, &x, sizeof(datamsg));
		// -- Send buffer to channel
		chan.cwrite(buf, sizeof(datamsg)); // question
		double reply;
		// -- reply (the ECG value) returned as a double
		chan.cread(&reply, sizeof(double)); //answer
		cout << "For person " << p << ", at time " << t << ", the value of ecg " << e << " is " << reply << endl;
		// {} END Section A
	}
	else if (p != -1) { // else, if (p) is specified, request 1000 lines of code
		// {} Section B: Many Lines Function
		fstream outfile;
		char outfilename[] = "received/xN.csv"; outfilename[10] = (char)(p + '0');
		fstream file(outfilename);
		outfile.open(outfilename, fstream::out);
		
		// + example line from 1.csv: `0,0.635,-0.64`
		
		double time = 0; // time in increments of 0.004
		
		char buf[MAX_MESSAGE]; // 256
		datamsg x(p, 0, 1);
		double reply;
		
		// need to loop over first 1000 lines
		for (int i = 0; i < 1000; ++i) {
			outfile << time << ",";
			
			// send request for ECG1
			x.seconds = time;
			x.ecgno = 1;
			memcpy(buf, &x, sizeof(datamsg)); // -- Copy message into buffer
			chan.cwrite(buf, sizeof(datamsg)); // question  // -- Send buffer to channel
			chan.cread(&reply, sizeof(double)); //answer // -- reply (the ECG value) returned as a double
			
			outfile << reply << ",";
			
			// send request for ECG2
			x.seconds = time;
			x.ecgno = 2;
			memcpy(buf, &x, sizeof(datamsg)); // -- Copy message into buffer
			chan.cwrite(buf, sizeof(datamsg)); // question  // -- Send buffer to channel
			chan.cread(&reply, sizeof(double)); //answer // -- reply (the ECG value) returned as a double
			
			outfile << reply << "\n";
			
			// write line to recieved/x1.csv
			// (functionally the same as 2000 individual requests plus a file write)
			// can open file however we chose (fstream, file, fopen, etc.), just have to make sure result is identical to original
			time += 0.004;
		}
		
		outfile.close();
		// {} END Section B
	}
	else if (filename != "") {
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
		file_req->offset = -1; // TODO set offset in the file
		file_req->length = -1; // TODO set length of segment, be careful to use min of segment len and len of file remaining
		// send the request (buf2)
		chan.cwrite(buf2, len);  // reuse this line from earlier
		// receive the response
		// cread into buf3 (length file_req = len (same length as specified in `file_req->length =`))	
		// write buf3 into file: received/filename

		delete[] buf2;
		delete[] buf3;
		
		// {} END Section C
	}
	else {
		cout 
			<< "Valid flags not set; flags were: " 
			<< "p: `" << p << "`, "
			<< "t: `" << t << "`, "
			<< "e: `" << e << "`, "
			<< "filename: `" << filename << "`, "
			<< "m: `" << m << "`, "
			<< "new_chan: `" << new_chan << "`.\n";
	}
	
	
	// delete any new channels
	if (channels.size() > 1) {
		for (unsigned int i = 1; i < channels.size(); ++i) {
			delete channels.at(i);
		}
	}
	
	
	// closing the channel    
    MESSAGE_TYPE qm = QUIT_MSG;
    chan.cwrite(&qm, sizeof(MESSAGE_TYPE));
}
