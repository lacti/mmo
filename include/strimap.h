#ifndef __STRIMAP_H__
#define __STRIMAP_H__

// case-independent (ci) string less_than
// returns true if s1 < s2
struct ci_less : std::binary_function<std::string, std::string, bool> {

	// case-independent (ci) compare_less binary function
	struct nocase_compare : public std::binary_function<unsigned char, unsigned char, bool>  {
		bool operator () (const unsigned char& c1, const unsigned char& c2) const
		{
            return tolower(c1) < tolower(c2);
        }
	};

	bool operator () (const std::string & s1, const std::string & s2) const
	{
		return std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), nocase_compare());
	}
}; // end of ci_less

template <typename T>
struct strimap_t
{
    typedef std::map<std::string, T, ci_less> type;
};


#endif