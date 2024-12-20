#pragma once

#include <minwindef.h>

#include <vector>

namespace ASM
{

class Writer
{
public:
    Writer(SIZE_T len, LPVOID addr = nullptr)
        : m_buf(len, '\x90')
        , m_index(0)
        , m_addr(addr)
    {
    }

    void Clear()
    {
        std::fill(m_buf.begin(), m_buf.end(), '\x90');
        m_index = 0;
    }

    void Push(char c)
    {
        m_buf[m_index++] = c;
    }

    template<typename TIter>
    void Push(TIter&& first, TIter&& last)
    {
        while (first != last)
        {
            Push(*first++);
        }
    }

    void PushPtr(intptr_t value)
    {
        Push((value & (0xFF << 0)) >> 0);
        Push((value & (0xFF << 8)) >> 8);
        Push((value & (0xFF << 16)) >> 16);
        Push((value & (0xFF << 24)) >> 24);
    }

    void PushAddrLong(intptr_t to)
    {
        intptr_t const from = (intptr_t)m_addr + (intptr_t)m_index + (intptr_t)4;
        PushPtr(to - from);
    }

    void PushAddrLong(LPVOID to)
    {
        PushAddrLong((intptr_t)to);
    }

    Writer& operator<<(char c)
    {
        Push(c);
        return *this;
    }

    char const* GetData() const   { return m_buf.data(); }
    SIZE_T      GetLength() const { return m_buf.size(); }

private:
    std::vector<char> m_buf;
    size_t            m_index;
    LPVOID            m_addr;
};

}
