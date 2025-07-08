#pragma once
#include <vector>
#include <thread>
#include <atomic>
#include <zmq.hpp>
#include "../json.hpp"
#include "../sim/sim.h"

class GymServer {
public:
    GymServer(int port = 5555);
    ~GymServer();

    void setSim(Sim* sim);

    void setCurrentImage(const std::vector<uint8_t>&);
    void setGoalImage(const std::vector<uint8_t>&);
    void setGoalDepthImage(const std::vector<uint8_t>&);

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
    Sim* sim_;


    std::vector<uint8_t> goal_image;
    std::vector<uint8_t> goal_depth_image;
    std::vector<uint8_t> current_image;


};
