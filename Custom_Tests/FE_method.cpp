#include <iostream>
#include <unistd.h>
#include <chrono>
#include <thread>

#include <string>

using namespace std;

struct Identifier {
    int ID;
    int ID_Parent;
    string name;
};

void identify(Identifier metadata) {
    cout << "Process named `" << metadata.name << "` with ID `" << metadata.ID << "` and Parent ID `" << metadata.ID_Parent << "`.\n";
}

int main() {
    cout << "Before fork.\n";
    
    Identifier metadata;
    metadata.ID = getpid(); metadata.ID_Parent = getppid(); metadata.name = "Original";
    
    identify(metadata);
    
    int clone_id = fork();
    
    if (metadata.ID != getpid()) {
        // metadata.ID = getpid(); metadata.ID_Parent = getppid(); metadata.name = "Forked";
        char* args[] = {(char*) "./FE_method_servant", NULL}; //"arg1", "arg2",
        int ret = execvp(args[0], args);
        if (ret == -1) {
            perror("execvp");
            exit(EXIT_FAILURE);
        }
    }
    
    identify(metadata);
    
    cout << "After fork.\n";
}