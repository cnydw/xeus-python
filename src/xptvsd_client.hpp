/***************************************************************************
* Copyright (c) 2018, Martin Renou, Johan Mabille, Sylvain Corlay and      *
* Wolf Vollprecht                                                          *
*                                                                          *
* Distributed under the terms of the BSD 3-Clause License.                 *
*                                                                          *
* The full license is in the file LICENSE, distributed with this software. *
****************************************************************************/

#ifndef XPYT_PTVSD_CLIENT_HPP
#define XPYT_PTVSD_CLIENT_HPP

#include <functional>
#include <queue>
#include <string>

#include "zmq.hpp"

#include "nlohmann/json.hpp"

#include "xeus/xauthentication.hpp"
#include "xeus/xkernel_configuration.hpp"

namespace xpyt
{
    class xptvsd_client
    {
    public:

        static constexpr const char* HEADER = "Content-Length: ";
        static constexpr size_t HEADER_LENGTH = 16;
        static constexpr const char* SEPARATOR = "\r\n\r\n";
        static constexpr size_t SEPARATOR_LENGTH = 4;

        using event_callback = std::function<void(const nl::json&)>;

        xptvsd_client(zmq::context_t& context,
                      const xeus::xconfiguration& config,
                      int socket_linger,
                      const std::string& user_name,
                      const std::string& session_id,
                      const event_callback& cb);


        ~xptvsd_client() = default;

        void start_debugger(std::string ptvsd_end_point,
                            std::string publisher_end_point,
                            std::string controller_end_point,
                            std::string controller_header_end_point);
 
    private:

        using queue_type = std::queue<std::string>;

        void process_message_queue();
        void handle_header_socket();
        void handle_ptvsd_socket(queue_type& message_queue);
        void handle_control_socket();
        void append_tcp_message(std::string& buffer);
        void handle_event(nl::json message);
        void forward_event(nl::json message);
        nl::json get_stack_frames(int thread_id, int seq);
        nl::json wait_next(int thread_id, int seq);
        void send_ptvsd_request(nl::json message);
        
        zmq::socket_t m_ptvsd_socket;
        std::size_t m_id_size;
        uint8_t m_socket_id[256];

        zmq::socket_t m_publisher;
        zmq::socket_t m_controller;
        zmq::socket_t m_controller_header;

        std::string m_user_name;
        std::string m_session_id;

        event_callback m_event_callback;

        using authentication_ptr = std::unique_ptr<xeus::xauthentication>;
        authentication_ptr p_auth;

        std::string m_parent_header;

        bool m_request_stop;

        queue_type m_message_queue;
        queue_type m_stopped_queue;
    };
}

#endif
