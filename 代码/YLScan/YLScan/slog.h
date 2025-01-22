#ifndef SLOG_H
#define SLOG_H

#include "string"
/// 包裹类
/// @note 在程序启动的时候调用Logger::Start方法，例如：
///       Log.init("slog.properties");
///       打日志的示例如下：
///       Log.Debug("Debug log[%d]", 100);
///      [可以自定义不同格式]
///
///

#if defined(__linux__)
#define VSPRINTF vsnprintf
#elif defined(_WIN32)
#define VSPRINTF _vsnprintf
#endif

#define DO_LOGGERSIMPLE(logLevel, pcSrcFile, iLine, pData)\
    log4cplus::Logger _logger1 = log4cplus::Logger::getInstance("global");\
	\
    if(_logger1.isEnabledFor(logLevel))\
	{                \
		char szFileLocation[1024];\
		memset(szFileLocation, 0x00, sizeof(szFileLocation));\
		sprintf(szFileLocation, "[%s:%d]", vFormatSourceFileName_slog(pcSrcFile), iLine);\
		_logger1.forcedLog(logLevel, pData, vFormatSourceFileName_slog(pcSrcFile), iLine); \
	}


#define DO_LOGGER(logLevel, pcSrcFile, iLine, pFormat, bufSize)\
    log4cplus::Logger _logger = log4cplus::Logger::getInstance("global");\
    \
    if(_logger.isEnabledFor(logLevel))\
	{                \
		va_list args;            \
		va_start(args, pFormat);        \
		char buf[bufSize] = {0};        \
		_vsnprintf(buf, sizeof(buf), pFormat, args);    \
		va_end(args);           \
		char szFileLocation[512];\
		memset(szFileLocation, 0x00, sizeof(szFileLocation));\
		sprintf(szFileLocation, "[%s:%d]", vFormatSourceFileName_slog(pcSrcFile), iLine);\
		_logger.forcedLog(logLevel, buf, vFormatSourceFileName_slog(pcSrcFile), iLine); \
	}

class Slog
{
public:
	Slog();
	virtual ~Slog();

	/// 启动日志系统
	/// @param[in] properties_filename 日志系统配置文件文件名
	/// log种类和属性都在配置文件中设置
	void init(const char* properties_filename);

public:
	void Debug(const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void Error(const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void Fatal(const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void Info(const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void Warn(const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void Trace(const char* pcSrcFile, const int iLine, const char* pFormat, ...);

	void DebugSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void ErrorSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void FatalSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void InfoSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void WarnSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...);
	void TraceSimpleBytes(const char *pcHexMsg, const int iHexSize, const char* pcSrcFile, const int iLine, const char* pFormat, ...);

public:
	static inline Slog* getSingletonPtr()
	{
		return &getSingleton();
	}
	static inline Slog& getSingleton()
	{
		static Slog _instance;
		return _instance;
	}
};
#define Log Slog::getSingleton()
#define Plog Slog::getSingleton()
#define DATALEN		1024 * 10

const char * vFormatSourceFileName_slog(const char* pcFileName);
std::string vFormatHexData_slog(unsigned char *uszData, int iLen);

//
// 断言日志
//
#define ASSERT_LOG(expr)\
    if ( (expr) ) {;} else Log.Error(__FILE__, __LINE__, #expr);


//
// 以下的宏只有VS2005以及之上的版本可以使用！因为VS2005之下的版本不支持可变参数宏
//
#if defined(_MSC_VER) && _MSC_VER > 1400
#define LOG_DEBUG(...)    Log.Debug(__FILE__, __LINE__, __VA_ARGS__);
#define LOG_ERROR(...)    Log.Error(__FILE__, __LINE__, __VA_ARGS__);
#define LOG_FATAL(...)    Log.Fatal(__FILE__, __LINE__, __VA_ARGS__);
#define LOG_INFO(...)     Log.Info(__FILE__, __LINE__, __VA_ARGS__);
#define LOG_WARN(...)     Log.Warn(__FILE__, __LINE__, __VA_ARGS__);
#define LOG_TRACE(...)    Log.Trace(__FILE__, __LINE__, __VA_ARGS__);

#define LOG_DEBUGSB(pcHexMsg, iHexSize, ...)    Log.DebugSimpleBytes(pcHexMsg, iHexSize, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_ERRORSB(pcHexMsg, iHexSize, ...)    Log.ErrorSimpleBytes(pcHexMsg, iHexSize, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_FATALSB(pcHexMsg, iHexSize, ...)    Log.FatalSimpleBytes(pcHexMsg, iHexSize, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_INFOSB(pcHexMsg, iHexSize, ...)     Log.InfoSimpleBytes(pcHexMsg, iHexSize, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_WARNSB(pcHexMsg, iHexSize, ...)     Log.WarnSimpleBytes(pcHexMsg, iHexSize, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_TRACESB(pcHexMsg, iHexSize, ...)    Log.TraceSimpleBytes(pcHexMsg, iHexSize, __FILE__, __LINE__, __VA_ARGS__);
#elif defined(__linux__)
#define LOG_DEBUG(...)    g_Logger.Debug(__VA_ARGS__);
#define LOG_ERROR(...)    g_Logger.Error(__VA_ARGS__);
#define LOG_FATAL(...)    g_Logger.Fatal(__VA_ARGS__);
#define LOG_INFO(...)     g_Logger.Info(__VA_ARGS__);
#define LOG_WARN(...)     g_Logger.Warn(__VA_ARGS__);
#define LOG_TRACE(...)    g_Logger.Trace(__VA_ARGS__);
#endif

#endif // SLOG_H