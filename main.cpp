// テストメイン
//
#include "libcpp.h"

int main( int argc,char **argv )
{
	LFile f = LFile();
	f.mPath = "Makefile";
	f.readTextFile();

	std::cout << f.mText << std::endl;

	return 0;
}
