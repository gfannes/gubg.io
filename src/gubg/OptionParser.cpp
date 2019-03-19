#include "gubg/OptionParser.hpp"
#include "gubg/mss.hpp"
#include <fstream>

using namespace gubg;
using namespace std;

OptionParser::OptionParser(const string &caption):
    caption_(caption){}

OptionParser::Args OptionParser::create_args(int argc, const char **argv)
{
	Args args;
	if (argv)
	{
		for (int i = 0; i < argc; ++i)
			args.push_back(argv[i]);
	}
	return args;
}

OptionParser::ReturnCode OptionParser::parse(Args &args, bool stripExe)
{
    MSS_BEGIN(ReturnCode);
    if (stripExe)
    {
        MSS(mss::on_fail(!args.empty(), ReturnCode::CouldNotFindExecutable));
        args.pop_front();
    }

    //Pop arguments one by one and look for corresponding callbacks
    while (!args.empty())
    {
        auto arg = args.front();
        args.pop_front();

        // should we split the argument (because it contains a '=')?
        {
            std::size_t s = arg.find('=');
            if (s != std::string::npos)
            {
                // push the value in the list (this can be an empty string)
                args.push_front(arg.substr(s+1));
                arg = arg.substr(0,s);
            }
        }

        VoidCallbacks::const_iterator itVoid;
        StringCallbacks::const_iterator itString;
        if (false) {}
        else if (is_response_(arg))
        {
            MSS(add_response_args_(arg, args), return ReturnCode::CouldNotFindResponse);
        }
        else if ((itVoid = switch_callbacks_.find(arg)) != switch_callbacks_.end())
            (itVoid->second)();
        else if ((itString = mandatory_callbacks_.find(arg)) != mandatory_callbacks_.end())
        {
            if (args.empty())
            {
                args.push_front(arg);
                MSS(ReturnCode::ExpectedMandatoryArgument);
            }
            (itString->second)(args.front());
            args.pop_front();
        }
        else if ((itString = optional_callbacks_.find(arg)) != optional_callbacks_.end())
        {
            if (args.empty() || args.front().empty() || '-' == args.front()[0])
                (itString->second)("");
            else
            {
                (itString->second)(args.front());
                args.pop_front();
            }
        }
        else
        {
            args.push_front(arg);
            break;
        }
    }
    MSS_END();
}
bool OptionParser::add_response_args_(const string & arg, Args & args) const
{
    MSS_BEGIN(bool);

    std::string fn;
    if (arg[0] == '@')
    {
        fn = arg.substr(1);
    }
    else
    {
        MSS(!args.empty());
        fn = args.front();
        args.pop_front();
    }

    std::ifstream ifs(fn);
    MSS(ifs.good());

    Args new_args;
    {
        std::string v;
        while(std::getline(ifs, v))
            new_args.push_back(v);
    }

    args.splice(args.begin(), new_args);

    MSS_END();
}

bool OptionParser::is_response_(const string & arg) const
{
    // possibilities are
    if(arg.empty() || !response_)
        return false;
    
    // maybe it is a filename directly after the shorthand
    if (arg[0] == response_->shorthand)
        return true;

    if (arg == response_->longhand)
        return true;

    return false;
}

void OptionParser::add_helpline_(char shorthand, const string &longhand, const string &description)
{
    help_ << shorthand_(shorthand) << "\t" << longhand_(longhand) << "\t" << description << endl;
}
string OptionParser::help() const
{
    return caption_ + "\n" + help_.str();
}
