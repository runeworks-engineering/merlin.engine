#include "gym.h"
#include "settings.h"
#include <iostream>

GymServer::GymServer(int port)
    : port_(port), running_(false), ctx_(nullptr), sock_(nullptr) {
}

GymServer::~GymServer() {
    stop();
}

void GymServer::setSim(Sim* sim){
    sim_ = sim;
}

void GymServer::setGoalImage(const std::vector<uint8_t>& in){
    goal_image = in;
}

void GymServer::setCurrentImage(const std::vector<uint8_t>& in) {
    current_image = in;
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
    // Only now is it safe to clean up socket/context:
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

    while (running_) {
        zmq::pollitem_t items[] = { {static_cast<void*>(*sock_), 0, ZMQ_POLLIN, 0} };
        zmq::poll(items, 1, 100); // 100 ms timeout

        if (items[0].revents & ZMQ_POLLIN) {
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
                sim_->api_reset();
                while (!sim_->hasReset() && sim_->isRunning());
                std::string ack = "ok";
                sock_->send(zmq::buffer(ack), zmq::send_flags::none);
            }
            else if (j["type"] == "get_images") {
                zmq::message_t cur_msg(current_image.data(), current_image.size());
                zmq::message_t goal_msg(goal_image.data(), goal_image.size());

                sock_->send(cur_msg, zmq::send_flags::sndmore);
                sock_->send(goal_msg, zmq::send_flags::none);

            }
            else if (j["type"] == "step") {
                auto xye = j["xye"].get<std::vector<float>>();
                if (xye.size() >= 3)
                    sim_->control(xye[0], xye[1], xye[2]);

                for (int i = 0; i < 10; i++) {
                    sim_->api_step();
                    while (!sim_->hasStepped() && sim_->isRunning()); // Wait until the sim has actually processed the step
                }
                // After you’ve executed the step in your sim:
                glm::vec2 pos = sim_->getNozzlePosition();

                // 1) Build a JSON object with the new position
                nlohmann::json reply;
                reply["x"] = pos.x;
                reply["y"] = pos.y;

                // 2) Dump it to a string
                std::string ack = reply.dump();

                // 3) Send it back over ZMQ
                sock_->send(zmq::buffer(ack), zmq::send_flags::none);
            }
            else if (j["type"] == "is_done") {
                bool done = !sim_->isRunning();
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
    }

    // Close and clean up
    sock_->close();
    delete sock_; sock_ = nullptr;
    delete ctx_; ctx_ = nullptr;
    std::cout << "[GymServer] Server stopped.\n";

}
