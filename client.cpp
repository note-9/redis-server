#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <cstring>
#include <iostream>
#include <vector>

using namespace std;

// Helper to write a 32-bit big-endian integer
void write_uint32_be(char* buf, uint32_t val) {
    buf[0] = (val >> 24) & 0xFF;
    buf[1] = (val >> 16) & 0xFF;
    buf[2] = (val >> 8) & 0xFF;
    buf[3] = val & 0xFF;
}

// Build a length-prefixed string field: 4-byte length + string data
vector<char> pack_str(const string& s) {
    vector<char> data(4 + s.size());
    write_uint32_be(data.data(), s.size());
    memcpy(data.data() + 4, s.data(), s.size());
    return data;
}

// Compose a simple command message: e.g. ["set", "foo", "bar"]
// Format: 4-byte message length + array count + each arg length + arg bytes
vector<char> build_command(const vector<string>& args) {
    vector<char> message;
    // Number of args as 4-byte int
    uint32_t arr_count = htonl(args.size());

    // Temporarily store args with length prefix
    vector<char> args_data;
    for (auto& arg : args) {
        auto packed = pack_str(arg);
        args_data.insert(args_data.end(), packed.begin(), packed.end());
    }

    // Total length = 4 (arr_count) + args_data.size()
    uint32_t total_len = 4 + args_data.size();

    message.resize(4 + total_len);
    // Total length prefix (excluding this 4-byte length)
    write_uint32_be(message.data(), total_len);
    // Copy array count (already big endian)
    memcpy(message.data() + 4, &arr_count, 4);
    // Copy args
    memcpy(message.data()+8, args_data.data(), args_data.size());

    return message;
}

int main() {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1234); // port your server listens on
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sock, (sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        return 1;
    }

    // Build SET command: set foo bar
    auto set_cmd = build_command({"set", "foo", "bar"});
    send(sock, set_cmd.data(), set_cmd.size(), 0);

    // Read response (simplified, assumes response fits in 1024 bytes)
    char buffer[1024];
    int n = recv(sock, buffer, sizeof(buffer), 0);
    if (n > 0) {
        cout << "SET response (hex): ";
        for (int i = 0; i < n; i++) printf("%02x ", (unsigned char)buffer[i]);
        cout << endl;
    }

    // Build GET command: get foo
    auto get_cmd = build_command({"get", "foo"});
    send(sock, get_cmd.data(), get_cmd.size(), 0);

    n = recv(sock, buffer, sizeof(buffer), 0);
    if (n > 0) {
        cout << "GET response (hex): ";
        for (int i = 0; i < n; i++) printf("%02x ", (unsigned char)buffer[i]);
        cout << endl;
    }

    close(sock);
    return 0;
}

