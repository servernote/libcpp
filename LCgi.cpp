// CGIプログラムクラス
//
#include "libcpp.h"

LCgi::LCgi( int argc,char **argv )
{
	mLogStr.append( G::clockText( "BEGINNING OF CGI" ) );

	char *p;

	readCgiParams();
	readCookieParams();

	time_t tm = time( NULL );
	mDate = *(localtime( &tm ));
	mDate.tm_sec = 0; //秒は常に0
	mTime = mktime( &mDate );
	mDateStr = NULL;

	if( (p = G::paramText( "dgdate",mCgi )) != NULL && strlen( p ) == 12 ){
		mDateStr = p; //YYYYMMDDHHMM
		sscanf( p,"%04d%02d%02d%02d%02d",&mDate.tm_year,&mDate.tm_mon,
			&mDate.tm_mday,&mDate.tm_hour,&mDate.tm_min );
		mDate.tm_year -= 1900; 
		mDate.tm_mon--;
		mTime = mktime( &mDate );
	}

	if( (p = getenv( "HTTP_X_FORWARDED_FOR" )) != NULL ||
		(p = getenv( "REMOTE_ADDR" )) != NULL ){
		mClientIp.append( p );
	}
	else{
		mClientIp.append( "127.0.0.1" );
	}
	//mClientIp = std::string( "182.171.232.49, 64.252.172.75" );

	std::string::size_type n = mClientIp.find(',');
	if( n != std::string::npos ){
		mClientIp = mClientIp.substr( 0,n );
	}
}

LCgi::~LCgi( void )
{

}

void LCgi::run( void )
{

}

void LCgi::finish( void )
{
	mLogStr.append( G::clockText( "ENTER PRINT" ) );

	if( !mOutHead.empty() ){
		std::cout << mOutHead;
		mLogStr.append( mOutHead );
	}
	if( !mOutBody.empty() ){
		std::cout << mOutBody;
		mLogStr.append( mOutBody );
	}

	mLogStr.append( G::clockText( "LEAVE PRINT" ) );

	mLogStr.append( G::clockText( "END OF CGI" ) );

	if( !mLogPath.empty() && !mLogStr.empty() ){
		std::ofstream logfile( mLogPath );
		logfile << mLogStr;
		logfile.close();
	}
}

void LCgi::readCgiParams( void )
{
	char *ptr,*mem = NULL; size_t len = 0,frn;

	if( (ptr = getenv( "QUERY_STRING" )) != NULL && (len = strlen( ptr )) > 0 ){
		mem = strdup( ptr );
	}else if( (ptr = getenv( "CONTENT_LENGTH" )) != NULL && (len = atoi( ptr )) > 0 ){
		if( (mem = (char*)malloc( len + 1 )) != NULL ){
			frn = fread( mem,len,1,stdin ); mem[len] = '\0';
		}
	}
	if( !mem ) return; //NOT CGI

	mLogStr.append( mem,len );
	mLogStr.append( "\n" );

	G::readSinglePart( mem,len,'&',mCgi );

}

void LCgi::readCookieParams( void )
{
	char *ptr,*mem = NULL; size_t len = 0; int i,j;

	if( (ptr = getenv( "HTTP_COOKIE" )) != NULL && (len = strlen( ptr )) > 0 &&
		(mem = (char*)malloc( len + 32 )) != NULL ){
		for( i = 0,j = 0; ptr[i] != '\0'; i++ ){
			if( ptr[i] == ';' && ptr[i + 1] == ' ' ){
				mem[j++] = ptr[i]; i++;
			}
			else{
				mem[j++] = ptr[i];
			}
		}
		mem[j] = '\0'; len = j;
		mLogStr.append( mem,len);
		mLogStr.append( "\n" );
		G::readSinglePart( mem,len,';',mCookie );
	}
}
