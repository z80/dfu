#include <iostream>

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/info_parser.hpp>
#include <boost/foreach.hpp>

#define foreach BOOST_FOREACH

void IniParser(const std::string& name, boost::property_tree::ptree& config)
{
	boost::property_tree::read_ini(name, config);
}

void XmlParser(const std::string& name, boost::property_tree::ptree& config)
{
	boost::property_tree::read_xml(name, config);
}

void JsonParser(const std::string& name, boost::property_tree::ptree& config)
{
	boost::property_tree::read_json(name, config);
}

void InfoParser(const std::string& name, boost::property_tree::ptree& config)
{
	boost::property_tree::read_info(name, config);
}

void ParseConfig(const boost::property_tree::ptree& config);

typedef void (*ParserType)(const std::string&, boost::property_tree::ptree&);

template <class Exception>
void ParseFile(const std::string& name, ParserType parser)
{
	boost::property_tree::ptree config;
	try
	{
		parser(name, config);
		ParseConfig(config);
	}
	catch (Exception& error)
	{
		std::cout << error.message() << ": " << error.filename() << ", line " 
			<< error.line() << std::endl;
	}
}

void ParseConfig(const boost::property_tree::ptree& config)
{
	try
	{
		const boost::property_tree::ptree& server = config.get_child("server");
		std::cout 
			<< "\nServer ===================================\n"
			<< "Name:\t" << server.get<std::string>("name") << '\n'
			<< "Port:\t" << server.get("port", 9000) << '\n';
		foreach (const boost::property_tree::ptree::value_type& channel, 
			config.get_child("server.channels"))
		{
			const boost::property_tree::ptree& values = channel.second;
			std::cout 
				<< "\tChannel --------------------------\n"
				<< "\tName:\t" << values.get<std::string>("name") << '\n'
				<< "\tId:\t" << values.get<unsigned>("id") << '\n'
				<< "\tEnabled:" << values.get<bool>("enabled", true) << '\n';
			if (const boost::optional<std::string> optionalComments = 
					values.get_optional<std::string>("comment"))
			{
				std::cout << "\t" << optionalComments.get() << '\n';
			}
		}
	}
	catch (const boost::property_tree::ptree_bad_data& error)
	{
		std::cout << "Bad data: " << error.what() << std::endl;
	}
	catch (const boost::property_tree::ptree_bad_path& error)
	{
		std::cout << "Bad path: " << error.what() << std::endl;
	}
}

int main(int argc, char* argv[])
{
	ParseFile<boost::property_tree::ini_parser_error>("config.ini", IniParser);
	ParseFile<boost::property_tree::xml_parser_error>("config.xml", XmlParser);
	ParseFile<boost::property_tree::json_parser_error>("config.json", JsonParser);
	ParseFile<boost::property_tree::info_parser_error>("config.info", InfoParser);
	return 0;
}
