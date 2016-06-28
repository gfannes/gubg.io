#include "gubg/parse/xml/Coding.hpp"
#include "gubg/Strange.hpp"
#include <cctype>
using namespace std;

namespace gubg { namespace xml {

    ReturnCode encode(string &enc, const string &raw)
    {
        MSS_BEGIN(ReturnCode);

        //We compute the size after encoding to avoid repetitive reallocations
        size_t encSize = raw.size();
        for (auto ch: raw)
        {
            switch (ch)
            {
                case '"': encSize += 5; break;
                case '<': encSize += 3; break;
                case '>': encSize += 3; break;
                case '&': encSize += 4; break;
                default:
                          MSS(mss::on_fail(std::isprint(ch) != 0 || ch == '\n' || ch == '\r' || ch == '\t', ReturnCode::NonPrintable));
                          break;
            }
        }

        if (encSize == raw.size())
            //No escaping is necessary, the raw string _is_ the encoded string
            enc = raw;
        else
        {
            enc.resize(encSize);
            auto dst = enc.begin();
            for (auto ch: raw)
            {
                assert(dst != enc.end());
                switch (ch)
                {
                    case '"': *dst++ = '&'; *dst++ = 'q'; *dst++ = 'u'; *dst++ = 'o'; *dst++ = 't'; *dst++ = ';'; break;
                    case '<': *dst++ = '&'; *dst++ = 'l'; *dst++ = 't'; *dst++ = ';'; break;
                    case '>': *dst++ = '&'; *dst++ = 'g'; *dst++ = 't'; *dst++ = ';'; break;
                    case '&': *dst++ = '&'; *dst++ = 'a'; *dst++ = 'm'; *dst++ = 'p'; *dst++ = ';'; break;
                    default:  *dst++ = ch; break;
                }
            }
            assert(dst == enc.end());
        }

        MSS_END();
    }

    ReturnCode decode(string &raw, const string &enc, QuoteStrategy qs)
    {
        MSS_BEGIN(ReturnCode);

        //We compute the size after decoding
        size_t rawSize = enc.size();
        {
            gubg::Strange strange(enc);
            char ch;
            while (!strange.empty())
            {
                if (strange.pop_if('&'))
                {
                    if (false) {}
                    else if (strange.pop_if("quot;"))
                    {
                        MSS(rawSize >= 5);
                        rawSize -= 5;
                    }
                    else if (strange.pop_if("lt;"))
                    {
                        MSS(rawSize >= 3);
                        rawSize -= 3;
                    }
                    else if (strange.pop_if("gt;"))
                    {
                        MSS(rawSize >= 3);
                        rawSize -= 3;
                    }
                    else if (strange.pop_if("amp;"))
                    {
                        MSS(rawSize >= 4);
                        rawSize -= 4;
                    }
                    else if (strange.pop_if("apos;"))
                    {
                        MSS(rawSize >= 5);
                        rawSize -= 5;
                    }
                    else if (strange.pop_if("#xa;") || strange.pop_if("#xA;") || strange.pop_if("#xd;") || strange.pop_if("#xD;"))
                    {
                        MSS(rawSize >= 4);
                        rawSize -= 4;
                    }
                    else if (strange.pop_if("#x2018;") || strange.pop_if("#x2019;") || strange.pop_if("#x2026;") || strange.pop_if("#x201c;") || strange.pop_if("#x201d;"))
                    {
                        MSS(rawSize >= 7);
                        rawSize -= 7;
                    }
                    else
                        MSS(ReturnCode::UnknownEscape);
                }
                else
                {
                    const auto ret = strange.pop_char(ch);
                    assert(ret);
                    switch (qs)
                    {
                        case DisallowQuote:
                            MSS(mss::on_fail(ch != '"' && ch != '<' && ch != '>' && ch != '&', ReturnCode::UnexpectedRawChar));
                            break;
                        case AllowQuote:
                            MSS(mss::on_fail(ch != '<' && ch != '>' && ch != '&', ReturnCode::UnexpectedRawChar));
                            break;
                    }
                }
            }
        }

        if (rawSize == enc.size())
            //The encoded string _is_ the raw string
            raw = enc;
        else
        {
            raw.resize(rawSize);
            auto dst = raw.begin();
            char ch;
            gubg::Strange strange(enc);
            while (!strange.empty())
            {
                if (strange.pop_if('&'))
                {
                    if (false) {}
                    else if (strange.pop_if("quot;") || strange.pop_if("#x201c;") || strange.pop_if("#x201d;"))
                        *dst++ = '"';
                    else if (strange.pop_if("lt;"))
                        *dst++ = '<';
                    else if (strange.pop_if("gt;"))
                        *dst++ = '>';
                    else if (strange.pop_if("amp;"))
                        *dst++ = '&';
                    else if (strange.pop_if("apos;"))
                        *dst++ = '\'';
                    else if (strange.pop_if("#xa;") || strange.pop_if("#xA;"))
                        *dst++ = '\n';
                    else if (strange.pop_if("#xd;") || strange.pop_if("#xD;"))
                        *dst++ = '\r';
                    else if (strange.pop_if("#x2018;"))
                        *dst++ = '\'';
                    else if (strange.pop_if("#x2019;"))
                        *dst++ = '\'';
                    else if (strange.pop_if("#x2026;"))
                        //Elipsis
                        *dst++ = '.';
                    else
                    {
                        //Should have failed above
                        assert(false);
                    }
                }
                else
                {
                    strange.pop_char(ch);
                    *dst++ = ch;
                }
            }
            assert(dst == raw.end());
        }

        MSS_END();
    }

} }
