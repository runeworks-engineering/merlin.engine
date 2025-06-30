#include "gym.h"
#include <iostream>
/*
GymServer::GymServer(int port)
    : port_(port), running_(false), ctx_(nullptr), sock_(nullptr) {
}

GymServer::~GymServer() {
    stop();
}

void GymServer::start() {
    if (!running_) {
        running_ = true;
        serverThread_ = std::thread(&GymServer::serverLoop, this);
    }
}

void GymServer::stop() {
    if (running_) {
        running_ = false;
        if (serverThread_.joinable())
            serverThread_.join();
    }
    // Cleanup
    if (sock_) { sock_->close(); delete sock_; sock_ = nullptr; }
    if (ctx_) { delete ctx_; ctx_ = nullptr; }
}

bool GymServer::isRunning() const {
    return running_;
}

void GymServer::serverLoop() {
    ctx_ = new zmq::context_t(1);
    sock_ = new zmq::socket_t(*ctx_, zmq::socket_type::rep);

    std::string addr = "tcp://*:" + std::to_string(port_);
    sock_->bind(addr);
    std::cout << "[GymServer] Listening on " << addr << std::endl;

    while (running_) {
        zmq::message_t msg;
        if (!sock_->recv(msg, zmq::recv_flags::none))
            continue;

        std::string msg_str(static_cast<char*>(msg.data()), msg.size());

        nlohmann::json j;
        try { j = nlohmann::json::parse(msg_str); }
        catch (...) {
            std::string err = "bad json";
            sock_->send(zmq::buffer(err), zmq::send_flags::none);
            continue;
        }

        // ---- Command Handlers ----
        if (j["type"] == "reset") {
            sim_->reset();
            std::string ack = "ok";
            sock_->send(zmq::buffer(ack), zmq::send_flags::none);
        }
        else if (j["type"] == "get_images") {
            auto current = sim_->get_current_image(); // std::vector<uint8_t>
            auto goal = sim_->get_goal_image();

            zmq::message_t cur_msg(current.data(), current.size());
            zmq::message_t goal_msg(goal.data(), goal.size());

            sock_->send(cur_msg, zmq::send_flags::sndmore);
            sock_->send(goal_msg, zmq::send_flags::none);
        }
        else if (j["type"] == "step") {
            auto xyze = j["xyze"].get<std::vector<float>>();
            sim_->step(xyze);
            std::string ack = "stepped";
            sock_->send(zmq::buffer(ack), zmq::send_flags::none);
        }
        else if (j["type"] == "is_done") {
            bool done = sim_->is_done();
            nlohmann::json resp;
            resp["done"] = done;
            auto s = resp.dump();
            sock_->send(zmq::buffer(s), zmq::send_flags::none);
        }
        else {
            std::string err = "unknown command";
            sock_->send(zmq::buffer(err), zmq::send_flags::none);
        }
    }

    // Close and clean up
    sock_->close();
    delete sock_; sock_ = nullptr;
    delete ctx_; ctx_ = nullptr;
    std::cout << "[GymServer] Server stopped.\n";
}
*/