// ファイル
//
#include "libcpp.h"

LFile::LFile( void )
{
	mFp = NULL;
}

LFile::~LFile( void )
{
	close();
}

bool LFile::open( const char *mode )
{
	struct flock flk;

	if( !mode || mPath.empty() ) return false; //NOP

	close();

	if( (mFp = fopen( mPath.c_str(),mode )) != NULL ){
		flk.l_type	= F_WRLCK;
		flk.l_whence= SEEK_SET;
		flk.l_start	= 0;
		flk.l_len	= 0;
		if( fcntl( fileno( mFp ),F_SETLKW,&flk ) != 0 ){
			fclose( mFp );
			mFp = NULL;
		}
	}
	return mFp ? true:false;
}

void LFile::close( void )
{
	int ret; struct flock flk;

	if( !mFp ) return;

	flk.l_type	= F_UNLCK;
	flk.l_whence= SEEK_SET;
	flk.l_start	= 0;
	flk.l_len	= 0;
	ret = fcntl( fileno( mFp ),F_SETLKW,&flk );
	fclose( mFp );

	mFp = NULL;
}

void LFile::readTextFile( void )
{
	if( mPath.empty() ) return; //NOP
	mText.clear();

	std::ifstream fin = std::ifstream( mPath.c_str() );

	if( !fin ){
		return; //Open Error
	}

	std::stringstream strstream = std::stringstream();

	strstream << fin.rdbuf();
	fin.close();

	mText.append( strstream.str() );
}

void LFile::readDataFile( void )
{
	if( mPath.empty() ) return; //NOP
	mData.clear();

	FILE *fp; uint8_t *ptr = NULL;
	size_t len = 0; int ret = 0;

	if( (fp = fopen( mPath.c_str(),"rb" )) != NULL ){
		fseek( fp,0L,SEEK_END );
		len = (size_t)ftell( fp );
		mData.resize( len );
		ptr = mData.data();
		rewind( fp );
		ret = fread( ptr,len,1,fp ); //一気読み
		fclose( fp );
	}
}
