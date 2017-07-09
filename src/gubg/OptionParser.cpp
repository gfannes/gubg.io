#include "gubg/OptionParser.hpp"
#include "gubg/mss.hpp"
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

        VoidCallbacks::const_iterator itVoid;
        StringCallbacks::const_iterator itString;
        if ((itVoid = switch_callbacks_.find(arg)) != switch_callbacks_.end())
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

void OptionParser::add_helpline_(char shorthand, const string &longhand, const string &description)
{
    help_ << shorthand_(shorthand) << "\t" << longhand_(longhand) << "\t" << description << endl;
}
string OptionParser::help() const
{
    return caption_ + "\n" + help_.str();
}
