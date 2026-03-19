#include "Log.h"

std::string Log::m_lastClassPrinted = "";
std::string Log::m_lastMessagePrinted = "";

string GetCallerFunctionName();

#ifdef _WIN32
#include <Windows.h>
#include <DbgHelp.h>
string GetCallerFunctionName() {
	HANDLE process = GetCurrentProcess();
	SymInitialize(process, NULL, TRUE);

	// Verify if there is a caller function
	void* stack[3];
	unsigned short frames = CaptureStackBackTrace(0, 3, stack, NULL);
	if (frames < 2) return "UNKNOWN";

	SYMBOL_INFO* symbol = (SYMBOL_INFO*)calloc(sizeof(SYMBOL_INFO) + 256 * sizeof(char), 1);
	symbol->MaxNameLen = 255;
	symbol->SizeOfStruct = sizeof(SYMBOL_INFO);

	SymFromAddr(process, (DWORD64)(stack[2]), 0, symbol);
	string name = symbol->Name;

	free(symbol);
	return name;
}
#endif


std::string ANSI24RGB(int R, int G, int B)
{
	return std::format("\033[38;2;{};{};{}m", R, G, B);
};

string separator = ANSI24RGB(255, 255, 255) + " - ";

string Log::GenerateTimestamp() {
	using namespace std::chrono;

	// get current time
	auto now = system_clock::now();

	// get number of milliseconds for the current second
	// (remainder after division into seconds)
	auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

	// convert to std::time_t in order to convert to std::tm (broken time)
	auto timer = system_clock::to_time_t(now);

	// convert to broken time
#pragma warning(suppress : 4996)
	std::tm bt = *std::localtime(&timer);

	std::ostringstream oss;

	oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
	oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

	return oss.str();
};


void Log::Info(string message) {
	using std::vformat, std::make_format_args, std::clog;

	string timestamp = GenerateTimestamp();

	string fullFunctionName = GetCallerFunctionName();
	string functionName = fullFunctionName;
	string className = "???";
	auto symbolPos = fullFunctionName.find(':');
	if (symbolPos != string::npos) {
		className = fullFunctionName.substr(0, symbolPos);
		functionName = fullFunctionName.substr(symbolPos + 2, -1);
	}

	// Print class name only once
	if (m_lastClassPrinted != className)
	{
		m_lastClassPrinted = className;
		clog << vformat("OpenGLRenderer - " + ANSI24RGB(64, 255, 64) + "{}" + separator + "\n", make_format_args(m_lastClassPrinted));
	};

	m_lastMessagePrinted = vformat(ANSI24RGB(64, 255, 255) + "[{}]" + separator + ANSI24RGB(255, 64, 64) + "DEBUG " + ANSI24RGB(255, 210, 64) + "{}" + separator + "{}\n", make_format_args(timestamp, functionName, message));
	clog << m_lastMessagePrinted;
}

string Log::ToString(glm::vec3 vector) {
	return ("x" + std::to_string(vector.x) + " y" + std::to_string(vector.y) + " z" + std::to_string(vector.z));
}

string Log::ToString(glm::quat quaternion) {
	return ("x" + std::to_string(quaternion.x) + " y" + std::to_string(quaternion.y) + " z" + std::to_string(quaternion.z) + " w" + std::to_string(quaternion.w));
}

string Log::ToString(glm::mat4 matrix, bool pretty) {
	string out = "";
	if (pretty) {
		out += ("x0: " + std::to_string(matrix[0].x) + " y0: " + std::to_string(matrix[0].y) + " z0: " + std::to_string(matrix[0].z) + "\n");
		out += ("x1: " + std::to_string(matrix[1].x) + " y1: " + std::to_string(matrix[1].y) + " z1: " + std::to_string(matrix[1].z) + "\n");
		out += ("x2: " + std::to_string(matrix[2].x) + " y2: " + std::to_string(matrix[2].y) + " z2: " + std::to_string(matrix[2].z) + "\n");
		out += ("x3: " + std::to_string(matrix[3].x) + " y3: " + std::to_string(matrix[3].y) + " z3: " + std::to_string(matrix[3].z) + "\n");
	} else {
		out += ("x0: " + std::to_string(matrix[0].x) + " y0: " + std::to_string(matrix[0].y) + " z0: " + std::to_string(matrix[0].z));
		out += (" x1: " + std::to_string(matrix[1].x) + " y1: " + std::to_string(matrix[1].y) + " z1: " + std::to_string(matrix[1].z));
		out += (" x2: " + std::to_string(matrix[2].x) + " y2: " + std::to_string(matrix[2].y) + " z2: " + std::to_string(matrix[2].z));
		out += (" x3: " + std::to_string(matrix[3].x) + " y3: " + std::to_string(matrix[3].y) + " z3: " + std::to_string(matrix[3].z));
	}
	return out;
}
