#include <gubg/naft/Reader.hpp>
#include <gubg/mss.hpp>

namespace gubg { namespace naft { 

	std::string Reader::Item::key() const
	{
		const auto ix = text.find(':');
		if (ix == std::string::npos)
			return text;
		return text.substr(0, ix);
	}

	std::string Reader::Item::value() const
	{
		const auto ix = text.find(':');
		if (ix == std::string::npos)
			return "";
		return text.substr(ix+1);
	}

	void Reader::setup(const std::string &str)
	{
		strange_ = str;
		state_ = State::Block;
		error.reset();
	}

	bool Reader::operator()(Item &item)
	{
		MSS_BEGIN(bool);

		for (bool again = true; again; )
		{
			L(C(strange_.str())C(state_));

			again = false;

			MSS(!error);

			pop_whitespace_();

			if (strange_.empty())
			{
				if (stack_.empty())
					return false;
				item.what = Item::NodeClose;
				item.text = stack_.back();
				stack_.pop_back();
			}
			else
			{
				if (state_ == State::Attributes)
				{
					if (strange_.pop_if('('))
					{
						item.what = Item::Attribute;
						MSS(strange_.pop_until(item.text, ')'), error_("Could not find matching ')' for " + strange_.str()));
					}
					else if (strange_.pop_if('{'))
					{
						state_ = State::Block;
						again = true;
					}
					else
					{
						item.what = Item::NodeClose;
						MSS(!stack_.empty());
						item.text = stack_.back();
						stack_.pop_back();
						state_ = State::Block;
					}
				}
				else
				{
					if (strange_.pop_if('['))
					{
						item.what = Item::NodeOpen;
						MSS(strange_.pop_until(item.text, ']'), error_("Could not find matching ']' for " + strange_.str()));
						stack_.push_back(item.text);
						state_ = State::Attributes;
					}
					else if (strange_.pop_if('}'))
					{
						item.what = Item::NodeClose;
						MSS(!stack_.empty());
						item.text = stack_.back();
						stack_.pop_back();
						state_ = State::Block;
					}
					else
					{
						item.what = Item::Text;
						strange_.pop_to_any(item.text, "[}") || strange_.pop_all(item.text);
						if (item.text.empty())
							again = true;
					}
				}
			}
		}

		MSS_END();
	}

	// Helper functions
    std::ostream &operator<<(std::ostream &os, const Reader::Item::What &what)
    {
    	switch (what)
    	{
    		case Reader::Item::What::NodeOpen: os << "NodeOpen"; break;
    		case Reader::Item::What::NodeClose: os << "NodeClose"; break;
    		case Reader::Item::What::Attribute: os << "Attribute"; break;
    		case Reader::Item::What::Text: os << "Text"; break;
    	}
    	return os;
    }
    std::ostream &operator<<(std::ostream &os, const Reader::State &state)
    {
    	switch (state)
    	{
    		case Reader::State::Block: os << "Block"; break;
    		case Reader::State::Attributes: os << "Attributes"; break;
    	}
    	return os;
    }
    std::ostream &operator<<(std::ostream &os, const Reader::Item &item)
    {
    	os << "[Item](what:" << item.what << ")(text:" << item.text << ")";
    	return os;
    }

	// Privates
	void Reader::pop_whitespace_()
	{
		strange_.strip_left(" \t\n\r");
	}

	bool Reader::error_(const std::string &msg)
	{
		if (!error)
			error = msg;
		else
			*error += "\n" + msg;
		return false;
	}

} } 
