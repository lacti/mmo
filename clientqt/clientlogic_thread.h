#pragma once

class clientlogic_thread : public QThread
{
    Q_OBJECT

public:
    explicit clientlogic_thread();

    bool is_alive() const { return m_alive; }

public slots:
    void set_alive(bool b) { m_alive = b; }
    void push_async_command(const clt_async_command_t& c) { m_command_queue.push(c); }

protected:
    void run();

private:
    void logic_update(const double delta);

    bool m_alive;
    typedef tbb::concurrent_bounded_queue< clt_async_command_t > clientlogic_command_queue_t;
    clientlogic_command_queue_t m_command_queue;
};
