#ifndef HEADER_gubg_OptionParser_hpp_ALREADY_INCLUDED
#define HEADER_gubg_OptionParser_hpp_ALREADY_INCLUDED

#include "gubg/mss.hpp"
#include <string>
#include <functional>
#include <map>
#include <list>
#include <sstream>
#include <memory>

namespace gubg
{
    class OptionParser
    {
        public:
            enum class ReturnCode {OK, Error, CouldNotFindExecutable, ExpectedMandatoryArgument, };

            typedef std::list<std::string> Args;
            static Args create_args(int argc, const char **argv);

            OptionParser(const std::string &caption);

            //shorthand example: "h"
            //longhand example: "help"
			//lambda is called without arguments
            template <typename Lambda>
                void add_switch(char shorthand, const std::string &longhand, const std::string &description, Lambda lambda)
                {
                    switch_callbacks_[shorthand_(shorthand)] = lambda;
                    switch_callbacks_[longhand_(longhand)] = lambda;
                    add_helpline_(shorthand, longhand, description);
                }

            //longhand can be of the form: "<option>" or "<option> <name>"
			//<name> is optional
			//lambda is called with <name> or "" as argument
            template <typename Lambda>
                void add_optional(char shorthand, const std::string &longhand, const std::string &description, Lambda lambda)
                {
                    optional_callbacks_[shorthand_(shorthand)] = lambda;
                    {
                        std::string longOption = longhand;
                        size_t pos = longOption.find(' ');
                        if (pos != std::string::npos)
                            longOption.resize(pos);
                        optional_callbacks_[longhand_(longOption)] = lambda;
                    }
                    add_helpline_(shorthand, longhand, description);
                }

            //longhand has to be of the form: "<option> <name>"
			//<name> is mandatory
			//lambda is called with <name> as argument
            template <typename Lambda>
                void add_mandatory(char shorthand, const std::string &longhand, const std::string &description, Lambda lambda)
                {
                    mandatory_callbacks_[shorthand_(shorthand)] = lambda;
                    {
                        std::string longOption = longhand;
                        size_t pos = longOption.find(' ');
                        if (pos != std::string::npos)
                            longOption.resize(pos);
                        mandatory_callbacks_[longhand_(longOption)] = lambda;
                    }
                    add_helpline_(shorthand, longhand, description);
                }

            std::string help() const;

            ReturnCode parse(Args &args, bool stripExe = true);

        private:
            void add_helpline_(char shorthand, const std::string &longhand, const std::string &description);
            static std::string shorthand_(char sh)
            {
                std::string str(2, '-');
                str[1] = sh;
                return str;
            }
            static std::string longhand_(const std::string &lh)
            {
                if (lh.size() >= 2 && lh.substr(0, 2) == "--")
                    return lh;
                std::string str(lh.size()+2, '-');
                lh.copy(&str[2], lh.size());
                return str;
            }

            std::string caption_;
            std::ostringstream help_;
            typedef std::function<void(void)> VoidCallback;
            typedef std::map<std::string, VoidCallback> VoidCallbacks;
            typedef std::function<void(std::string)> StringCallback;
            typedef std::map<std::string, StringCallback> StringCallbacks;
            VoidCallbacks switch_callbacks_;
            StringCallbacks optional_callbacks_;
            StringCallbacks mandatory_callbacks_;
    };
}

#endif
