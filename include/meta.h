#ifndef _META_H
#define _META_H

#include "config.h"

#if defined(_POSIX_C_SOURCE) && (_POSIX_C_SOURCE < 200112L)
#undef _POSIX_C_SOURCE
#endif
#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L
#endif

#include <string>
#include <map>
#include <set>
#include <vector>
#include <deque>
#include <limits>
#include <cstdio>
#include <ctime>
#include <cstring>

#include <fcntl.h>
#include <pthread.h>
#include <strings.h>
#include <stdlib.h>
#include <errno.h>

#define EXTREME_MEMORY_SAVING false

typedef std::string mstring;
typedef const std::string cmstring;

typedef mstring::size_type tStrPos;
const static tStrPos stmiss(cmstring::npos);
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef unsigned char UCHAR;
typedef const char * LPCSTR;

#define citer const_iterator

#define CPATHSEPUNX '/'
#define SZPATHSEPUNIX "/"
#define CPATHSEPWIN '\\'
#define SZPATHSEPWIN "\\"
extern cmstring sPathSep, sPathSepUnix, sDefPortHTTP, sDefPortHTTPS;

#ifdef WINDOWS
#define WIN32
#define SZPATHSEP SZPATHSEPWIN
#define CPATHSEP CPATHSEPWIN
#define szNEWLINE "\r\n"
#else
#define SZPATHSEP SZPATHSEPUNIX
#define CPATHSEP CPATHSEPUNX
#define szNEWLINE "\n"
#endif

// some alternative versions of these flags

#ifndef O_NONBLOCK
#ifdef NOBLOCK
#define O_NONBLOCK NOBLOCK
#else
#ifdef O_NDELAY
#define O_NONBLOCK O_NDELAY
#endif
#endif
#endif

#ifndef O_NONBLOCK
#error "Unknown how to configure non-blocking mode (O_NONBLOCK) on this system"
#endif

#include <sys/socket.h>
#ifndef SO_MAXCONN
#define SO_MAXCONN 250
#endif
#if defined(__linux__)
#include <sys/socketvar.h>
#endif

//#define PATHSEP "/"
int getUUID();

#define SPACECHARS " \f\n\r\t\v"

typedef std::map<mstring, mstring> tStrMap;

inline void trimFront(mstring &s, LPCSTR junk=SPACECHARS)
{
	mstring::size_type pos = s.find_first_not_of(junk);
	if(pos != 0)
		s.erase(0, pos);
}

inline void trimBack(mstring &s, LPCSTR junk=SPACECHARS)
{
	mstring::size_type pos = s.find_last_not_of(junk);
	s.erase(pos+1);
}

inline void trimString(mstring &s, LPCSTR junk=SPACECHARS)
{
	trimBack(s, junk);
	trimFront(s, junk);
}

#define trimLine(x) { trimFront(x); trimBack(x); }

#define startsWith(where, what) (0==(where).compare(0, (what).size(), (what)))
#define endsWith(where, what) ((where).size()>=(what).size() && \
		0==(where).compare((where).size()-(what).size(), (what).size(), (what)))
#define startsWithSz(where, what) (0==(where).compare(0, sizeof((what))-1, (what)))
#define endsWithSzAr(where, what) ((where).size()>=(sizeof((what))-1) && \
		0==(where).compare((where).size()-(sizeof((what))-1), (sizeof((what))-1), (what)))
#define stripSuffix(where, what) if(endsWithSzAr(where, what)) where.erase(where.size()-sizeof(what)+1);
#define stripPrefixChars(where, what) where.erase(0, where.find_first_not_of(what))

#define setIfNotEmpty(where, cand) { if(where.empty() && !cand.empty()) where = cand; }
#define setIfNotEmpty2(where, cand, alt) { if(where.empty()) { if(!cand.empty()) where = cand; else where = alt; } }

mstring GetBaseName(cmstring &in);
mstring GetDirPart(cmstring &in);

void trimProto(mstring & sUri);
tStrPos findHostStart(const mstring & sUri);

#ifndef _countof
#define _countof(x) sizeof(x)/sizeof(x[0])
#endif

#define NAMEWLEN(x) x, (_countof(x)-1)

//extern mstring sPathSep, sPathSepUnix, sCR, sCRLF;

// there is memchr and strpbrk but nothing like the last one acting on specified RAW memory range
static inline LPCSTR  mempbrk (LPCSTR  membuf, char const * const needles, size_t len)
{
   for(LPCSTR pWhere=membuf ; pWhere<membuf+len ; pWhere++)
      for(LPCSTR pWhat=needles; *pWhat ; pWhat++)
         if(*pWhat==*pWhere)
            return pWhere;
   return NULL;
}

typedef std::vector<mstring> tStrVec;
typedef std::set<mstring> tStrSet;
typedef std::deque<mstring> tStrDeq;

// Sometimes I miss Perl...
tStrVec::size_type Tokenize(const mstring &in, LPCSTR sep, tStrVec & out, bool bAppend=false, mstring::size_type nStartOffset=0);
/*inline void Join(mstring &out, const mstring & sep, const tStrVec & tokens)
{out.clear(); if(tokens.empty()) return; for(const auto& tok: tokens)out+=(sep + tok);}
*/

// TODO: __attribute__((externally_visible))
bool ParseKeyValLine(const mstring & sIn, mstring & sOutKey, mstring & sOutVal);
#define keyEq(a, b) (0 == strcasecmp((a), (b).c_str()))


class tHttpUrl
{
public:
	bool SetHttpUrl(cmstring &uri, bool unescape = true);
	mstring ToURI(bool bEscaped) const;
	mstring sHost, sPath, sUserPass;
	bool bIsTransferlEncoded;
#ifdef HAVE_SSL
	bool bSSL;
#endif

	tHttpUrl & operator=(const tHttpUrl &a) 
	{
		sHost=a.sHost; sPort=a.sPort; sPath=a.sPath; sUserPass=a.sUserPass;
#ifdef HAVE_SSL
		bSSL=a.bSSL;
#endif
		bIsTransferlEncoded=a.bIsTransferlEncoded;
		return *this;
	};
	bool operator==(const tHttpUrl &a) const
	{
		return a.sHost==sHost && a.sPort == sPort
				&& a.sPath == sPath && a.sUserPass == sUserPass
#ifdef HAVE_SSL
				&& a.bSSL == bSSL
#endif
				&& a.bIsTransferlEncoded == bIsTransferlEncoded
				;
	};
	bool operator!=(const tHttpUrl &a) const
	{
		return ! (a==*this);
	}
	inline void clear() { sHost.clear(); sPort.clear(); sPath.clear();
	sUserPass.clear();
#ifdef HAVE_SSL
	bSSL=false;
#endif
	bIsTransferlEncoded=false;
	}
	inline tHttpUrl()
	: bIsTransferlEncoded(false)
#ifdef HAVE_SSL
	, bSSL(false)
#endif
	{};

	inline cmstring& GetDefaultPortForProto() const { return
#ifdef HAVE_SSL
			bSSL ? sDefPortHTTPS : sDefPortHTTP;
#else
	sDefPortHTTP;
#endif
	}
	inline cmstring& GetPort() const { return !sPort.empty() ? sPort : GetDefaultPortForProto(); }
private:
	mstring sPort;
};

#define POKE(x) for(;;) { ssize_t n=write(x, "", 1); if(n>0 || (EAGAIN!=errno && EINTR!=errno)) break;  }

#define MIN_VAL(x) (std::numeric_limits<x>::min()) 
#define MAX_VAL(x) (std::numeric_limits<x>::max()) 

void appendLong(mstring &s, long val);

mstring BytesToHexString(const uint8_t b[], unsigned short binLength);
bool CsAsciiToBin(LPCSTR a, uint8_t b[], unsigned short binLength);

typedef const unsigned char CUCHAR;
bool CsEqual(LPCSTR a, uint8_t b[], unsigned short binLength);
void StrSubst(mstring &contents, const mstring &from, const mstring &to);

#if SIZEOF_LONG == 8
// _FILE_OFFSET_BITS mostly irrelevant. But if it's set, watch out for user's "experiments".
#if _FILE_OFFSET_BITS == 32
#error Unsupported: _FILE_OFFSET_BITS == 32 with large long size
#else
#define OFF_T_FMT "%" PRId64
#endif

#else // not a 64bit arch?

#if 64 == _FILE_OFFSET_BITS
#define OFF_T_FMT "%" PRId64
#endif

#if 32 == _FILE_OFFSET_BITS
#define OFF_T_FMT "%" PRId32
#endif

#endif // !64bit arch

#ifndef OFF_T_FMT // either set above or let the os/compiler deal with the mess
#define OFF_T_FMT "%ld"
#endif

// let the compiler optimize and keep best variant
inline off_t atoofft(LPCSTR p)
{
	if(sizeof(long long) == sizeof(off_t))
		return atoll(p);
	if(sizeof(int) == sizeof(off_t))
		return atoi(p);
	return atol(p);
}

inline off_t atoofft(LPCSTR p, off_t nDefVal)
{
	return p ? atoofft(p) : nDefVal;
}

mstring offttosH(off_t n);

tStrDeq ExpandFilePattern(cmstring& pattern, bool bSorted=false);

//void MakeAbsolutePath(mstring &dirToFix, const mstring &reldir);


mstring UrlEscape(cmstring &s);
void UrlEscapeAppend(cmstring &s, mstring &sTarget);
bool UrlUnescapeAppend(cmstring &from, mstring & to);
// Decode with result as return value, no error reporting
inline mstring UrlUnescape(cmstring &from)
{
	mstring ret; // let the compiler optimize
	UrlUnescapeAppend(from, ret);
	return ret;
}
mstring DosEscape(cmstring &s);

#define pathTidy(s) { if(startsWithSz(s, "." SZPATHSEP)) s.erase(0, 2); tStrPos n(0); \
	for(n=0;stmiss!=n;) { n=s.find(SZPATHSEP SZPATHSEP, n); if(stmiss!=n) s.erase(n, 1);}; \
	for(n=0;stmiss!=n;) { n=s.find(SZPATHSEP "." SZPATHSEP, n); if(stmiss!=n) s.erase(n, 2);}; }

// appears in the STL container?
#define ContHas(stlcont, needle) ((stlcont).find(needle) != (stlcont).end())

#define StrHas(haystack, needle) (haystack.find(needle) != stmiss)
#define StrHasFrom(haystack, needle, startpos) (haystack.find(needle, startpos) != stmiss)

off_t GetFileSize(cmstring & path, off_t defret);

inline mstring offttos(off_t n)
{
	char buf[21];
	int len=snprintf(buf, 21, OFF_T_FMT, n);
	return mstring(buf, len);
}

inline mstring ltos(long n)
{
	char buf[21];
	int len=snprintf(buf, 21, "%ld", n);
	return mstring(buf, len);
}

inline mstring offttosH(off_t n)
{
	LPCSTR  pref[]={"", " KiB", " MiB", " GiB", " TiB", " PiB", " EiB"};
	for(UINT i=0;i<_countof(pref)-1; i++)
	{
		if(n<1024)
			return ltos(n)+pref[i];
		if(n<10000)
			return ltos(n/1000)+"."+ltos((n%1000)/100)+pref[i+1];

		n/=1024;
	}
	return "INF";
}

inline void replaceChars(mstring &s, LPCSTR szBadChars, char goodChar)
{
	for(mstring::iterator p=s.begin();p!=s.end();p++)
		for(LPCSTR b=szBadChars;*b;b++)
			if(*b==*p)
			{
				*p=goodChar;
				break;
			}
}

extern cmstring sEmptyString;

//! split-and-extract helper for strings, for convenient use with for-loops
class tSplitWalk
{
	cmstring &s;
	mstring::size_type start, len, oob;
	LPCSTR m_seps;

public:
	inline tSplitWalk(cmstring *line, LPCSTR separators=SPACECHARS)
	: s(*line), start(0), len(stmiss), oob(line->size()), m_seps(separators) {}
	inline bool Next()
	{
		if(len != stmiss) // not initial state, find the next position
			start = start + len + 1;

		if(start>=oob)
			return false;

		start = s.find_first_not_of(m_seps, start);

		if(start<oob)
		{
			len = s.find_first_of(m_seps, start);
			len = (len == stmiss) ? oob-start : len-start;
		}
		else if (len != stmiss) // not initial state, end reached
			return false;
		else if(s.empty()) // initial state, no parts
			return false;
		else // initial state, use the whole string
		{
			start = 0;
			len = oob;
		}

		return true;
	}
	inline mstring str(){ return s.substr(start, len); }
	inline operator mstring() { return str(); }
//	inline LPCSTR rest() { return s.c_str() + start; }
};

//bool CreateDetachedThread(void *(*threadfunc)(void *));

void DelTree(cmstring &what);

bool IsAbsolute(cmstring &dirToFix);



inline char unEscape(const char p)
{
	switch (p)
	{
	case '0':
		return '\0';
	case 'a':
		return '\a';
	case 'b':
		return '\b';
	case 't':
		return '\t';
	case 'n':
		return '\n';
	case 'r':
		return '\r';
	case 'v':
		return '\v';
	case 'f':
		return '\f';
	default:
		return p;
	}
}

inline mstring unEscape(cmstring &s)
{
	mstring ret;
	for(cmstring::const_iterator it=s.begin();it!=s.end();++it)
	{
		if(*it=='\\')
		{
			++it;
			ret+=unEscape(*it);
		}
		else
			ret+=*it;
	}
	return ret;
}

std::string BytesToHexString(const uint8_t sum[], unsigned short lengthBin);

// STFU helpers, (void) casts are not effective for certain functions
static inline void ignore_value (int i) { (void) i; }
static inline void ignore_ptr (void* p) { (void) p; }

static inline time_t GetTime()
{
	return ::time(0);
}

// arbitrary chosen time declaring some date in far future
// -1 is preserved for convenience reasons (avoid clashes with time(2) return code and similar uses)
#ifndef PTHREAD_COND_TIMEDWAIT_TIME_RANGE_OK
#define END_OF_TIME (time_t(MAX_VAL(int))-2)
#else
#define END_OF_TIME (MAX_VAL(time_t)-2)
#endif

static inline UINT FormatTime(char *buf, const time_t cur)
{
	struct tm tmp;
	gmtime_r(&cur, &tmp);
	asctime_r(&tmp, buf);
	//memcpy(buf + 24, " GMT", 4); // wrong, only needed for rfc-822 format, not for asctime's
	//return 28;
	return 24;
}

struct tCurrentTime
{
	char buf[30];
	UINT len;
	inline tCurrentTime() { len=FormatTime(buf, time(NULL)); }
	inline operator mstring() { return mstring(buf, len); }
};

// represents a boolean value like a normal bool but also carries additional data
template <typename Textra, Textra defval>
struct extended_bool
{
	bool value;
	Textra xdata;
	inline operator bool() { return value; }
	inline extended_bool(bool val, Textra xtra = defval) : value(val), xdata(xtra) {};
};

void DelTree(cmstring &what);

struct tErrnoFmter: public mstring
{
	tErrnoFmter(LPCSTR prefix = NULL);
};

mstring EncodeBase64Auth(cmstring & s);

#endif // _META_H

