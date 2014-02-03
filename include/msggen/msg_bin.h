#ifndef __MSG_BIN_H__
#define __MSG_BIN_H__

#include <stdint.h>

// binary field type
namespace msg { ;

typedef char byte;

class bin_t {
public:
    bin_t();
    bin_t(const byte* buffer, uint32_t length);
    bin_t(const bin_t& other);
    bin_t(bin_t&& wrapper);
    ~bin_t();
        
    bin_t& operator = (const bin_t& other);
    bin_t& operator = (bin_t&& wrapper);
    void set(const byte* buffer, uint32_t length);
    void release();
    void reset();

    const byte* get_buffer() const { return buffer; }
    uint32_t get_length() const { return length; }
    std::size_t size() const { return sizeof(byte) * length; }

private:
    byte* buffer;
    uint32_t length;
};

}

inline msg::bin_t::bin_t()
    : buffer(nullptr), length(0)
{
}

inline msg::bin_t::bin_t(const byte* buffer, uint32_t length)
    : buffer(nullptr), length(0)
{
    set(buffer, length);
}

inline msg::bin_t::bin_t(const bin_t& other)
    : buffer(nullptr), length(0)
{
    set(other.buffer, other.length);
}

inline msg::bin_t::bin_t(bin_t&& wrapper)
    : buffer(nullptr), length(0)
{
    set(wrapper.buffer, wrapper.length);
    wrapper.reset();
}

inline msg::bin_t::~bin_t()
{
    release();
}

inline msg::bin_t& msg::bin_t::operator = (const bin_t& other)
{
    set(other.buffer, other.length);
    return (*this);
}

inline msg::bin_t& msg::bin_t::operator = (bin_t&& wrapper)
{
    set(wrapper.buffer, wrapper.length);
    wrapper.reset();
    return (*this);
}

inline void msg::bin_t::set(const byte* src, uint32_t srclen)
{
    release();
    std::size_t copysize = sizeof(byte) * srclen;
    buffer = static_cast<byte *>(malloc(copysize));
	length = srclen;

    std::copy(src, src + srclen, buffer);
}

inline void msg::bin_t::release()
{
    if (buffer != nullptr) {
        free(buffer);
    }
    reset();
}

inline void msg::bin_t::reset()
{
    buffer = nullptr;
    length = 0;
}

#endif
