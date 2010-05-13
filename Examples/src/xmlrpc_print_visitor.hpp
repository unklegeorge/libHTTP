
class print_visitor : 
	public boost::static_visitor<void>
{
public:
	print_visitor(int level = 0) : tab_level(level) {}

    void operator()(const int& i) const
    {
		print_tabs();
		std::cout << i << "\r\n";
    }
    void operator()(const double& d) const
    {
		print_tabs();
		std::cout << d << "\r\n";
    }
    void operator()(const std::string& str) const
    {
		print_tabs();
		std::cout << str << "\r\n";
    }
    void operator()(const std::vector<char>& vec) const
    {
		std::string b64plain;
		std::copy(vec.begin(), vec.end(), std::back_inserter(b64plain));
		
		print_tabs();
		std::cout << b64plain << "\r\n";
    }
    void operator()(const boost::posix_time::ptime& tim) const
    {
		print_tabs();
		std::cout << tim << "\r\n";
    }
	void operator()(const http::xmlrpc::param_vec& params) const
	{
		print_tabs();
		std::cout << "array" << "\r\n";
		
		tab_level++;		
		for(std::vector<http::xmlrpc::param>::const_iterator it=params().begin(); it!=params().end(); ++it)
		{
			boost::apply_visitor(print_visitor(tab_level), *it);
		}
		tab_level--;		
	}
   void operator()(const http::xmlrpc::param_map& map) const
    {		
		print_tabs();
		std::cout << "struct" << "\r\n";
		
		tab_level++;
		for(std::map<std::string, http::xmlrpc::param>::const_iterator it=map().begin(); it!=map().end(); ++it)
		{
			print_tabs();
			std::cout << (*it).first << "\r\n";
			
			boost::apply_visitor(print_visitor(tab_level), (*it).second);
		}
		tab_level--;
    }
	
private:
	void print_tabs() const
	{
		for(int i=0; i<tab_level; i++)
			std::cout << "-- ";
	}
	mutable int tab_level;
};
