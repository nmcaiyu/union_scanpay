#pragma once
#include "cJSON.h"
#include "map"

using namespace std;

#pragma comment(lib, "version.lib")

#define  BUFCLR(x) memset(x, 0x00, sizeof(x))

namespace Tool{
	class Tools
	{
	public:
		Tools();
		~Tools();

		/* 获取时间 */
		static void GetTimestamp(char *szTimestamp);
		static void GetDateTime(char *szDateTime);
		/* 句柄处理 */
		static HMODULE GetSelfModuleHandle();
		static void GetCurrFileVersion(char *p);

		/* JSON处理 */
		static int GetJsonString(cJSON *jsonRoot, char *szTag, int iValueLen, char *szValue);
		static int GetJsonCString(cJSON *jsonRoot, char *szTag, CString &strValue);
		static int GetJsonInt(cJSON *jsonRoot, char *szTag);

		/* 字符串处理 */
		static size_t strlcpy(char *destination, const char *source, const size_t size);
		static char* Trim(char *str);
		static void GBKToUTF8(char *szGBK, char *szOutUTF8);
		static void UTF8ToGBK(char *szUTF8, char *szOutGBK);
		static CString UTF8ToGBK(const char *szUTF8);
		static CString GBKToUTF8(const char *szGBK);
		static void asc_to_bcd(unsigned char *bcd_buf, unsigned char *ascii_buf, int conv_len, bool bIsLetter);
		static void bcd_to_asc(unsigned char *ascii_buf, unsigned char *bcd_buf, int conv_len, bool bIsLetter);
		static int URLEncode(const char* str, const int strSize, char* result, const int resultSize);
		static int TI_iCmpStruct(const void *a, const void *b);
		static void DeleteCDATA(char *pDesxml, char *pSrcxml);

		
		static int is_valid_ip(const char *ip_str);

		static BOOL IsDigit2(string str);
	};
}
