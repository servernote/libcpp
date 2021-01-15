// 静的ツール
//
#include "libcpp.h"

int G::sizeFile( const char *path,time_t *mtime )
{
	struct stat status;

	if( mtime ) *mtime = 0;

	if( strnull(path) ) return (-1);

	if( stat( path, &status ) == 0 ){
		if( mtime ) *mtime = status.st_mtime;
		return status.st_size; //file size
	}
	return (-1); //not exists
}

void G::urlDecode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP
	int i,num; char buf[8];
	const char *c_src = src.c_str();

	for( i = 0; c_src[i] != '\0'; i++ ){
		if( c_src[i] == '%' ){
			if( c_src[i + 1] == '\0' || c_src[i + 2] == '\0' ){
				break; //overflow
			}
			memcpy( buf,c_src + i,3 ); buf[3] = '\0';
			sscanf( buf,"%%%02X",&num );
			if( num > 255 ){
				break; //overflow
			}
			dst.append( 1,(char)num );
			i += 2;
		}
		else if( c_src[i] == '+' ){
			dst.append( 1,' ' );
		}
		else{
			dst.append( 1,c_src[i] );
		}
	}
}

void G::urlEncode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP
	int i; char buf[8];
	const char *c_src = src.c_str();

	for( i = 0; c_src[i] != '\0'; i++ ){
		if( (c_src[i] >= '0' && c_src[i] <= '9') ||
			(c_src[i] >= 'A' && c_src[i] <= 'Z') ||
			(c_src[i] >= 'a' && c_src[i] <= 'z') ||
			c_src[i] == '_' || c_src[i] == '-' || c_src[i] == '.' ){
			dst.append( 1,c_src[i] );
		}
		else if( c_src[i] == ' ' ){
			dst.append( 1,'+' );
		}
		else{
			sprintf( buf,"%%%02X",(unsigned char)(c_src[i]) );
			dst.append( buf,3 );
		}
	}
}

void G::xmlDecode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP

	dst.append(src);
	replace( dst,"&amp;","&" );
	replace( dst,"&quot;","\"" );
	replace( dst,"&lt;","<" );
	replace( dst,"&gt;",">" );
	replace( dst,"&#x27;","'" );
	replace( dst,"&#x60;","`" );
}

void G::xmlEncode( std::string &src,std::string &dst )
{
	if( src.empty() ) return; //NOP
	dst.append(src);
	replace( dst,"&","&amp;" );
	replace( dst,"\"","&quot;" );
	replace( dst,"<","&lt;" );
	replace( dst,">","&gt;" );
	replace( dst,"'","&#x27;" );
	replace( dst,"`","&#x60;" );
}

void G::replace( std::string &str,std::string from,std::string to )
{
	std::string::size_type pos( str.find( from ) );

	while( pos != std::string::npos ){
		str.replace( pos,from.length(),to );
		pos = str.find( from,pos + to.length() );
	}
}

int G::split( const char *src,char key,std::vector<std::string> &dst )
{
	int counts = 0;
	dst.clear();

	if( strnull(src) ) return counts;

	char *p1,*p2; size_t len;

	for( p1 = (char*)src; (p2 = strchr( p1,key )) != NULL ||
		(p2 = strchr( p1,'\0' )) != NULL; p1 = p2 + 1 ){
		len = (size_t)p2 - (size_t)p1;
		if( len > 0 ){
			std::string as = std::string( p1,len );
			dst.push_back( as );
			counts++;
		}
		if( *p2 == '\0' ) break;
	}
	return counts;
}

std::string G::readTextFile( const char *path )
{
	std::string reply = std::string();
	LFile file = LFile();
	file.mPath.append( path );
	file.readTextFile();
	if( !file.mText.empty() ){
		reply.append( file.mText );
	}
	return reply;
}

void G::readConfigFile( std::string path,std::vector<SParam> &params )
{
	char *mem,*p1,*p2,*p3,*p4; int i,n; size_t len; SParam param;

	LFile file = LFile();
	file.mPath = path;
	file.readTextFile();
	if( file.mText.empty() ) return; //read fail
	mem = (char*)file.mText.c_str();

	for( i = 0,p1 = mem; (p2 = strstr( p1,"=\"" )) != NULL; p1 = p2 + 2 ){
		if( p1 == p2 || *(p2 + 2) == '\0' || *(p2 + 2) == '"' ) continue;
		for( p3 = p2 - 1; p3 != mem && *p3 != '\r' && *p3 != '\n'; p3-- ){ ; }
		if( p3 != mem ) p3++; if( *p3 == '#' ) continue;
		if( (p4 = strchr( p2 + 2,'"' )) == NULL ) p4 = strchr( p2 + 2,'\0' );
		len = (size_t)p2 - (size_t)p3;
		if( len <= 0 ) continue;
		param.mName.clear();
		param.mName.append( p3,len );
		p2 += 2; len = (size_t)p4 - (size_t)p2;
		if( len <= 0 ) continue;
		param.mText.clear();
		param.mText.append( p2,len );
		params.push_back( param );
	}
}

void G::readSinglePart( char *mem,size_t len,char sep,std::vector<SParam> &params )
{
	char *fr,*to,*bg,*en; size_t k_len; int i; SParam param;
	std::string ts = std::string();

	for( i = 0,fr = mem; (to = strchr( fr,'=' )) != NULL; fr = to + 1 ){
		k_len = (size_t)to - (size_t)fr;
		bg = to + 1;
		if( (en = strchr( bg,sep )) != NULL ){ *(to = en) = '\0'; }
		else{ en = mem + len; to = en - 1; }
		if( bg[0] != '\0' && k_len > 0 ){ 
			param.mName.clear();
			param.mName.append( fr,k_len );
			ts.clear();
			ts.append( bg );
			param.mText.clear();
			urlDecode( ts,param.mText );
			params.push_back( param );
		}
		if( *(to + 1) == ' ' ) to++;
	}
}

char* G::paramText( const char *key,const std::vector<SParam> &params )
{
	for( int i = 0,n = params.size(); i < n; i++ ){
		if( strcasecmp( params.at( i ).mName.c_str(),key ) == 0 ){
			return (char*)params.at( i ).mText.c_str();
		}
	}
	return NULL;
}

double G::getMicroTime(void) {
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (((double)tv.tv_sec)*((double)1000000)+((double)tv.tv_usec));
}

std::string G::clockText( std::string label )
{
	time_t tm; struct tm date; struct timeval tv; int msec; char tstr[128];

	tm = time( NULL );
	date = *(localtime( &tm ));
	gettimeofday( &tv,NULL );
	msec = (int)(tv.tv_usec / 1000);

	sprintf( tstr,"%04d-%02d-%02d %02d:%02d:%02d.%03d ",
		date.tm_year + 1900,date.tm_mon + 1,
		date.tm_mday,date.tm_hour,date.tm_min,date.tm_sec,msec );

	std::string ret( tstr );
	ret.append( label );
	ret.append( "\n" );
	return ret;
}

void G::getDateText( const struct tm *src,char *dst,size_t len )
{
	if( !src || !dst ) return; //NOP
	if( len <= 0 ) len = 14; //Default

	static const char *mons[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
	static const char *week[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

	if( len == 32 || len == 34 ){ //for cookie or rss-pubDate
		struct tm gmt; memcpy( &gmt,src,sizeof(gmt) );
		if( len == 32 ){ //cookie
			gmt.tm_hour -= 9; mktime( &gmt );
			sprintf( dst,"%s, %d-%s-%04d %02d:%02d:%02d GMT",week[gmt.tm_wday],
				gmt.tm_mday,mons[gmt.tm_mon],gmt.tm_year + 1900,gmt.tm_hour,gmt.tm_min,gmt.tm_sec );
		}
		else{ //rss-pubDate
			sprintf( dst,"%s, %02d %s %04d %02d:%02d:%02d +0900",week[gmt.tm_wday],
				gmt.tm_mday,mons[gmt.tm_mon],gmt.tm_year + 1900,gmt.tm_hour,gmt.tm_min,gmt.tm_sec );
		}
	}
	else if( len == 10 ) sprintf( dst,"%04d-%02d-%02d",src->tm_year + 1900,src->tm_mon + 1,src->tm_mday );
	else if( len == 14 ) sprintf( dst,"%04d%02d%02d%02d%02d%02d",src->tm_year + 1900,src->tm_mon + 1,
		src->tm_mday,src->tm_hour,src->tm_min,src->tm_sec );
	else sprintf( dst,"%04d-%02d-%02d %02d:%02d:%02d",src->tm_year + 1900,src->tm_mon + 1,
		src->tm_mday,src->tm_hour,src->tm_min,src->tm_sec );
}

int G::addText( std::string &str,const char *fmt, ... )
{
	int n; va_list va; char buf[4096];

	va_start( va,fmt );
	n = vsnprintf( buf,sizeof(buf),fmt,va );
	va_end( va );

	if(n <= 0){
		return n;
	}

	str.append(buf);
	return n;
}

int G::addBigText( std::string &str,const char *fmt, ... )
{
	int n; va_list va; char *ptr = NULL;

	va_start( va,fmt );
	n = vasprintf( &ptr,fmt,va );
	va_end( va );

	if( n >= 0 && ptr ){
		str.append( ptr );
		free( ptr );
	}else n = 0;

	return n;
}

// 世界測地系->日本測地系変換
// double lat,lon=度単位10進数 例=35.910722,139.459500
void G::wgs84ToTokyo( double lat_w,double lon_w,double *lat_t,double *lon_t )
{
	*lat_t = (lat_w + 0.00010696 * lat_w - 0.000017467 * lon_w - 0.0046020);
	*lon_t = (lon_w + 0.000046047 * lat_w + 0.000083049 * lon_w - 0.010041);
}

// 日本測地系->世界測地系変換
void G::tokyoToWgs84( double lat_t,double lon_t,double *lat_w,double *lon_w )
{
	*lat_w = (lat_t - 0.00010695 * lat_t + 0.000017464 * lon_t + 0.0046017);
	*lon_w = (lon_t - 0.000046038 * lat_t - 0.000083043 * lon_t + 0.010040);
}

// 2点間の距離を算出(M) ミリ秒INT
int G::getDistance( int lat_fr, int lon_fr, int lat_to, int lon_to ) {

	double xof = lon_to - lon_fr;
	double yof = lat_to - lat_fr;

	if( xof < 0 ) xof *= (-1);
	if( yof < 0 ) yof *= (-1);

	xof *= 2.5; xof /= 100; yof *= 3.0; yof /= 100;

	return (int)sqrt( xof * xof + yof * yof );
}

// 2点間の距離を算出(M) 10進数DOUBLE
double G::getDistance( double lat_fr, double lon_fr, double lat_to, double lon_to ) {

	// ミリ秒表記に変換
	int xfr = (int)(lon_fr * 3600000.0);
	int yfr = (int)(lat_fr * 3600000.0);
	int xto = (int)(lon_to * 3600000.0);
	int yto = (int)(lat_to * 3600000.0);

	double xof = xto - xfr;
	double yof = yto - yfr;

	if( xof < 0 ) xof *= (-1);
	if( yof < 0 ) yof *= (-1);

	xof *= 2.5; xof /= 100; yof *= 3.0; yof /= 100;

	return sqrt( xof * xof + yof * yof );
}

void G::costText( int cost_num,char *cost_str )
{
	char cost[32 + 1],tsoc[32 + 1];
	int i,j,k; size_t len;

	len = sprintf( cost,"%d",cost_num );

	for( i = 0,k = 1,j = (len - 1); j >= 0; i++,k++,j-- ){
		tsoc[i] = cost[j];
		if( k % 3 == 0 && (j - 1) >= 0 ) tsoc[++i] = ',';
	}tsoc[i] = '\0';

	len = strlen( tsoc );
	for( i = 0,j = (len - 1); j >= 0; i++,j-- ){
		cost_str[i] = tsoc[j];
	}
	cost_str[i] = '\0';
}

std::string G::trimingText( const std::string &src )
{
	const char* meta = " \t\v\r\n";
	std::string dst = std::string();

	std::string::size_type left = src.find_first_not_of(meta);

	if (left != std::string::npos){
		std::string::size_type right = src.find_last_not_of(meta);
		dst = src.substr(left, right - left + 1);
	}

	return dst;
}

//ヘッダー文字列作成 全角を2、半角を1として扱う
void G::headerText( const char *src,char *dst,int count )
{
	dst[0] = '\0';
	if( strnull(src) ) return;
	int i,j,k;
	for( i = 0,j = 0,k = 0; src[i] != '\0'; i++ ){
		if( k >= count ){
			break;
		}
		if( (unsigned char)src[i] >= 0x80 ){ //全角
			if( (unsigned char)src[i] >= 0xE0 ){ //3バイト全角
				memcpy( dst + j,src + i,3 );
				i += 2; j += 3;
			}
			else if( (unsigned char)src[i] != 0x5C ){ //2バイト全角(特殊)
				memcpy( dst + j,src + i,2 );
				i++; j += 2;
			}
			else{ //1バイト全角(特殊)
				dst[j++] = src[i];
			}
			k += 2;
		}
		else{ //半角
			dst[j++] = src[i];
			k++;
		}
	}
	dst[j] = '\0';
	if( src[i] != '\0' ){
		strcat( dst,"..." );
	}
}

static struct{ char han; const char *zen; }gHanZenTbl[] = {
	{' ',"　"},{'!',"！"},{'"',"”"},{'#',"＃"},{'$',"＄"},{'%',"％"},{'&',"＆"},{'\'',"’"},
	{'(',"（"},{')',"）"},{'*',"＊"},{'+',"＋"},{',',"、"},{'-',"－"},{'.',"。"},{'/',"／"},
	{':',"："},{';',"；"},{'<',"＜"},{'=',"＝"},{'>',"＞"},{'?',"？"},{'@',"＠"},{'[',"［"},
	{'\\',"￥"},{']',"］"},{'^',"＾"},{'_',"＿"},{'`',"‘"},{'{',"｛"},{'|',"｜"},{'}',"｝"},{'~',"～"}
};

//可能な限りの半角→全角変換
char* G::hankakuToZenkaku( const char *src,char *dst )
{
	int i,j,x,y;
	if( dst ) dst[0] = '\0';
	if( strnull(src) ) return NULL;
	if( !dst && (dst = (char*)malloc( strlen( src ) * 3 + 1 )) == NULL ) return NULL;

	for( i = 0,j = 0; src[i] != '\0'; i++ ){
		if( (unsigned char)src[i] >= 0x80 ){ //全角
			if( (unsigned char)src[i] >= 0xE0 ){
				dst[j++] = src[i];
				dst[j++] = src[i + 1];
				dst[j++] = src[i + 2];
				i += 2;
			}else if( (unsigned char)src[i] != 0x5C ){
				dst[j++] = src[i];
				dst[j++] = src[i + 1];
				i++;
			}else{
				dst[j++] = src[i];
			}
		}else{ //半角現る
			if( src[i] >= 'A' && src[i] <= 'Z' ){
				dst[j++] = 0xEF;
				dst[j++] = 0xBC;
				dst[j++] = 0xA1 + (src[i] - 'A');
			}
			else if( src[i] >= 'a' && src[i] <= 'z' ){
				dst[j++] = 0xEF;
				dst[j++] = 0xBD;
				dst[j++] = 0x81 + (src[i] - 'a');
			}
			else if( src[i] >= '0' && src[i] <= '9' ){
				dst[j++] = 0xEF;
				dst[j++] = 0xBC;
				dst[j++] = 0x90 + (src[i] - '0');
			}
			else{
				for( x = 0; x < countof(gHanZenTbl); x++ ){
					if( src[i] == gHanZenTbl[x].han ){
						y = strlen( gHanZenTbl[x].zen );
						memcpy( dst + j,gHanZenTbl[x].zen,y );
						j += y; break;
					}
				}
				if( x >= countof(gHanZenTbl) ) dst[j++] = src[i]; //GIVE UP
			}
		}
	}dst[j] = '\0';
	return dst;
}

//可能な限りの全角→半角変換
char* G::zenkakuToHankaku( const char *src,char *dst )
{
	int i,j,x;
	if( dst ) dst[0] = '\0';
	if( strnull(src) ) return NULL;
	if( !dst && (dst = (char*)malloc( strlen( src ) + 1 )) == NULL ) return NULL;

	for( i = 0,j = 0; src[i] != '\0'; i++ ){
		if( (unsigned char)src[i] < 0x80 ){ //半角
			dst[j++] = src[i]; continue;
		}
		if( (unsigned char)src[i] == 0xEF &&
			(unsigned char)src[i + 1] == 0xBC &&
			(unsigned char)src[i + 2] >= 0x90 &&
			(unsigned char)src[i + 2] <= 0x99 ){
			dst[j++] = '0' + ((unsigned char)src[i + 2] - 0x90);
			i += 2; continue;
		}
		if( (unsigned char)src[i] == 0xEF &&
			(unsigned char)src[i + 1] == 0xBC &&
			(unsigned char)src[i + 2] >= 0xA1 &&
			(unsigned char)src[i + 2] <= 0xBA ){
			dst[j++] = 'A' + ((unsigned char)src[i + 2] - 0xA1);
			i += 2; continue;
		}
		if( (unsigned char)src[i] == 0xEF &&
			(unsigned char)src[i + 1] == 0xBD &&
			(unsigned char)src[i + 2] >= 0x81 &&
			(unsigned char)src[i + 2] <= 0x9A ){
			dst[j++] = 'a' + ((unsigned char)src[i + 2] - 0x81);
			i += 2; continue;
		}
		for( x = 0; x < countof(gHanZenTbl); x++ ){
			if( strncmp( &(src[i]),gHanZenTbl[x].zen,strlen( gHanZenTbl[x].zen ) ) == 0 ){
				dst[j++] = gHanZenTbl[x].han; break;
			}
		}
		if( (unsigned char)src[i] >= 0xE0 ){
			if( x >= countof(gHanZenTbl) ){
				dst[j++] = src[i];
				dst[j++] = src[i + 1];
				dst[j++] = src[i + 2];
			}i += 2;
		}else{
			if( x >= countof(gHanZenTbl) ){
				dst[j++] = src[i];
				dst[j++] = src[i + 1];
			}i++;
		}
	}dst[j] = '\0';
	return dst;
}

//Base64エンコード
unsigned char* G::base64Encode( const unsigned char *str,int length,int *ret_length )
{
	static const char base64_table[] = {
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
        'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
        'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
	};
	static const char base64_pad = '=';
    	const unsigned char *current = str;
        unsigned char *p;
        unsigned char *result;

        if ((length + 2) < 0 || ((length + 2) / 3) >= (1 << (sizeof(int) * 8 - 2))) {
                if (ret_length != NULL) {
                        *ret_length = 0;
                }
                return NULL;
        }

        result = (unsigned char *)malloc(((length + 2) / 3) * 4 + 128);
        p = result;

        while (length > 2) { /* keep going until we have less than 24 bits */
                *p++ = base64_table[current[0] >> 2];
                *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
                *p++ = base64_table[((current[1] & 0x0f) << 2) + (current[2] >> 6)];
                *p++ = base64_table[current[2] & 0x3f];

                current += 3;
                length -= 3; /* we just handle 3 octets of data */
        }

        /* now deal with the tail end of things */
        if (length != 0) {
                *p++ = base64_table[current[0] >> 2];
                if (length > 1) {
                        *p++ = base64_table[((current[0] & 0x03) << 4) + (current[1] >> 4)];
                        *p++ = base64_table[(current[1] & 0x0f) << 2];
                        *p++ = base64_pad;
                } else {
                        *p++ = base64_table[(current[0] & 0x03) << 4];
                        *p++ = base64_pad;
                        *p++ = base64_pad;
                }
        }
        if (ret_length != NULL) {
                *ret_length = (int)(p - result);
        }
        *p = '\0';
        return result;
}
