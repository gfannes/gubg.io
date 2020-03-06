#ifndef HEADER_gubg_ip_tcp_Connection_hpp_ALREADY_INCLUDED
#define HEADER_gubg_ip_tcp_Connection_hpp_ALREADY_INCLUDED

#include <gubg/ip/Codes.hpp>
#include <gubg/ip/Socket.hpp>
#include <gubg/mss.hpp>
#include <ostream>
#include <list>

namespace gubg { namespace ip { namespace tcp { 

    class Server;
    class Client;

    class Connection
    {
    public:
        Socket socket;
        std::list<std::string> received_msgs;

        Connection()
        {
            socket.set_blocking(false);
        }

        const Endpoint &peer_ep() const {return peer_ep_;}

        bool established() const { return state_ == State::Connected; }

        bool send(const std::string &msg)
        {
            MSS_BEGIN(bool);
            out_msgs_.emplace_back(msg);
            MSS_END();
        }

        bool process()
        {
            MSS_BEGIN(bool);

            MSS(established());

            //Send a message part, if any
            if (!out_msgs_.empty())
            {
                auto &out_msg = out_msgs_.front();
                MSS(socket.send(out_msg.msg.data(), out_msg.msg.size(), out_msg.offset));
                if (out_msg.offset == out_msg.msg.size())
                    out_msgs_.pop_front();
            }

            //Try to receive a message
            {
                receive_buffer_.resize(1024);
                std::size_t offset = 0;
                switch (const auto rc = socket.recv(&receive_buffer_[0], receive_buffer_.size(), offset))
                {
                    case ReturnCode::WouldBlock: break;
                    default: MSS(rc); break;
                }
                receive_buffer_.resize(offset);
                if (!receive_buffer_.empty())
                    received_msgs.push_back(receive_buffer_);
            }

            MSS_END();
        }

        void stream(std::ostream &os) const
        {
            os << "[Connection]{" << std::endl;
            os << socket << std::endl;
            os << peer_ep() << std::endl;
            os << "}" << std::endl;
        }

    private:
        enum class State { Unconnected, Connecting, Connected, };

        ReturnCode error_(const std::string &msg)
        {
            MSS_BEGIN(ReturnCode);
            std::cout << "Error: " << msg << std::endl;
            MSS(change_state_(State::Unconnected));
            MSS_END();
        }

        ReturnCode change_state_(State new_state)
        {
            MSS_BEGIN(ReturnCode);

            if (new_state == state_)
                return ReturnCode::OK;

            state_ = new_state;

            switch (state_)
            {
                case State::Unconnected:
                    socket.close();
                    peer_ep_.clear();
                    break;
                case State::Connected:
                    MSS(socket.valid(), return error_("Socket is not valid"));
                    MSS(peer_ep_.valid(), return error_("Peer endpoint is not valid"));
                    break;
            }

            MSS_END();
        }
        State state_ = State::Unconnected;

        friend class Server;
        friend class Client;
        Endpoint peer_ep_;

        struct OutMsg
        {
            std::string msg;
            std::size_t offset = 0;
            OutMsg(const std::string &msg): msg(msg) {}
        };
        std::list<OutMsg> out_msgs_;

        std::string receive_buffer_;
    };

    inline std::ostream &operator<<(std::ostream &os, const Connection &connection)
    {
        connection.stream(os);
        return os;
    }

} } } 

#endif
