#include <gubg/markdown/Reader.hpp>
#include <gubg/mss.hpp>

namespace gubg { namespace markdown { 

	void Reader::Item::heading(unsigned int level)
	{
		clear();
		this->what = HeadingOpen;
		this->level = level;
	}
	void Reader::Item::bullet(unsigned int level)
	{
		clear();
		this->what = BulletOpen;
		this->level = level;
	}
	void Reader::Item::line(const std::string &text)
	{
		clear();
		this->what = Line;
		this->text = text;
	}

	bool Reader::operator()(Item &item)
	{
		MSS_BEGIN(bool);

		if (strange_.empty())
		{
			if (stack_.empty())
				// No more text and no more nested data on the stack_: we are done
				return false;

			item = stack_.back();
			stack_.pop_back();
			return true;
		}

		if (false) ;
		else if (heading_(item)) { }
		else if (bullet_(item)) { }
		else if (Strange line; strange_.pop_line(line))
		{
			item.line(line.str());
		}
		else
		{
			MSS(false, error_("Could not handle content"));
		}

		MSS_END();
	}

	// Helpers
	std::ostream &operator<<(std::ostream &os, const Reader::Item::What &what)
	{
		switch (what)
		{
			case Reader::Item::HeadingOpen: os << "HeadingOpen"; break;
			case Reader::Item::HeadingClose: os << "HeadingClose"; break;
			case Reader::Item::BulletOpen: os << "BulletOpen"; break;
			case Reader::Item::BulletClose: os << "BulletClose"; break;
			case Reader::Item::Line: os << "Line"; break;
		}
		return os;
	}
	std::ostream &operator<<(std::ostream &os, const Reader::Item &item)
	{
		os << "[Item](what:" << item.what << ")(level:" << item.level << ")(text:" << item.text << ")";
		return os;
	}

	// Privates
	void Reader::setup_(std::string &&text)
	{
		strange_ = text;
		stack_.clear();
	}

	void Reader::error_(const std::string &msg)
	{
		if (!error)
			error = msg;
		else
			*error += "\n" + msg;
		*error += "\n" + strange_.str();
	}

	bool Reader::heading_(Item &item)
	{
		const auto sp = strange_;

		auto pop_markdown_heading = [&](unsigned int &level){
			strange_ = sp;
			level = strange_.strip('#');
			return level > 0 && strange_.pop_if(' ');
		};
		
		auto pop_textile_heading = [&](unsigned int &level){
			strange_ = sp;
			return strange_.pop_if('h') && strange_.pop_decimal(level) && strange_.pop_if('.') && strange_.pop_if(' ');
		};

		unsigned int level = 0;
		if (pop_markdown_heading(level) || pop_textile_heading(level))
		{
			// Close all bullets and headings that are nested deeper
			if (stack_has_geq_(Item::BulletClose, 0) || stack_has_geq_(Item::HeadingClose, level))
			{
				item = stack_.back();
				stack_.pop_back();

				strange_ = sp;
				return true;
			}

			item.heading(level);
			strange_.pop_line(item.text);
			stack_.push_back(item);
			stack_.back().what = Item::HeadingClose;
			return true;
		}

		strange_ = sp;
		return false;
	}

	bool Reader::bullet_(Item &item)
	{
		const auto sp = strange_;

		const auto space_count = strange_.strip(" \t");
		const auto star_count = strange_.strip('*');
		if (star_count > 0 && strange_.pop_if(' '))
		{
			const auto level = star_count + space_count;

			// Close all bullets that are nested deeper
			if (stack_has_geq_(Item::BulletClose, level))
			{
				item = stack_.back();
				stack_.pop_back();

				strange_ = sp;
				return true;
			}

			item.bullet(level);
			strange_.pop_line(item.text);
			stack_.push_back(item);
			stack_.back().what = Item::BulletClose;
			return true;
		}

		strange_ = sp;
		return false;
	}

	bool Reader::stack_has_geq_(Item::What what, unsigned int level) const
	{
		for (const auto &item: stack_)
		{
			if (item.what == what && item.level >= level)
				return true;
		}
		return false;
	}

} } 