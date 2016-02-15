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
                if (strange.popCharIf('&'))
                {
                    if (false) {}
                    else if (strange.popStringIf("quot;"))
                    {
                        MSS(rawSize >= 5);
                        rawSize -= 5;
                    }
                    else if (strange.popStringIf("lt;"))
                    {
                        MSS(rawSize >= 3);
                        rawSize -= 3;
                    }
                    else if (strange.popStringIf("gt;"))
                    {
                        MSS(rawSize >= 3);
                        rawSize -= 3;
                    }
                    else if (strange.popStringIf("amp;"))
                    {
                        MSS(rawSize >= 4);
                        rawSize -= 4;
                    }
                    else if (strange.popStringIf("apos;"))
                    {
                        MSS(rawSize >= 5);
                        rawSize -= 5;
                    }
                    else if (strange.popStringIf("#xa;") || strange.popStringIf("#xA;") || strange.popStringIf("#xd;") || strange.popStringIf("#xD;"))
                    {
                        MSS(rawSize >= 4);
                        rawSize -= 4;
                    }
                    else if (strange.popStringIf("#x2018;") || strange.popStringIf("#x2019;") || strange.popStringIf("#x2026;") || strange.popStringIf("#x201c;") || strange.popStringIf("#x201d;"))
                    {
                        MSS(rawSize >= 7);
                        rawSize -= 7;
                    }
                    else
                        MSS(ReturnCode::UnknownEscape);
                }
                else
                {
                    const auto ret = strange.popChar(ch);
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
                if (strange.popCharIf('&'))
                {
                    if (false) {}
                    else if (strange.popStringIf("quot;") || strange.popStringIf("#x201c;") || strange.popStringIf("#x201d;"))
                        *dst++ = '"';
                    else if (strange.popStringIf("lt;"))
                        *dst++ = '<';
                    else if (strange.popStringIf("gt;"))
                        *dst++ = '>';
                    else if (strange.popStringIf("amp;"))
                        *dst++ = '&';
                    else if (strange.popStringIf("apos;"))
                        *dst++ = '\'';
                    else if (strange.popStringIf("#xa;") || strange.popStringIf("#xA;"))
                        *dst++ = '\n';
                    else if (strange.popStringIf("#xd;") || strange.popStringIf("#xD;"))
                        *dst++ = '\r';
                    else if (strange.popStringIf("#x2018;"))
                        *dst++ = '\'';
                    else if (strange.popStringIf("#x2019;"))
                        *dst++ = '\'';
                    else if (strange.popStringIf("#x2026;"))
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
                    strange.popChar(ch);
                    *dst++ = ch;
                }
            }
            assert(dst == raw.end());
        }

        MSS_END();
    }

} }
