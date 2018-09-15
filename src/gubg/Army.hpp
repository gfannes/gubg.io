#ifndef HEADER_gubg_Army_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Army_hpp_ALREADY_INCLUDED

#include <ostream>
#include <iomanip>

namespace gubg { 

    // Army | Minutes
    //------|--------
    // 30   | 30
    // 100  | 60
    // 230  | 150
    class Army
    {
    public:
        unsigned int as_minutes() const {return minutes_;}
        unsigned int as_army() const {return (minutes_/60)*100 + (minutes_%60);}

        Army &from_minutes(unsigned int minutes) {minutes_ = minutes; return *this;}
        Army &from_army(unsigned int army) {minutes_ = (army/100)*60 + (army%100); return *this;}

    private:
        unsigned int minutes_ = 0;
    };

    std::ostream &operator<<(std::ostream &os, const Army &army)
    {
        const auto minutes = army.as_minutes();
        const auto ch = os.fill();
        os << minutes/60 << ':' << std::setw(2) << std::setfill('0') << minutes%60;
        os.fill(ch);
        return os;
    }

} 

#endif
