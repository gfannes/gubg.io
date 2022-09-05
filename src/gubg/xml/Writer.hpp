#ifndef HEADER_gubg_xml_Writer_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xml_Writer_hpp_ALREADY_INCLUDED

#include <gubg/xml/writer/Prolog.hpp>
#include <gubg/xml/writer/Comment.hpp>
#include <gubg/xml/writer/Tag.hpp>
#include <ostream>

namespace gubg { namespace xml { 

	class Writer
	{
	public:
		using Prolog = writer::Prolog;
		using Comment = writer::Comment;
		using Tag = writer::Tag;

		Writer(std::ostream &os): os_(os) {}

		template <typename ...Rest>
		Prolog prolog(Rest... rest)
		{
			Prolog p{os_};
			prolog_(p, rest...);
			return std::move(p);
		}

		template <typename ...Rest>
		Comment comment(Rest... rest)
		{
			Comment c{os_};
			comment_(c, rest...);
			return std::move(c);
		}

		Tag tag(const std::string &name);

	private:
		void prolog_(Prolog &p) { }
		template <typename Key, typename Value, typename ...Rest>
		void prolog_(Prolog &p, const Key &key, const Value &value, Rest... rest)
		{
			p.attr(key, value);
			prolog_(p, rest...);
		}

		void comment_(Comment &c) { }
		template <typename Part, typename ...Rest>
		void comment_(Comment &c, const Part &part, Rest... rest)
		{
			c << part;
			comment_(c, rest...);
		}


		std::ostream &os_;
	};

} } 

#endif