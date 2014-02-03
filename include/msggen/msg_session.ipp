#include "msg_session.h"
#include "msg_handler_map.h"
#include "system_msg.h"
#include "exclusive_run.h"

/// message handler
msg_handler msg_handler_map[65536];


msg_session::msg_session(boost::asio::io_service& _io_service)
    : socket(_io_service), io_service(_io_service), request_close(false), deferred_close_timer(_io_service), connected(false),
    msg_size(0), msg_buffer_length(0), msg_buffer(nullptr)
{
    exclusive_run_t::init(&write_execlusion);
}

msg_session::~msg_session()
{
}

void msg_session::connect(tcp::resolver::iterator endpoint_iterator)
{
    tcp::endpoint endpoint1 = *endpoint_iterator;

    socket.async_connect(endpoint1,
        boost::bind(&msg_session::handle_connect, shared_from_this(),
        boost::asio::placeholders::error, ++endpoint_iterator));
}

void msg_session::close()
{
    io_service.post(boost::bind(&msg_session::do_close, shared_from_this()));
}

void msg_session::deferred_close()
{
    /// after 1 second, request deferred close
    deferred_close_timer.expires_from_now(boost::posix_time::seconds(1));
    deferred_close_timer.async_wait(boost::bind(&msg_session::do_deferred_close, shared_from_this()));
}

bool msg_session::is_connected() const
{
    return connected;
}

void msg_session::accepted()
{
    connected = true;
    request_read_msg_size();
}

void msg_session::request_read_msg_size()
{
    boost::asio::async_read(socket,
        boost::asio::buffer(&msg_size, sizeof(msg_size_t)),
        boost::asio::transfer_exactly(sizeof(msg_size_t)),
        boost::bind(&msg_session::handle_read_msg_size, shared_from_this(),
        boost::asio::placeholders::error));
}

void msg_session::request_read_msg()
{
    size_t msg_body_size = msg_size - sizeof(msg_size_t);

    // reallocate receive buffer
    if (msg_body_size > msg_buffer_length) {
        if (msg_buffer != nullptr) {
            free(msg_buffer);
        }
        msg_buffer_length = (msg_body_size >> 1) << 2;
        msg_buffer = static_cast<msg::byte *>(malloc(sizeof(msg::byte) * msg_buffer_length));
    }

    boost::asio::async_read(socket,
        boost::asio::buffer(msg_buffer, msg_body_size),
        boost::asio::transfer_exactly(msg_body_size),
        boost::bind(&msg_session::handle_read_msg, shared_from_this(),
        boost::asio::placeholders::error));
}

void msg_session::request_write()
{
    msg_writer_ref writer;
    if (write_msgs.try_pop(writer)) {
        boost::asio::async_write(socket,
            boost::asio::buffer(writer->buffer, writer->written),
            boost::bind(&msg_session::handle_write, shared_from_this(),
            boost::asio::placeholders::error));

    } else {
        handle_error(boost::system::error_code());
    }
}

void msg_session::handle_connect(const boost::system::error_code& error, tcp::resolver::iterator endpoint_iterator)
{
    if (!error)
    {
        if (request_close) {
            close();
            return;
        }
        connected = true;

        if (!write_msgs.empty()) {
            request_write();
        }

        request_read_msg_size();
    }
    else if (endpoint_iterator != tcp::resolver::iterator())
    {
        socket.close();
        tcp::endpoint endpoint = *endpoint_iterator;
        socket.async_connect(endpoint,
            boost::bind(&msg_session::handle_connect, shared_from_this(),
            boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else
    {
        std::cout << "handle_connect FAIL!" << std::endl;
        handle_error(error);
    }
}

void msg_session::handle_read_msg_size(const boost::system::error_code& error)
{
    if (!error && msg_size < msg::MAX_MSG_SIZE) {
        if (request_close) {
            close();
            return;
        }

        request_read_msg();

    } else {
        handle_error(error);
    }
}

void msg_session::handle_read_msg(const boost::system::error_code& error)
{
    if (!error) {
        if (request_close) {
            close();
            return;
        }
        // TODO check receive size, and re-request read

        msg_reader reader(msg_buffer);
        msg_type_t msg_type;
        reader >> msg_type;

        /// msg dispatching
        dispatch_msg(msg_type, msg_reader(reader.next()));

        request_read_msg_size();

    } else {
        handle_error(error);
    }
}

void msg_session::handle_write(const boost::system::error_code& error)
{
    if (!error) {
        if (request_close) {
            close();
            return;
        }

        if (!write_msgs.empty()) {
            request_write();
        }

    } else {
        handle_error(error);
    }
}

void msg_session::handle_error(const boost::system::error_code& code)
{
    if (connected)
    {
        // cancel all request
        socket.cancel();

        // disconnect
        socket.close();
        connected = false;
    }
    
    msg::session_error_msg::handle(shared_from_this(), msg::session_error_msg(code.value()));
}

void msg_session::dispatch_msg(msg_type_t msg_type, msg_reader reader)
{
    _ASSERT(msg_handler_map[msg_type] != NULL);
    msg_handler_map[msg_type](shared_from_this(), reader);
}

void msg_session::do_write(msg_writer_ref writer)
{
    write_msgs.push(writer);

    if (!connected)
        return;

    exclusive_run_t exec(&write_execlusion);

    // only-one thread can enter below code
    if (exec.is_acquired()) {
        request_write();
    }
}

void msg_session::do_close()
{
    socket.close();
    connected = false;
}

void msg_session::do_deferred_close()
{
    /// if there is no writing msg, do close right now
    if (write_msgs.empty()) {
        close();

    } else {
        /// or, request close after io completion
        request_close = true;
    }
}

test_msg_session::test_msg_session(boost::asio::io_service& io_service)
    : msg_session(io_service)
{
}

test_msg_session::~test_msg_session()
{
}

bool test_msg_session::is_queue_empty()
{
    lock_type lock(mutex);
    return msg_queue.empty();
}

test_msg_session::msg_pair test_msg_session::peek_msg()
{
    lock_type lock(mutex);
    return msg_queue.empty()? msg_pair() : msg_queue.front();
}

void test_msg_session::pop_msg()
{
    lock_type lock(mutex);
    if (msg_queue.empty())
        return;

    auto front = msg_queue.front();
    delete[] front.second.buffer;
    msg_queue.pop_front();
}

void test_msg_session::clear_msg()
{
    lock_type lock(mutex);
    std::for_each(msg_queue.begin(), msg_queue.end(), [] (msg_pair& pair) {
        delete[] pair.second.buffer;
    });
    msg_queue.clear();
}

void test_msg_session::dispatch_msg(msg_type_t msg_type, msg_reader reader)
{
    // copy buffer to new buffer for preserving data
    msg_size_t msg_body_size = msg_size - sizeof(msg_type_t);
    msg::byte* copied_buffer = new msg::byte[msg_body_size];
    std::copy(&msg_buffer[sizeof(msg_type_t)], &msg_buffer[msg_size], copied_buffer);

    {
        lock_type lock(mutex);
        msg_queue.push_back(std::make_pair(msg_type, msg_reader(copied_buffer)));
    }

    msg_session::dispatch_msg(msg_type, reader);
}
