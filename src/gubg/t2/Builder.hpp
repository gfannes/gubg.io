#ifndef HEADER_gubg_t2_Builder_hpp_ALREADY_INCLUDED
#define HEADER_gubg_t2_Builder_hpp_ALREADY_INCLUDED

#include <gubg/t2/Types.hpp>
#include "gubg/Range.hpp"
#include "gubg/mss.hpp"
#include <cassert>

namespace gubg { namespace t2 { 

    namespace details { 
        struct State
        {
            bool ok = true;
            unsigned int level = 0;
        };
    } 

    template <typename String>
    class Node
    {
    public:
        Node(String &string, details::State &state): string_(string), state_(&state)
        {
            if (state_->level == 0)
                AGG(state_->ok, string_.add(md_som));
            ++state_->level;
        }
        Node(Node &&dying): string_(dying.string_), state_(dying.state_)
        {
            dying.state_ = nullptr;
        }
        ~Node()
        {
            commit();
        }

        bool commit()
        {
            if (!state_)
                return false;
            --state_->level;
            AGG(state_->ok, string_.add(state_->level == 0 ? md_eom : md_close_block));
            if (state_->level == 0)
                state_ = nullptr;
            return true;
        }

        Node tag(Data tag)
        {
            assert(!!state_);
            Node n(string_, *state_);
            add_(md_open_tag, tag);
            return std::move(n);
        }

    private:
        void add_(Byte tt, Data v)
        {
            const auto next = (v >> 6);
            if (next == 0)
            {
                //Last step in the recursion
                AGG(state_->ok, string_.add(tt | v));
                return;
            }
            add_(tt, next);
            AGG(state_->ok, string_.add(md_data | (v & mask_data)));
        }

        String &string_;
        details::State *state_ = nullptr;
    };

    //Keep detail::State before Node<> to make sure it is initialized before Node<>
    template <typename String>
    class Document: private details::State, public Node<String>
    {
    private:
        using State = details::State;
        using Base = Node<String>;

    public:
        Document(String &string): Base(string, *this) {}

        bool commit() { return Base::commit(); }

    private:
        //Do not copy
        Document();
        Document(Document &);
    };

} } 

#endif
