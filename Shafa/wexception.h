#pragma once
#include <string>

namespace shafa {
    class wexception : public std::exception
    {
    public:
        wexception(const std::wstring& msg) : msg_(msg) {}
        virtual ~wexception() {}

        const std::wstring& wwhat() const
        {
            return msg_;
        }

    private:
        std::wstring msg_;
    };

    class wruntime_error : public wexception
    {
    public:
        wruntime_error(const std::wstring& msg) : wexception(msg) {}
    };
}