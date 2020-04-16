#include <iostream>
#include <thread>

#include <boost/asio.hpp>

using namespace boost::asio;
using namespace std::literals;

bool send_udp_message(const std::string& message, std::string source_ip, const unsigned short port) {
	io_service io_service;
	ip::udp::socket socket(io_service);
 
	auto remote = ip::udp::endpoint(ip::address_v4::broadcast(), port);
 
	try {
		socket.open(ip::udp::v4());
		socket.bind(ip::udp::endpoint{
                                 ip::address::from_string(source_ip), (unsigned short)(port + 1)});
                socket.set_option(boost::asio::socket_base::broadcast(true));

	} catch (const boost::system::system_error& ex) {
                std::cout << "Exception thrown: " << ex.what();
                std::cout << "\nSrc: " << source_ip << "\n";
		return false;
	}
        std::cout << "Source ip : " << source_ip << ", source port " << port + 1 << "\n";
        std::cout << "Start sending...\n";
        for (int count = 0; ; count++) {
            std::string msg = message + "_" + std::to_string(count);
            size_t send_size = socket.send_to(buffer(msg), remote);
            if (send_size == 0) {
                std::cout << "Can't send from " << source_ip << "\n";
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
 
	return true;
}

bool get_udp_message(std::string source_ip, const unsigned short port) {
	io_service io_service;
	ip::udp::socket socket(io_service, ip::udp::endpoint(ip::udp::v4(), port));
        socket.set_option(boost::asio::socket_base::broadcast(true));
 
	try {

	} catch (const boost::system::system_error& ex) {
                std::cout << "Exception thrown: " << ex.what();
                std::cout << "\nSrc: " << source_ip << "\n";
		return false;
	}
        std::cout << "Start receiving on " << source_ip << ":" << port << "\n";
        while (1) {
            unsigned char recv_buf[65536];
            boost::asio::ip::udp::endpoint remote_endpoint;
            boost::system::error_code error;
            int size = socket.receive_from(boost::asio::buffer(recv_buf, 65536), 
                    remote_endpoint, 0, error);
            if (error && error != boost::asio::error::message_size) {
                std::cout << "Error in receive: \n";
            } else {
                recv_buf[size] = 0;
                std::cout << recv_buf << "\n";
            }

        }
 
	return true;
}

int main (int argc, char *argv[]){
    auto help_msg = "Usage: \n"
        "\tudp_broadcast send <interface ip> <port> <msg prefix>\n"
        "\tudp_broadcast get <interface ip> <port>";
    if (argc < 2) {
        std::cout << help_msg;
        return 1;
    }
    if (argv[1] == "send"s) {
        if (argc != 5) {
            std::cout << help_msg;
            return 1;
        }
        send_udp_message(argv[4], argv[2], std::stoi(argv[3]));
        return 0;
    } else if (argv[1] == "get"s) {
        if (argc != 4) {
            std::cout << help_msg;
            return 1;
        }
        get_udp_message(argv[2], std::stoi(argv[3]));
        return 0;
    }
    std::cout << help_msg;
    return 1;
}
