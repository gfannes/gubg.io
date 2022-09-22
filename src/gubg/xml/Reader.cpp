#include <gubg/xml/Reader.hpp>
#include <gubg/mss.hpp>

namespace gubg { namespace xml { 

	void Reader::setup(const std::string &content)
	{
		strange_ = content;
		check_prolog_ = true;
		emit_top_stack_ = false;
	}

	bool Reader::operator()(Item &item)
	{
		MSS_BEGIN(bool);

		item.kind = Item::Unset;

		if (emit_top_stack_)
		{
			MSS(!tag_stack_.empty(), error_("Could not emit top of stack: it is empty"));
			item.kind = Item::TagClose;
			item.tag = tag_stack_.back();
			tag_stack_.pop_back();
			item.attributes.clear();
			emit_top_stack_ = false;
		}

		while (item.kind == Item::Unset)
		{
			pop_whitespace_();

			if (strange_.empty())
			{
				if (tag_stack_.empty())
					return false;

				item.kind = Item::TagClose;
				item.tag = tag_stack_.back();
				item.attributes.clear();
				tag_stack_.pop_back();
			}
			else
			{
				if (check_prolog_)
				{
					MSS(pop_prolog_(item), error_("Could not handle prolog"));
					check_prolog_ = false;
				}
				else
				{
					if (strange_.pop_if("</"))
					{
						pop_whitespace_();
						MSS(strange_.pop_until(item.tag, '>'), error_("Could not find end of open tag"));
						while (!item.tag.empty() && item.tag.back() == ' ') item.tag.pop_back();
						MSS(!tag_stack_.empty(), error_("Found closing tag '" + item.tag + "' but tag_stack_ is empty"));
						MSS(tag_stack_.back() == item.tag, error_("Found closing tag '" + item.tag + "' but expected '" + tag_stack_.back() + "'"));
						tag_stack_.pop_back();
						item.kind = Item::TagClose;
						item.attributes.clear();
					}
					else if (strange_.pop_if('<'))
					{
						gubg::Strange strange;
						MSS(strange_.pop_to_any(strange, "/>"), error_("Could not find end of open tag"));
						MSS(strange.pop_until(item.tag, ' ') || strange.pop_all(item.tag), error_("Could not find name of open tag"));
						tag_stack_.push_back(item.tag);

						Reader::pop_whitespace_(strange);
						item.attributes.clear();
						while (!strange.empty())
						{
							auto &attribute = item.attributes.emplace_back();
							MSS(Reader::pop_attribute_(attribute, strange, error_), error_("Could not handle tag attribute"));
						}
						item.kind = Item::TagOpen;
						if (strange_.pop_if('/'))
						{
							MSS(strange_.pop_if('>'), error_("Could not find '>' as part of open-close tag"));
							emit_top_stack_ = true;
						}
						else
							MSS(strange_.pop_if('>'), error_("Could not find '>' as part of open tag"));
					}

				}
			}
		}

		MSS_END();
	}

	// Privates
	void Reader::Error::operator()(const std::string &msg)
	{
		if (!error)
			error.emplace();
		*error += msg + '\n';
	}

	void Reader::pop_whitespace_()
	{
		Reader::pop_whitespace_(strange_);
	}
	void Reader::pop_whitespace_(gubg::Strange &strange)
	{
		strange.strip_left(" \t\n\r");
	}

	bool Reader::pop_prolog_(Item &item)
	{
		MSS_BEGIN(bool);

		if (strange_.pop_if("<?xml"))
		{
			item.kind = Item::Prolog;
			gubg::Strange strange;
			MSS(strange_.pop_until(strange, "?>"), error_("Could not find closing prolog marker"));
			Reader::pop_whitespace_(strange);
			while (!strange.empty())
			{
				auto &attribute = item.attributes.emplace_back();
				MSS(Reader::pop_attribute_(attribute, strange, error_), error_("Could not handle prolog attribute"));
			}
		}

		MSS_END();
	}

	bool Reader::pop_attribute_(Item::Attribute &attribute, gubg::Strange &strange, Error &err)
	{
		MSS_BEGIN(bool);

		auto &key = attribute.first;
		MSS(strange.pop_until(key, '='), err("Could not find attribute key"));
		while (!key.empty() && key.back() == ' ') key.pop_back();
		MSS(Reader::unescape_(key, err), err("Could not unescape attribute key"));

		Reader::pop_whitespace_(strange);
		MSS(!strange.empty(), err("Could not find attribute value"));
		char quote;
		strange.pop_char(quote);
		MSS(quote == '"' || quote == '\'', err("Found unexpected attribute quote '" + std::string(1, quote) + "'"));

		auto &value = attribute.second;
		MSS(strange.pop_until(value, quote), err("Could not find closing quote '" + std::string(1, quote) + "'"));
		MSS(Reader::unescape_(value, err), err("Could not unescape attribute value"));

		Reader::pop_whitespace_(strange);

		MSS_END();
	}

	bool Reader::unescape_(std::string &str, Error &err)
	{
		MSS_BEGIN(bool);

		if (str.find('&') != std::string::npos)
		{
			std::string res;

			for (std::string::size_type prev_ix = 0; prev_ix != std::string::npos; )
			{
				auto ix = str.find('&', prev_ix);

				if (ix == std::string::npos)
				{
					res += str.substr(prev_ix);
				}
				else
				{
					auto replace = [&](const std::string &name, char ch){
						if (name != str.substr(ix, name.size()))
							return false;
						res.push_back(ch);
						ix += name.size();
						return true;
					};
					MSS(replace("&amp;", '&')
						|| replace("&lt;", '<')
						|| replace("&gt;", '>')
						|| replace("&apos;", '\'')
						|| replace("&quot;", '"'),
						err("Could not unescape '" + str + "'")
						);
				}

				prev_ix = ix;
			}

			str.swap(res);
		}

		MSS_END();
	}

} } 
