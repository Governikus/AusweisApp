#ifndef FVVERSIONCOMPARATOR_H
#define FVVERSIONCOMPARATOR_H

#include <iosfwd>
#include <vector>


class FvVersionComparator
{
public:

	typedef enum {
		kSame = 0,
		kDescending = 1,
		kAscending = -1
	} ComparatorResult;

	static ComparatorResult CompareVersions(const std::string& versionA,
											const std::string& versionB);

private:

	FvVersionComparator();

	typedef enum {
		kNumberType,
		kStringType,
		kSeparatorType
	} CharacterType;

	static CharacterType TypeOfCharacter(const std::string& character);
	static std::vector<std::string> SplitVersionString(const std::string& version);

};

#endif // FVVERSIONCOMPARATOR_H
