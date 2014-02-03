#pragma once

#include "msg_def.h"

/*
    [msg_length][msg_type][msg_data]
*/

class msg_reader {
public:
    msg_reader();
    msg_reader(const char* buffer);

    template <typename _Ty>
    msg_reader& operator >> (_Ty& value);
    msg_reader& operator >> (std::string& value);

    const char* next() const;
    msg_reader& skip(msg_size_t size);

    bool is_valid() const;

public:
    msg_size_t read;
    const char* buffer;
};


inline msg_reader::msg_reader()
    : read(0), buffer(nullptr)
{
}

inline msg_reader::msg_reader(const char* _buffer)
    : read(0), buffer(_buffer)
{
}

template <typename _Ty>
inline msg_reader& msg_reader::operator >> (_Ty& value)
{
    value = * reinterpret_cast<const _Ty *>(&buffer[read]);
    read += sizeof(_Ty);
    return (*this);
}

inline msg_reader& msg_reader::operator >> (std::string& value)
{   
    uint32_t length = 0;
    (*this) >> length;
    
    const char* tchar_ptr = reinterpret_cast<const char *>(&buffer[read]);
    char temp[msg::MAX_MSG_SIZE] = { 0, };
    std::copy(tchar_ptr, tchar_ptr + length, temp);
    value = std::string(temp);
    read += static_cast<msg_size_t>(length * sizeof(char));

    return (*this);
}

template <>
inline msg_reader& msg_reader::operator >> (msg::bin_t& bin)
{   
    uint32_t length = 0;
    (*this) >> length;
    
    const msg::byte* byte_ptr = reinterpret_cast<const char *>(&buffer[read]);
    bin.set(byte_ptr, length);
    read += static_cast<msg_size_t>(length * sizeof(msg::byte));

    return (*this);
}

inline const char* msg_reader::next() const
{
    return &buffer[read];
}

inline msg_reader& msg_reader::skip(msg_size_t size)
{
    read += size;
    return (*this);
}

inline bool msg_reader::is_valid() const
{
    return buffer != nullptr;
}

template <typename _MsgTy>
inline msg_reader get_reader_from_msg(const _MsgTy& msg)
{
    msg_writer writer;
    writer << msg;

    msg_reader reader(writer);
    msg_size_t size;
    msg_type_t type;
    reader >> size;
    reader >> type;
    return msg_reader(reader.next());
}
