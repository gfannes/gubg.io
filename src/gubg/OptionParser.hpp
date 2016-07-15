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

            OptionParser(const std::string &caption);

            //shortHand example: "-h"
            //longHand example: "--help"
			//lambda is called without arguments
            template <typename Lambda>
                void add_switch(const std::string &shortHand, const std::string &longHand, const std::string &description, Lambda lambda)
                {
                    switch_callbacks_[shortHand] = lambda;
                    switch_callbacks_[longHand] = lambda;
                    add_helpline_(shortHand, longHand, description);
                }

            //longHand can be of the form: "--<option>" or "--<option> <name>"
			//<name> is optional
			//lambda is called with <name> or "" as argument
            template <typename Lambda>
                void add_optional(const std::string &shortHand, const std::string &longHand, const std::string &description, Lambda lambda)
                {
                    optional_callbacks_[shortHand] = lambda;
                    {
                        std::string longOption = longHand;
                        size_t pos = longOption.find(' ');
                        if (pos != std::string::npos)
                            longOption.resize(pos);
                        optional_callbacks_[longOption] = lambda;
                    }
                    add_helpline_(shortHand, longHand, description);
                }

            //longHand has to be of the form: "--<option> <name>"
			//<name> is mandatory
			//lambda is called with <name> as argument
            template <typename Lambda>
                void add_mandatory(const std::string &shortHand, const std::string &longHand, const std::string &description, Lambda lambda)
                {
                    mandatory_callbacks_[shortHand] = lambda;
                    {
                        std::string longOption = longHand;
                        size_t pos = longOption.find(' ');
                        if (pos != std::string::npos)
                            longOption.resize(pos);
                        mandatory_callbacks_[longOption] = lambda;
                    }
                    add_helpline_(shortHand, longHand, description);
                }

            std::string help() const;

            typedef std::list<std::string> Args;
            static Args create_args(int argc, const char **argv);

            ReturnCode parse(Args &args, bool stripExe = true);

        private:
            void add_helpline_(const std::string &shortHand, const std::string &longHand, const std::string &description);

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
