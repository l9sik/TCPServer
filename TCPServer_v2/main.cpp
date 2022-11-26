
#include "header.h"


int main() {
    auto* serv = new server();
    std::cout << "starting server.." << std::endl;
    return serv->startServer();
}
