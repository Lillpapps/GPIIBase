// Config.cpp

#include "stdafx.h"
#include "Config.h"

Config::Config(const std::string &filename)
: m_filename(filename)
{
	
};

Config::~Config() {
	m_stream.open(m_filename.c_str(), std::ios_base::out);
	auto it = m_order.begin();
	while(it != m_order.end()) {
		m_stream << (*it) << " " << m_store[(*it)] << std::endl;
		m_stream.flush();
		++it;
	};
	m_store.clear();
	m_stream.close();
};

bool Config::Read() {
	m_stream.open(m_filename.c_str(), std::ios_base::beg|std::ios_base::in);
	if(!m_stream.is_open()) throw std::exception("Could not open config file!");

	std::string line, key, value;
	while(!m_stream.eof()) {
		std::getline(m_stream, line);
		if(line.length() == 0) { continue; };
		std::string::size_type pos = line.find_first_of(' ');
		key = line.substr(0, pos);
		value = line.substr(pos + 1);
		m_store.insert( std::pair<std::string,std::string>(key, value) );
		m_order.push_back(key);
	};
	m_stream.close();
	return true;
};

/*void Config::operator()(const std::string& key, const std::string& value, bool temporary) {
	m_store[key] = value;
	if(!temporary) m_order.push_back(key);
};*/
