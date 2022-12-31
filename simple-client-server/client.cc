#include <arpa/inet.h>   // inet_addr()
#include <netinet/in.h>  // Sockaddr for AF_INET family
#include <netinet/ip.h>  // Linux ipv4 implementation
#include <sys/socket.h>  // socket
#include <unistd.h>      // close

#include <array>
#include <iostream>
#include <string_view>

constexpr uint16_t PORT = 8080;
const std::string LOCAL_HOST("127.0.0.1");
const auto SERVER_IP = inet_addr(LOCAL_HOST.c_str());

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

  ipv4SocketAddr addrInfo{};
  addrInfo.sin_addr.s_addr = SERVER_IP;
  addrInfo.sin_port =
      htons(PORT);  // Convert to network byte ordering not little/big endian
  addrInfo.sin_family = AF_INET;

  int res = connect(socketFD, reinterpret_cast<struct sockaddr*>(&addrInfo),
                    sizeof(addrInfo));
  if (res == -1) {
    std::cerr << "Failed to connect to " << LOCAL_HOST << ", on port " << PORT
              << std::endl;
    return 0;
  }

  std::cout << "Connected to server, sending message to server" << std::endl;

  const std::string msg("Hello I am a client! I'm gonna send some messages!!");
  res = send(socketFD, msg.c_str(), msg.size(), 0);

  if (res == -1) {
    std::cerr << "Failed to send message to server!" << std::endl;
    return 0;
  }

  std::array<char, 128 + 1> response = {'\0'};

  res = recv(socketFD, &response[0], response.size(), 0);
  if (res == -1) {
    std::cerr << "Failed to receive response from server" << std::endl;
    return 0;
  }

  std::printf("+++ Server replied. Message:\n>>> %s\n", &response[0]);

  std::string buffer;
  buffer.reserve(512);

  while (true) {
    buffer.clear();

    // Wait on stdin to send messages to the server
    std::cout << "=== Send at most 512 chars server (type END to exit): ";

    std::getline(std::cin, buffer);

    // Send to server
    res = send(socketFD, buffer.c_str(), buffer.size(), 0);
    if (res == -1) {
      std::cout << "Failed to send message to server!" << std::endl;
      return 0;
    }

    // Receive response from server
    res = recv(socketFD, &response[0], response.size() - 1, 0);
    if (res == -1) {
      std::cout << "Failed to receive response from server" << std::endl;
      return 0;
    }

    response[res] = '\0';

    // When recv() returns 0, the peer has performed an orderly shutdown. We
    // shall terminate too.
    // https://man7.org/linux/man-pages/man2/recv.2.html#RETURN_VALUE
    if (res == 0) {
      std::cout << "Server has already closed connection! Terminating ..."
                << std::endl;
      close(socketFD);
      return 1;
    }

    std::printf("+++ Server replied. Message:\n>>> %s\n", &response[0]);

    if (buffer == "END") break;
  }

  // When the program terminates, the file descriptors will be automatically
  // closed, but it is good practice to close it ourselves

  std::cout << "Closing client socket. Goodbye!" << std::endl;
  // Close the socket for goodness sake
  close(socketFD);
}