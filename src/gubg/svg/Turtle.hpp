#ifndef HEADER_gubg_svg_Turtle_hpp_ALREADY_INCLUDED
#define HEADER_gubg_svg_Turtle_hpp_ALREADY_INCLUDED

#include "gubg/xml/Builder.hpp"
#include "gubg/V2.hpp"
#include <ostream>
#include <vector>

namespace gubg { namespace svg { 

    class Turtle
    {
    public:
        void down() { actions_.push_back(Action::create_down()); }
        void up() { actions_.push_back(Action::create_up()); }
        void run(double distance) { actions_.push_back(Action::create_run(distance)); }

        void stream(std::ostream &os, unsigned int x_width, unsigned int y_width) const
        {
            using namespace gubg::xml::builder;
            {
                Header header(os);
                header.attr("version", "1.0");
                header.attr("encoding", "UTF-8");
            }
            {
                Comment(os) << "Created by gubg" << " (http://www.github.com/gfannes/gubg)";
            }
            Tag svg(os, "svg");
            svg.attr("width", x_width);
            svg.attr("height", y_width);
            auto g = svg.tag("g");
            auto path = svg.tag("path");

            std::ostringstream oss;
            double d = 10;
            auto M = [&](int x, int y){oss << "M" << d*x << "," << d*y << " ";};
            auto m = [&](int x, int y){oss << "m" << d*x << "," << d*y << " ";};
            auto arc = [&](int x, int y, bool b){oss << "a" << d*2 << "," << d*2 << " 0 0," << (b?1:0) << d*x << "," << d*y << " ";};
            auto a = [&](int x, int y){arc(x, y, true);};
            auto aa = [&](int x, int y){arc(x, y, false);};
            auto l = [&](int x, int y){oss << "l" << d*x << "," << d*y << " ";};

            M(10,10);

            V2 pos;
            double angle = 0.0;
            bool down = false;
            for (const auto &action: actions_)
            {
                switch (action.cmd)
                {
                    case Pen:
                        down = action.down;
                        break;
                    case Run:
                        {
                            const auto dir = V2::from_frac(angle, action.distance);
                            pos += dir;
                            if (down)
                                l(pos.x, pos.y);
                            else
                                m(pos.x, pos.y);
                        }
                        break;
                    case Turn:
                        {
                            const auto dir = V2::from_frac(angle, action.distance);
                            const auto ortho = V2::ortho(dir, action.angle >= 0);
                        }
                        break;
                }
            }
            g.attr("d", oss.str()).attr("fill", "none").attr("stroke", "blue");
        }

    private:
        enum Command
        {
            Pen, Run, Turn,
        };
        struct Action
        {
            Command cmd;
            bool down = false;
            double distance = 0.0;
            double angle = 0.0;
            double radius = 0.0;

            Action(){}
            Action(Command cmd): cmd(cmd) {}

            static Action create_down()
            {
                Action action(Pen);
                action.down = true;
                return action;
            }
            static Action create_up()
            {
                Action action(Pen);
                action.down = false;
                return action;
            }
            static Action create_run(double distance)
            {
                Action action(Run);
                action.distance = distance;
                return action;
            }
        };
        using Actions = std::vector<Action>;
        Actions actions_;
    };

} } 

#endif
