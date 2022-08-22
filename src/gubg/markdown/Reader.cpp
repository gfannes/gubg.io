#include <gubg/markdown/Reader.hpp>
#include <gubg/mss.hpp>

namespace gubg { namespace markdown { 

	void Reader::Item::heading(unsigned int level, bool is_open)
	{
		clear();
		this->what = Heading;
		this->is_open = is_open;
		this->level = level;
	}
	void Reader::Item::bullet(unsigned int level, bool is_open)
	{
		clear();
		this->what = Bullet;
		this->is_open = is_open;
		this->level = level;
	}
	void Reader::Item::text(const std::string &str)
	{
		clear();
		this->what = Text;
		this->str = str;
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
			item.is_open = false;
			stack_.pop_back();
			return true;
		}

		if (false) ;
		else if (heading_(item)) { }
		else if (bullet_(item)) { }
		else if (Strange line; strange_.pop_line(line))
		{
			item.text(line.str());
		}
		else
		{
			MSS(false, error_("Could not handle content"));
		}

		MSS_END();
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

		if (const auto level = strange_.strip('#'); level > 0 && strange_.pop_if(' '))
		{
			// Close all bullets and headings that are nested deeper
			if (stack_has_geq_(Item::Bullet, 0) || stack_has_geq_(Item::Heading, level))
			{
				item = stack_.back();
				item.is_open = false;
				stack_.pop_back();

				strange_ = sp;
				return true;
			}

			item.heading(level, true);
			stack_.push_back(item);
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
			if (stack_has_geq_(Item::Bullet, level))
			{
				item = stack_.back();
				item.is_open = false;
				stack_.pop_back();

				strange_ = sp;
				return true;
			}

			item.bullet(level, true);
			stack_.push_back(item);
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