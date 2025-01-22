/**********************************************************************
* Copyright (C) 2018 -  - All Rights Reserved
*
* 文件名称:        slog.cpp
* 摘    要:        日志包裹类,日志模块的接口
*
* 作    者:       xuqiang918
* 修    改:       查看文件最下方.
*
***********************************************************************/

#include "slog.h"

#include <stdio.h>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/config.hxx>
#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <log4cplus/helpers/loglog.h>
#include <log4cplus/helpers/stringhelper.h>
#include <log4cplus/helpers/socket.h>
#include <log4cplus/spi/loggerimpl.h>
#include <log4cplus/spi/loggingevent.h>

using namespace log4cplus;
using namespace log4cplus::helpers;


#ifdef _DEBUG
#pragma comment(lib, "log4cplusSD.lib")
#else 
#ifndef _WIN64
#pragma comment(lib, "log4cplusS.lib")
#else
#pragma comment(lib, "x64/log4cplusS.lib")
#endif
#endif // DEBUG


Slog::Slog()
{

}

Slog::~Slog()
{
	log4cplus::Logger _logger = log4cplus::Logger::getInstance("global");
	LOG_INFO("Slog System Stop Finish...");
	_logger.shutdown();
}



void Slog::Debug(const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::DEBUG_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
}

void Slog::Error(const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::ERROR_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
}

void Slog::Fatal(const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::FATAL_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
}

void Slog::Info(const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::INFO_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
}

void Slog::Warn(const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::WARN_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
}

void Slog::Trace(const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::TRACE_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
}

void Slog::DebugSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::DEBUG_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
	DO_LOGGERSIMPLE(log4cplus::DEBUG_LOG_LEVEL, pcSrcFile, iLine, vFormatHexData_slog((unsigned char *)pcHexMsg, iHexSize), iHexSize * 3 + 100);
}

void Slog::ErrorSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::ERROR_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
	DO_LOGGERSIMPLE(log4cplus::ERROR_LOG_LEVEL, pcSrcFile, iLine, vFormatHexData_slog((unsigned char *)pcHexMsg, iHexSize), iHexSize * 3 + 100);
}

void Slog::FatalSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::FATAL_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
	DO_LOGGERSIMPLE(log4cplus::FATAL_LOG_LEVEL, pcSrcFile, iLine, vFormatHexData_slog((unsigned char *)pcHexMsg, iHexSize), iHexSize * 3 + 100);
}

void Slog::InfoSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::INFO_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
	DO_LOGGERSIMPLE(log4cplus::INFO_LOG_LEVEL, pcSrcFile, iLine, vFormatHexData_slog((unsigned char *)pcHexMsg, iHexSize), iHexSize * 3 + 100);
}

void Slog::WarnSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::WARN_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
	DO_LOGGERSIMPLE(log4cplus::WARN_LOG_LEVEL, pcSrcFile, iLine, vFormatHexData_slog((unsigned char *)pcHexMsg, iHexSize), iHexSize * 3 + 100);
}

void Slog::TraceSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...)
{
	DO_LOGGER(log4cplus::TRACE_LOG_LEVEL, pcSrcFile, iLine, pFormat, DATALEN);
	DO_LOGGERSIMPLE(log4cplus::TRACE_LOG_LEVEL, pcSrcFile, iLine, vFormatHexData_slog((unsigned char *)pcHexMsg, iHexSize), iHexSize * 3 + 100);
}

void Slog::init(const char* properties_filename)
{
	if (properties_filename == NULL) return;
	log4cplus::initialize();
	log4cplus::PropertyConfigurator::doConfigure(properties_filename);
	log4cplus::Logger _logger = log4cplus::Logger::getInstance("global");
	LOG_INFO("Logger system start finish.");
}

const char * vFormatSourceFileName_slog(const char* pcFileName)
{
	int i;

	if (pcFileName == NULL)
	{
		return "";
	}
	for (i = strlen(pcFileName) - 1; i > 0; --i)
	{
		if (pcFileName[i] == '\\' || pcFileName[i] == '/')
		{
			return pcFileName + i + 1;
		}
	}
	return pcFileName;
}

const char * vFormatHexData_slog(unsigned char *uszData, int iLen)
{
	char szData[1024 * 10];
	memset(szData, 0x00, sizeof(szData));
	for (int i = 0; i < iLen; i++)
	{
		sprintf(szData, "%s%02X ", szData, uszData[i]);
	}
	return szData;
}