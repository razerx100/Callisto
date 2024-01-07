#ifndef EXCEPTION_HPP_
#define EXCEPTION_HPP_
#include <exception>
#include <string>

class Exception : public std::exception {
public:
	inline Exception(std::string_view errorType, std::string_view errorMessage) noexcept
		: m_exceptionType{ std::move(errorType) }, m_errorMessage{ std::move(errorMessage) } {}

	[[nodiscard]]
	inline const char* GetType() const noexcept { return std::data(m_exceptionType); }
	[[nodiscard]]
	inline const char* what() const noexcept override { return std::data(m_errorMessage); }

private:
	std::string_view m_exceptionType;
	std::string_view m_errorMessage;
};
#endif
