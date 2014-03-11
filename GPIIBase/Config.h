// Config.h

#pragma once

class Config {
public:
	Config(const std::string &filename);
	~Config();

	bool Read();

	template <class T>
	void Set(const std::string& key, const T& value, bool temporary = true) {
		std::stringstream ss;
		ss << value;
		m_store[key] = ss.str();
		if(!temporary) m_order.push_back(key);
	};

	template <class T>
	T Get(const std::string& key, const T& default) {
		auto it = m_store.find(key);
		if(it == m_store.end()) { return default; };
		std::stringstream ss;
		ss << it->second;
		T t; ss >> t;
		return t;
	};

	template <>
	std::string Get(const std::string& key, const std::string& default) {
		auto it = m_store.find(key);
		if(it == m_store.end()) { return default; };
		return it->second;
	};

private:
	std::string m_filename;
	std::fstream m_stream;
	std::vector<std::string> m_order;
	std::map<std::string, std::string> m_store;
};


