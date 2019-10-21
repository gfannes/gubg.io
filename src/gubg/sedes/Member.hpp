#ifndef HEADER_gubg_sedes_Member_hpp_ALREADY_INCLUDED
#define HEADER_gubg_sedes_Member_hpp_ALREADY_INCLUDED

#include <memory>

namespace gubg { namespace sedes { 

    class Field;
    class Optional;
    class Array;

    class Visitor
    {
    public:
        virtual void visit(const Field &) = 0;
        virtual void visit(const Optional &) = 0;
        virtual void visit(const Array &) = 0;
    };

    class Member
    {
    public:
        using Ptr = std::shared_ptr<Member>;

        std::string name;
        std::string type;

        bool is_primitive() const
        {
            return type == "float" || type == "int" || type == "char";
        }

        virtual void accept(Visitor &) const = 0;

    private:
    };

    class Field: public Member
    {
    public:
        template <typename Ftor>
        static Member::Ptr create(Ftor &&ftor)
        {
            auto ptr = std::make_shared<Field>();
            ftor(*ptr);
            return ptr;
        }

        void accept(Visitor &visitor) const override { visitor.visit(*this); }

    private:
    };

    class Optional: public Member
    {
    public:
        template <typename Ftor>
        static Member::Ptr create(Ftor &&ftor)
        {
            auto ptr = std::make_shared<Optional>();
            ftor(*ptr);
            return ptr;
        }

        void accept(Visitor &visitor) const override { visitor.visit(*this); }

    private:
    };

    class Array: public Member
    {
    public:
        template <typename Ftor>
        static Member::Ptr create(Ftor &&ftor)
        {
            auto ptr = std::make_shared<Array>();
            ftor(*ptr);
            return ptr;
        }

        void accept(Visitor &visitor) const override { visitor.visit(*this); }

    private:
    };

} } 

#endif
