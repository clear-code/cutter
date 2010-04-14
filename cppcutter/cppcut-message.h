/* -*- Mode: C; indent-tabs-mode: nil; c-basic-offset: 4 -*- */
/*
 *  Copyright (C) 2009-2010  Kouhei Sutou <kou@clear-code.com>
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __CPPCUT_MESSAGE_H__
#define __CPPCUT_MESSAGE_H__

#include <sstream>

/**
 * SECTION: cppcut-message
 * @title: Optional assertion message in C++
 * @short_description: Supports optional assertion message
 * in C++.
 *
 */

namespace cut
{
    template <typename Type> inline void
    delegate_shift_operator(std::ostream& stream, const Type& value) {
        stream << value;
    }

    class Message
    {
    public:
        CUT_EXPORT Message();
        CUT_EXPORT Message(const char *format, ...);
        CUT_EXPORT Message(const char *format, va_list args);
        CUT_EXPORT ~Message();

        template <typename Type> inline Message&
        operator <<(const Type& value)
        {
            delegate_shift_operator(buffer_, value);
            return *this;
        }

        template <typename Type> inline Message&
        operator <<(const Type *value)
        {
            if (value == NULL) {
                buffer_ << "(null)";
            } else {
                delegate_shift_operator(buffer_, value);
            }
            return *this;
        }

        CUT_EXPORT inline Message&
        operator <<(bool value)
        {
            return *this << (value ? "true" : "false");
        }

        CUT_EXPORT void printf(const char *format=NULL, ...);

        CUT_EXPORT inline const char *
        string() const
        {
            return buffer_.str().c_str();
        }

    private:
        std::ostringstream buffer_;
    };

    inline std::ostream&
    operator <<(std::ostream& stream, const Message& message) {
        return stream << message.string();
    }
}

#endif /* __CPPCUT_MESSAGE_H__ */

/*
vi:ts=4:nowrap:ai:expandtab:sw=4
*/
