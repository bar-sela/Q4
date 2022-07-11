/*
** server.c -- a stream socket server demo
*/

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <signal.h>
#include <pthread.h>
#include "stack.hpp"
#include <mutex>

#define PORT "3490"  // the port users will be connecting to
#define BACKLOG 10   // how many pending connections queue will hold
std::mutex lock;

/*
void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}
*/

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}
void *thread_func(void* new_fd_socket) {
    int sock_connection = *(int *) new_fd_socket;
    char buff[1024];
    while (1) {
        printf("waiting for client command:\n");
        /// the client already sent command
        bzero(buff, sizeof(buff));
        read(sock_connection, buff, 1024);
        //printf("buf is:%s",buff);
       if (strncmp(buff, "PUSH", 4) == 0) {
            char *p = &buff[5];
            lock.lock();
            push(p);
            lock.unlock();
        } else if (strncmp(buff, "POP\n", 4) == 0) {
            lock.lock();
            pop();
            lock.unlock();
        }
        else if (strncmp(buff, "TOP", 3) == 0) {
            sleep(1);
            lock.lock();
            char *p_toFirstElement = top();
            lock.unlock();
            if (p_toFirstElement == NULL)
                send(sock_connection, "Error : the stack is empty\n", 1024, 0);
            else
                send(sock_connection, p_toFirstElement, 1024, 0);
        }
    }
}
int main(void) {
    int sockfd, new_fd_socket;  // listen on sock_fd, new connection on new_fd_socket
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
    socklen_t sin_size;
    struct sigaction sa;
    int yes = 1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                       sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure

    if (p == NULL) {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    ///////////////////////**** lock init
    printf("server: waiting for connections...\n");
    while (1) {  // main accept() loop
        sin_size = sizeof their_addr;
        new_fd_socket = accept(sockfd, (struct sockaddr *) &their_addr, &sin_size);
        if (new_fd_socket == -1) {
            perror("accept");
            continue;
        }

        inet_ntop(their_addr.ss_family,
                  get_in_addr((struct sockaddr *) &their_addr),
                  s, sizeof s);
        printf("server: got connection from %s\n", s);

        pthread_t client_thread;
        pthread_create(&client_thread, NULL, thread_func, &new_fd_socket);
    }
}