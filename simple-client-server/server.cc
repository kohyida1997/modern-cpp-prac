#include <arpa/inet.h>   // inet_ntop()
#include <netinet/in.h>  // Sockaddr for AF_INET family
#include <netinet/ip.h>  // Linux ipv4 implementation
#include <sys/socket.h>  // socket
#include <unistd.h>      // close

#include <array>
#include <iostream>

constexpr uint16_t PORT = 8080;

using ipv4SocketAddr = struct sockaddr_in;

int main(int argc, char* argv[]) {
  // Server socket file descriptor
  // AF_INET -> ipv4 addresses
  // SOCK_STREAM -> TCP connection based protocol
  // 0 -> Protocol to use, 0 means choose automatically
  int socketFD = socket(AF_INET, SOCK_STREAM, 0);

  // Check if we could create a socket
  if (socketFD < 0) {
    std::cerr << "Failed to create socket" << std::endl;
    return 0;
  }

  // We are the server, bind the server to listen on specific address
  // and port (see: https://man7.org/linux/man-pages/man7/ip.7.html)
  ipv4SocketAddr sockAddr{};
  sockAddr.sin_addr.s_addr =
      htonl(INADDR_LOOPBACK);  // convert to network byte ordering
  sockAddr.sin_family = AF_INET;
  sockAddr.sin_port =
      htons(PORT);  // htons() is to convert to network byte ordering
                    // (irrespective of host byte ordering Little/Big endian)

  const int opt = 1;
  // Set option to allow reuse after closing. This is to avoid waiting out
  // TIME_WAIT after closing the socket
  int res = setsockopt(socketFD, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt,
                       sizeof(opt));

  if (res < 0) {
    std::cerr << "Failed to set socket options" << std::endl;
    return 0;
  }

  // Bind the server socket to Port 8080
  res = bind(socketFD, reinterpret_cast<struct sockaddr*>(&sockAddr),
             sizeof(sockAddr));

  if (res < 0) {
    std::cerr << "Failed to bind socket" << std::endl;
    return 0;
  }

  // Listen to incoming connections
  res = listen(socketFD, 1);
  if (res != 0) {
    std::cerr << "Failed to listen for incoming connections" << std::endl;
    return 0;
  }

  // Start accepting connections
  while (true) {
    std::array<char, 512 + 1> buffer = {'\0'};

    std::cout << "Server: Listening for new TCP Connections..." << std::endl;

    // Info about the accepted socket
    ipv4SocketAddr clientInfo{};
    size_t clientInfoSize = sizeof(clientInfo);

    // Block and wait until receive a connection
    int newSock =
        accept(socketFD, reinterpret_cast<struct sockaddr*>(&clientInfo),
               reinterpret_cast<socklen_t*>(&clientInfoSize));

    if (newSock == -1) {
      std::cerr << "Failed to accept client" << std::endl;
      return 0;
    }

    // Print information about the connected client
    char buf[32];
    std::cout << "Connection established with client at "
              << inet_ntop(AF_INET, &clientInfo.sin_addr.s_addr, buf, 32) << ":"
              << ntohs(clientInfo.sin_port) << std::endl;

    ssize_t readBytes = -1;

    // Buffer to store the reply to the client
    std::string replyBuffer;
    replyBuffer.reserve(128);

    // Buffer to store the message from the client
    std::string clientMessage;
    clientMessage.reserve(512);

    while (true) {
      // Blocking recv()
      readBytes =
          recv(newSock, reinterpret_cast<void*>(&buffer[0]), buffer.size(), 0);

      if (readBytes == 0) {
        std::cout << "The client has closed connection. Terminating ..."
                  << std::endl;
        close(socketFD);
        close(newSock);
        return 0;
      }

      if (readBytes == -1) {
        std::cerr << "Error on recv() bytes from Client" << std::endl;
        return 0;
      }

      // Put client message into the string
      clientMessage.clear();
      clientMessage.append(buffer.begin(), buffer.begin() + readBytes);

      // display message
      std::printf("+++ Read %ld bytes from client. Message:\n>>> %s\n",
                  clientMessage.size(), clientMessage.c_str());

      // reply to the client
      replyBuffer.clear();
      replyBuffer = "Server received " + std::to_string(readBytes) + " bytes";
      int sendRes = send(newSock, replyBuffer.c_str(), replyBuffer.size(), 0);

      std::printf("=== Replying to client. Message:\n--- %s\n",
                  replyBuffer.c_str());

      if (sendRes == -1) {
        std::cerr << "Failed to reply to the client!" << std::endl;
        return 0;
      }

      // Check if Client issued END
      if (clientMessage == "END") {
        std::cout << "Client issued END message. Terminating ..." << std::endl;
        break;
      }
    }

    close(newSock);
    break;
  }

  // When the program terminates, the file descriptors will be automatically
  // closed, but it is good practice to close it ourselves

  std::cout << "Closing server socket. Goodbye!" << std::endl;
  // Close the socket for goodness sake
  close(socketFD);
}