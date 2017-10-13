#ifndef HEADER_gubg_svg_Turtle_hpp_ALREADY_INCLUDED
#define HEADER_gubg_svg_Turtle_hpp_ALREADY_INCLUDED

#include "gubg/xml/Builder.hpp"
#include "gubg/V2.hpp"
#include <sstream>
#include <vector>

namespace gubg { namespace svg { 

    class Turtle
    {
    public:
        Turtle & down() { actions_.push_back(Action::create_down()); return *this; }
        Turtle & up() { actions_.push_back(Action::create_up());  return *this;}
        Turtle & run(double distance) { actions_.push_back(Action::create_run(distance));  return *this;}
        Turtle & turn(double angle_frac, double radius = 0.0) { actions_.push_back(Action::create_turn(angle_frac, radius));  return *this;}
        Turtle & zoom(double zoom) { actions_.push_back(Action::create_zoom(zoom));  return *this;}
        Turtle & to(double x, double y, double angle_frac) { actions_.push_back(Action::create_to(x,y, angle_frac));  return *this;}
        Turtle & color(const std::string &color) { actions_.push_back(Action::create_color(color));  return *this;}

        bool stream(std::ostream &os, unsigned int x_width, unsigned int y_width) const
        {
            MSS_BEGIN(bool);
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

            std::ostringstream oss;
            double zoom = 1.0;
            auto trans = [&](const V2 &pos){return V2(zoom*pos.x, zoom*(y_width-pos.y));};

            auto mm = [&](const V2 &p){const auto pos = trans(p); oss << "M" << pos.x << "," << pos.y << " ";};
            auto m = [&](const V2 &p){const auto pos = trans(p); oss << "m" << pos.x << "," << pos.y << " ";};
            auto arc = [&](const V2 &p, bool b){const auto pos = trans(p); oss << "a" << zoom*2 << "," << zoom*2 << " 0 0," << (b?1:0) << pos.x << "," << pos.y << " ";};
            auto a = [&](const V2 &p){arc(p, true);};
            auto aa = [&](const V2 &p){arc(p, false);};
            auto l = [&](const V2 &p){const auto pos = trans(p); oss << "l" << pos.x << "," << pos.y << " ";};
            auto ll = [&](const V2 &p){const auto pos = trans(p); oss << "L" << pos.x << "," << pos.y << " ";};

            V2 start_pos;
            V2 pos;
            std::string color = "black";

            auto commit = [&](){
                const auto str = oss.str();
                if (!str.empty())
                {
                    oss.str("");
                    mm(start_pos);
                    start_pos = pos;
                    auto path = g.tag("path");
                    path.attr("d", oss.str()+str).attr("fill", "none").attr("stroke", color);
                    oss.str("");
                }
            };

            double angle_frac = 0.0;
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
                            const auto dir = V2::from_frac(angle_frac, action.distance);
                            pos += dir;
                            if (down)
                                ll(pos);
                            else
                                mm(pos);
                        }
                        break;
                    case Turn:
                        if (action.radius == 0.0)
                        {
                            angle_frac += action.angle_frac;
                        }
                        else
                        {
                            const auto dir = V2::from_frac(angle_frac, action.distance);
                            const auto ortho = V2::ortho(dir, action.angle_frac >= 0);
                        }
                        break;
                    case Zoom:
                        {
                            zoom = action.zoom;
                        }
                        break;
                    case To:
                        {
                            pos = action.pos;
                            if (down)
                                ll(pos);
                            else
                                mm(pos);
                            angle_frac = action.angle_frac;
                        }
                        break;
                    case Color:
                        if (color != action.color)
                        {
                            commit();
                            color = action.color;
                        }
                        break;
                    default:
                        MSS(false, std::cout << "Error: Unknown command " << C(action.cmd, int) << std::endl);
                        break;
                }
            }
            commit();
            MSS_END();
        }

    private:
        enum Command
        {
            Pen, Run, Turn, Zoom, To, Color,
        };
        struct Action
        {
            Command cmd;
            bool down = false;
            double distance = 0.0;
            V2 pos;
            double angle_frac = 0.0;
            double radius = 0.0;
            double zoom = 0.0;
            std::string color;

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
            static Action create_turn(double angle_frac, double radius)
            {
                Action action(Turn);
                action.angle_frac = angle_frac;
                action.radius = radius;
                return action;
            }
            static Action create_zoom(double zoom)
            {
                Action action(Zoom);
                action.zoom = zoom;
                return action;
            }
            static Action create_to(double x, double y, double angle_frac)
            {
                Action action(To);
                action.pos.x = x;
                action.pos.y = y;
                action.angle_frac = angle_frac;
                return action;
            }
            static Action create_color(const std::string &color)
            {
                Action action(Color);
                action.color = color;
                return action;
            }
        };
        using Actions = std::vector<Action>;
        Actions actions_;
    };

} } 

#endif
