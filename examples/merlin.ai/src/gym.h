#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <zmq.hpp>
#include "json.hpp"
/*
class GymServer {
public:
    GymServer(int port = 5555);
    ~GymServer();

    void start();
    void stop();
    bool isRunning() const;

private:
    void serverLoop();

    int port_;
    std::atomic<bool> running_;
    std::thread serverThread_;

    // ZMQ context and socket as pointers to allow reinit in thread
    zmq::context_t* ctx_;
    zmq::socket_t* sock_;
};
*/