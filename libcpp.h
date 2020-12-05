// 共通
//
#ifndef __libcpp_h__
#define __libcpp_h__

//C
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/rand.h>

//C++
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <thread>
#include <mutex>
#include <algorithm>
#include <functional>
#include <unordered_map>
#include <utility>

//defines
#define strnull(x) (x == NULL || x[0] == '\0')
#define setnull(x) (x[0] = '\0')
#define countof(x) sizeof(x) / sizeof(x[0])

//structs
typedef struct {
	std::string mName;
	std::string mText;
}SParam;

typedef struct {
	std::string mName;
	double mValue;
}SNumber;

//classes

// ファイル
class LFile
{
public:
	std::string mPath;
	std::string mText;
	std::vector<uint8_t> mData;
	FILE *mFp;

	LFile( void );
	virtual	~LFile( void );
	bool open( const char *mode );
	void close( void );
	void readTextFile( void );
	void readDataFile( void );
};

// ソケット通信
#define dSocketTimeout 10

class LSocket
{
public:
	std::string mHost;
	std::string mPort;
	int mHandle;
	SSL_CTX *mSSLCtx;
	SSL *mSSL;
	std::vector<uint8_t> mData;
	std::string mHead;
	std::vector<uint8_t> mBody;

	LSocket( void );
	virtual	~LSocket( void );

	bool connect( void );
	void disconnect( void );
	void setTimeout( int second );
	int writeString( std::string &str );
	void read( void );
	bool split( void );
};

#ifdef _USE_MYSQL
// MySQL
#include <mysql/mysql.h>

#define dMaxDbConn 4 //同時にコネクション張れるハンドル数

class LMySQL
{
public:
	std::string &mLogStr;
	char *mDBHost;
	char *mDBUser;
	char *mDBPass;
	char *mDBName;
	int mDBPort;

	MYSQL *mDB[dMaxDbConn]; 

	LMySQL( std::string &logstr );
	virtual	~LMySQL( void );

	bool connectDB( int db_index );
	void disconnectDB( int db_index );
	MYSQL_RES* readDB( int db_index,std::string query );
	bool writeDB( int db_index,std::string query );
	int countDB( int db_index,std::string query );
	const char* errorDB( int db_index );
	char* escapeDB( int db_index,const char *src );
};

#endif //_USE_MYSQL

#ifdef _USE_PGSQL
// PostgreSQL
#include <postgresql/libpq-fe.h>

class LPgSQL {
public:
	const char *mParam;
	int mStatus;
	char mMessage[512];
	PGconn *mConn;
	PGresult *mResult;
	int mTuples;

	LPgSQL(const char *param);
	virtual	~LPgSQL(void);
	bool connect(void);
	bool exec(const char *sql);
};

#endif //_USE_PGSQL

// 静的ツール
class G
{
public:
	static int sizeFile( const char *path,time_t *mtime );
	static void urlDecode( std::string &src,std::string &dst );
	static void urlEncode( std::string &src,std::string &dst );
	static void xmlDecode( std::string &src,std::string &dst );
	static void xmlEncode( std::string &src,std::string &dst );
	static void replace( std::string &str,std::string from,std::string to );
	static int split( const char *src,char key,std::vector<std::string> &dst );
	static std::string readTextFile( const char *path );
	static void readConfigFile( std::string path,std::vector<SParam> &params );
	static void readSinglePart( char *mem,size_t len,char sep,std::vector<SParam> &params );
	static char* paramText( const char *key,const std::vector<SParam> &params );
	static double getMicroTime(void);
	static std::string clockText( std::string label );
	static void getDateText( const struct tm *src,char *dst,size_t len );
	static int addText( std::string &str,const char *fmt, ... );
	static int addBigText( std::string &str,const char *fmt, ... );
	static void wgs84ToTokyo( double lat_w,double lon_w,double *lat_t,double *lon_t );
	static void tokyoToWgs84( double lat_t,double lon_t,double *lat_w,double *lon_w );
	static double getDistance( double lat_fr, double lon_fr, double lat_to, double lon_to );
	static void costText( int cost_num,char *cost_str );
	static std::string trimingText( const std::string &src );
	static void headerText( const char *src,char *dst,int count );
	static char* hankakuToZenkaku( const char *src,char *dst );
	static char* zenkakuToHankaku( const char *src,char *dst );
	static unsigned char* base64Encode( const unsigned char *str,int length,int *ret_length );
};

// CGIプログラムクラス
class LCgi
{
public:
	std::string mLogPath;
	std::string mLogStr;

	std::vector<SParam> mConfig;
	std::vector<SParam> mCgi;
	std::vector<SParam> mCookie;

	struct tm mDate;
	time_t mTime;
	char* mDateStr;

	std::string mClientIp;

	std::string mOutHead;
	std::string mOutBody;

	LCgi( int argc,char **argv );
	LCgi( void );
	virtual	~LCgi( void );
	virtual void run( void );
	virtual void finish( void );
	void readCgiParams( void );
	void readCookieParams( void );
};

#endif //__libcpp_h__
