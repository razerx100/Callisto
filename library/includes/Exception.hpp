#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_
#include <exception>
#include <string>

class Exception : public std::exception
{
public:
	Exception(std::string_view errorType, std::string_view errorMessage)
		: m_exceptionType{ std::move(errorType) }, m_errorMessage{ std::move(errorMessage) }
	{}

	[[nodiscard]]
	const char* GetType() const noexcept { return std::data(m_exceptionType); }
	[[nodiscard]]
	const char* what() const noexcept override { return std::data(m_errorMessage); }

private:
	std::string_view m_exceptionType;
	std::string_view m_errorMessage;
};
#endif
