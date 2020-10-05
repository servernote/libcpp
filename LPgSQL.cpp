#ifdef _USE_PGSQL
// PostgreSQL
//
#include "libcpp.h"

LPgSQL::LPgSQL(const char *param) {
	mParam = param;
	mStatus = 0;
	mMessage[0] = '\0';
	mConn = NULL;
	mResult = NULL;
	mTuples = 0;
}

LPgSQL::~LPgSQL(void) {
	if(mResult){
		PQclear(mResult);
	}
	if(mConn){
		PQfinish(mConn);
	}
}

bool LPgSQL::connect(void) {
	if(mConn){
		return true;
	}
	if(!mParam){
		mStatus = (-1);
		return false;
	}
	mConn = PQconnectdb( mParam );
	if(!mConn || PQstatus( mConn ) == CONNECTION_BAD ){
		mStatus = (-3);
		return false;
	}
	return true;
}

bool LPgSQL::exec(const char *sql) {
	if(mResult){
		PQclear(mResult);
		mResult = NULL;
	}
	if(!connect()){
		return false;
	}
	mStatus = 0;
	int n = 0;
	mTuples = 0;
	mResult = PQexec(mConn,sql);
	if(!mResult || PQresultStatus( mResult ) != PGRES_TUPLES_OK || (n =PQntuples(mResult)) <= 0){
		if(mResult){
			PQclear(mResult);
			mResult = NULL;
		}
		mStatus = (-31);
		return false;
	}
	mTuples = n;
	return true;
}

#endif //_USE_PGSQL
