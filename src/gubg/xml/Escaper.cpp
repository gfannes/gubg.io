#include <gubg/xml/Escaper.hpp>

namespace gubg { namespace xml { 

	const std::string &Escaper::escape(const std::string &orig)
	{
		tmp_.resize(0);

		auto append = [&](const char *str){
			tmp_ += str;
		};

		for (auto ch: orig)
		{
			switch (ch)
			{
				case '<': append("&lt;"); break;
				case '>': append("&gt;"); break;
				case '"': append("&quot;"); break;
				case '&': append("&amp;"); break;
				case '\'': append("&apos;"); break;

				default: tmp_.push_back(ch); break;
			}
		}

		return tmp_;
	}

	const std::string &Escaper::unescape(const std::string &esc)
	{
		tmp_.resize(0);

		for (auto ix = 0u; ix < esc.size(); ++ix)
		{
			char ch = esc[ix];

			auto translate = [&](const std::string &str, char wanted){
				if (esc.substr(ix, str.size()) != str)
					return false;
				ch = wanted;
				return true;
			};

			if (ch == '&')
				translate("&lt;", '<') || translate("&gt;", '>') || translate("&quot;", '"') || translate("&amp;", '&') || translate("&apos;", '\'');

			tmp_.push_back(ch);
		}

		return tmp_;
	}


} } 