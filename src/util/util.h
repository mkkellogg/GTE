/*
 * class: Util
 *
 * author: Mark Kellogg
 *
 * General utility class
 *
 */

#ifndef _UTIL_H_
#define _UTIL_H_

//forward declarations
class Matrix4x4;

#include <string>

class Util
{
	protected:

	Util();
	~Util();

	public:

	static std::string TrimLeft(const std::string& str);
	static std::string TrimRight(const std::string& str);
	static std::string Trim(const std::string& str);
	static void PrintMatrix(const Matrix4x4& matrix);
};

#endif
