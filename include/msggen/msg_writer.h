#pragma once

#include "msg_def.h"

/*
    [msg_length][msg_type][msg_data]
*/

class msg_writer {
public:
    msg_writer();
    operator const msg::byte* () const;

    template <typename _Ty>
    msg_writer& operator << (const _Ty& value);
    msg_writer& operator << (const std::string& string);

    void ensure_buffer(std::size_t more_size);

public:
    msg_size_t written;
    msg::byte* buffer;

private:
    std::size_t buffer_length;
    msg_size_t* written_offset;
};

typedef boost::shared_ptr<msg_writer> msg_writer_ref;

inline msg_writer::msg_writer()
    : written(0), written_offset(NULL), buffer_length(0), buffer(nullptr)
{
    ensure_buffer(msg::DEFAULT_BUFFER_SIZE);

    // write size
    written_offset = reinterpret_cast<msg_size_t *>(buffer);
    *written_offset = written = sizeof(written);
}

inline msg_writer::operator const msg::byte* () const
{
    return buffer;
}

template <typename _Ty>
inline msg_writer& msg_writer::operator << (const _Ty& value)
{
    typedef _Ty* pointer_type;
    ensure_buffer(sizeof(_Ty));

    pointer_type ptr = reinterpret_cast<_Ty *>(&buffer[written]);
    *ptr = value;

    written += sizeof(_Ty);
    *written_offset = written;
    return (*this);
}

inline msg_writer& msg_writer::operator << (const std::string& string)
{
    uint32_t length = static_cast<uint32_t>(string.size());
    uint32_t strsize = length * sizeof(msg::byte);
    ensure_buffer(length + strsize);

    (*this) << length;

    msg::byte* tchar_ptr = reinterpret_cast<msg::byte *>(&buffer[written]);
	for (size_t i = 0; i < string.size(); ++i) {
		tchar_ptr[i] = string[i];
	}

    written += static_cast<msg_size_t>(strsize);
    *written_offset = written;

    return (*this);
}

template <>
inline msg_writer& msg_writer::operator << (const msg::bin_t& bin)
{
    uint32_t length = static_cast<uint32_t>(bin.get_length());
    ensure_buffer(bin.size());
    
    (*this) << length;

    msg::byte* byte_ptr = reinterpret_cast<msg::byte *>(&buffer[written]);
    std::copy(bin.get_buffer(), bin.get_buffer() + bin.get_length(), byte_ptr);

    written += static_cast<msg_size_t>(bin.size());
    *written_offset = written;

    return (*this);
}

inline void msg_writer::ensure_buffer(std::size_t more_size)
{
    if (written + more_size < buffer_length) {
        return;
    }

    // allocate new size
    buffer_length = ((written + more_size) >> 1) << 2;
    msg::byte* old_buffer = buffer;
    buffer = static_cast<msg::byte *>(malloc(sizeof(msg::byte) * buffer_length));

    if (old_buffer != nullptr) {
        std::copy(old_buffer, old_buffer + written, buffer);
        free(old_buffer);
    }
}
