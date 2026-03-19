#pragma once

#include <string>
#include <format>
#include <tuple>
#include <chrono>
#include <iostream>
#include <filesystem>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

using std::string;


class Log {
public:
	static string GenerateTimestamp();
	static void Info(string message);
	static string ToString(glm::vec3 vector);
	static string ToString(glm::quat quaternion);
	static string ToString(glm::mat4 matrix, bool pretty);

private:
	static std::string m_lastClassPrinted;
	static std::string m_lastMessagePrinted;
};