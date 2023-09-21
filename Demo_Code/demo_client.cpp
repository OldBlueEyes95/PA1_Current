#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

using namespace std;

int main() {
    cout << "Hi I am client" << endl;
    
    // Print the PID and PPID
    cout << "client PID: " << getpid() << " client PPID: " << getppid() << endl;

    int server_pid = fork();
    if(server_pid == 0) {
        //child process
        //running server process as a child of client
        char* args[] = {(char*) "../fork/fork", "arg1", "arg2", NULL};
        int ret = execvp(args[0], args);
        if (ret == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    } else {
        //client process -> client.cpp
    }

    this_thread::sleep_for(std::chrono::seconds(5));
    cout << "Client ends" << endl;
}