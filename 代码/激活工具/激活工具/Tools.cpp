#include "stdafx.h"
#include "Tools.h"
using namespace Tool;
extern "C" { FILE _iob[3] = { __iob_func()[0], __iob_func()[1], __iob_func()[2] }; }

Tools::Tools()
{
}


Tools::~Tools()
{
}

void Tools::GBKToUTF8(char *szGBK, char *szOutUTF8)
{
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, szGBK, -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, szGBK, -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	memcpy(szOutUTF8, str2, n);
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
}

void Tools::UTF8ToGBK(char *szUTF8, char *szOutGBK)
{
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, str1, n);
	n = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_ACP, 0, str1, -1, str2, n, NULL, NULL);
	memcpy(szOutGBK, str2, n);
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
}

CString Tools::UTF8ToGBK(const char *szUTF8)
{
	CString OutGBK;
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_UTF8, 0, szUTF8, -1, str1, n);
	n = WideCharToMultiByte(CP_ACP, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n + 2];
	memset(str2, 0, n + 2);
	WideCharToMultiByte(CP_ACP, 0, str1, -1, str2, n, NULL, NULL);
	OutGBK = str2;
	//memcpy(szOutGBK, str2, n);
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return OutGBK;
}

CString Tools::GBKToUTF8(const char *szGBK)
{
	CString OutUTF8;
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, szGBK, -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, szGBK, -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n + 2];
	memset(str2, 0, n + 2);
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	OutUTF8 = str2;
	//memcpy(szOutUTF8, str2, n);
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return OutUTF8;
}

//转换的长度皆以ASC码的长度为准
void Tools::asc_to_bcd(unsigned char *bcd_buf, unsigned char *ascii_buf, int conv_len, bool bIsLetter)
{
	int cnt;
	char ch, ch1;
	ch1 = 0;

	for (cnt = 0; cnt < conv_len; ascii_buf++, cnt++)
	{
		if (*ascii_buf >= 'a' || *ascii_buf >= 'A')
		{
			if (!bIsLetter)
			{
				ch = *ascii_buf - 'A' + 10;
			}
			else
			{
				ch = *ascii_buf - 'a' + 10;
			}
		}
		else if (*ascii_buf >= '0')
		{
			ch = *ascii_buf - '0';
		}
		else
		{
			ch = 0;
		}
		if (ch1 == 0x55)
		{
			ch1 = ch;
		}
		else
		{
			*bcd_buf++ = (ch1 << 4) | ch;
			ch1 = 0x55;
		}
	}
	if (ch1 != 0x55)
	{
		*bcd_buf = ch1 << 4;
	}
}

//转换的长度皆以ASC码的长度为准
void Tools::bcd_to_asc(unsigned char *ascii_buf, unsigned char *bcd_buf, int conv_len, bool bIsLetter)
{
	int cnt;
	cnt = 0;
	for (; cnt < conv_len; cnt++, ascii_buf++)
	{
		*ascii_buf = ((cnt & 0x01) ? (*bcd_buf++ & 0x0f) : (*bcd_buf >> 4));
		if (!bIsLetter)
		{
			*ascii_buf += ((*ascii_buf > 9) ? ('A' - 10) : '0');
		}
		else
		{
			*ascii_buf += ((*ascii_buf > 9) ? ('a' - 10) : '0');
		}
	}
}

int Tools::URLEncode(const char* str, const int strSize, char* result, const int resultSize)
{
	int i;
	int j = 0;//for result index
	char ch;

	if ((str == NULL) || (result == NULL) || (strSize <= 0) || (resultSize <= 0))
	{
		return 0;
	}

	for (i = 0; (i < strSize) && (j < resultSize); ++i)
	{
		ch = str[i];
		if (((ch >= 'A') && (ch <= 'Z')) ||
			((ch >= 'a') && (ch <= 'z')) ||
			((ch >= '0') && (ch <= '9')))
		{
			result[j++] = ch;
		}
		else if (ch == ' ')
		{
			result[j++] = '+';
		}
		else if (ch == ':' || ch == ',' || ch == '.' || ch == '-' || ch == '_' || ch == '*' || ch == '{' || ch == '}' || ch == '"')
		{
			result[j++] = ch;
		}
		else
		{
			if (j + 3 < resultSize)
			{
				sprintf(result + j, "%%%02x", (unsigned char)ch);
				j += 3;
			}
			else
			{
				return 0;
			}
		}
	}

	result[j] = '\0';
	return j;
}

void Tools::GetTimestamp(char *szTimestamp)
{
	time_t t = time(0);
	sprintf(szTimestamp, "%ld", t);
}


void Tools::GetDateTime(char *szDateTime)
{
	struct tm *ptm = NULL;
	time_t currtime;
	memset(&currtime, 0x00, sizeof(currtime));
	time(&currtime);
	ptm = localtime(&currtime);
	sprintf(szDateTime, "%04d%02d%02d%02d%02d%02d",
		ptm->tm_year + 1900, ptm->tm_mon + 1, ptm->tm_mday,
		ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
	return;
}

HMODULE Tools::GetSelfModuleHandle()
{
	MEMORY_BASIC_INFORMATION mbi;
	return ((::VirtualQuery(GetSelfModuleHandle, &mbi, sizeof(mbi)) != 0) ? (HMODULE)mbi.AllocationBase : NULL);
}

int Tools::TI_iCmpStruct(const void *a, const void *b)
{
	CString *strA = (CString*)a;
	CString *strB = (CString*)b;
	return strA->Compare(*strB);
}

char* Tools::Trim(char *str)
{
	int i, j, k;

	for (i = 0; str[i] == ' ' && str[i] != '\0'; i++);
	for (j = strlen(str) - 1; (str[j] == ' ') && (j >= 0); j--);
	for (k = 0; k <= j - i; k++)
		str[k] = str[i + k];

	str[k] = 0;

	return str;
}

int Tools::GetJsonString(cJSON *jsonRoot, char *szTag, int iValueLen, char *szValue)
{
	if (jsonRoot == NULL)
	{
		return 0;
	}
	cJSON *node;
	if ((node = cJSON_GetObjectItem(jsonRoot, szTag)) != NULL && node->valuestring != NULL)
	{
		if (strlen(node->valuestring) != 0)
		{
			strlcpy(szValue, node->valuestring, iValueLen);
			return 0;
		}
	}
	return -1;
}

int Tools::GetJsonCString(cJSON *jsonRoot, char *szTag, CString &strValue)
{
	if (jsonRoot == NULL)
	{
		return 0;
	}
	cJSON *node;
	if ((node = cJSON_GetObjectItem(jsonRoot, szTag)) != NULL && node->valuestring != NULL)
	{
		if (strlen(node->valuestring) != 0)
		{
			strValue = node->valuestring;
			return 0;
		}
	}
	return -1;
}

int Tools::GetJsonInt(cJSON *jsonRoot, char *szTag)
{
	cJSON *node;
	if ((node = cJSON_GetObjectItem(jsonRoot, szTag)) != NULL)
	{
		return node->valueint;
	}
	return -1;
}

size_t Tools::strlcpy(char *destination, const char *source, const size_t size)
{
	if (source == NULL)
	{
		return 0;
	}
	if (0 < size)
	{
		_snprintf(destination, size, "%s", source);
		/*
		* Platforms that lack strlcpy() also tend to have
		* a broken snprintf implementation that doesn't
		* guarantee nul termination.
		*
		* XXX: the configure script should detect and reject those.
		*/
		destination[size - 1] = '\0';
	}
	return strlen(source);
}

void Tools::GetCurrFileVersion(char *p)
{
	char szVersion[100];
	VS_FIXEDFILEINFO *pVerInfo = NULL;
	DWORD dwTemp, dwSize, dwHandle = 0;
	BYTE *pData = NULL;
	UINT uLen;

	char exePath[255];
	memset(exePath, 0x00, sizeof(exePath));
	GetModuleFileName(Tools::GetSelfModuleHandle(), exePath, sizeof(exePath));
	dwSize = GetFileVersionInfoSize(exePath, &dwTemp);
	if (dwSize > 0)
	{
		pData = new BYTE[dwSize];
		if (pData)
		{
			if (GetFileVersionInfo(exePath, dwHandle, dwSize, pData))
			{
				if (VerQueryValue(pData, ("\\"), (void **)&pVerInfo, &uLen))
				{
					DWORD verMS = pVerInfo->dwFileVersionMS;
					DWORD verLS = pVerInfo->dwFileVersionLS;
					int ver[4];
					ver[0] = HIWORD(verMS);
					ver[1] = LOWORD(verMS);
					ver[2] = HIWORD(verLS);
					ver[3] = LOWORD(verLS);
					sprintf(p, "%d,%d,%d,%d", ver[0], ver[1], ver[2], ver[3]);

					//                     version.Format(_T("%d,%d,%d,%d"), ver[0], ver[1], ver[2], ver[3]);
				}
			}
			delete pData;
		}
	}
}

static size_t http_response(void *contents, size_t size, size_t nmemb, void *userp)
{
	int isize = 0;
	char strSize[4];
	size_t realsize = size * nmemb;
	char **response_ptr = (char **)userp;

	if (*response_ptr == NULL)
	{
		printf("*response_ptr NULL\n");
		*response_ptr = (char*)calloc(1, realsize + 1 + 4);
		isize = 0;
		memcpy(*response_ptr, &isize, 4);
	}
	else
	{
		memset(strSize, 0, sizeof(strSize));
		memcpy(strSize, *response_ptr, 4);
		isize = *(int*)strSize;
		printf("size==%d\n", isize);
		*response_ptr = (char *)realloc(*response_ptr, isize + realsize + 1 + 4);
	}

	if (*response_ptr == NULL)
	{
		free(*response_ptr);
		printf("not enough memory (realloc returned NULL)\n");
		return 0;
	}

	printf("indata==%s\n", contents);
	memcpy((*response_ptr) + isize + 4, contents, realsize);
	isize += realsize;
	memcpy(*response_ptr, &isize, 4);
	(*response_ptr)[isize + 4] = 0;

	return realsize;
}

int Tools::HttpPostXml(char *URL, char *pRequest, char **ppResponse, int iTimeout)
{
	LOG_INFO("发送[%s]报文[%s]", URL, pRequest);
	CURLcode res;
	CURL* curlHandle;
	int iCode;
	iCode = 1;
	curlHandle = curl_easy_init();
	if (curlHandle)
	{
		curl_easy_setopt(curlHandle, CURLOPT_HEADER, 0L);
		curl_easy_setopt(curlHandle, CURLOPT_URL, URL);
		curl_easy_setopt(curlHandle, CURLOPT_HTTPPOST, 1);
		curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, 0);
		curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, 0);
		curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, pRequest);
		curl_easy_setopt(curlHandle, CURLOPT_FORBID_REUSE, 1);
		curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, iTimeout);
		curl_easy_setopt(curlHandle, CURLOPT_NOSIGNAL, 1L);
		curl_easy_setopt(curlHandle, CURLOPT_CONNECTTIMEOUT, iTimeout);
		curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, http_response);
		curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, ppResponse);


		struct curl_slist *pHeaders = NULL;
		pHeaders = curl_slist_append(pHeaders, "Content-Type:application/xml");
		curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, pHeaders);

		res = curl_easy_perform(curlHandle);
		if (res == CURLE_OK && *ppResponse != NULL)
		{
			iCode = 0;
		}
		else
		{
			iCode = -1;
			LOG_ERROR("CURL返回失败[%s]", curl_easy_strerror(res));
		}
		curl_slist_free_all(pHeaders);
		curl_easy_cleanup(curlHandle);
	}
	return iCode;
}