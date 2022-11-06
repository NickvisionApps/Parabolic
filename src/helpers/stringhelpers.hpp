#pragma once

#include <memory>
#include <stdexcept>
#include <string>

namespace NickvisionTubeConverter::Helpers::StringHelpers
{
	/**
     * Formats a string similarly to sprintf in C
     *
     * @param format The source string
     * @param args The arguments to be added
     * @returns The formatted string
     */
	template <typename... Args>
    std::string format(const std::string& format, Args... args)
    {
		int size_s{ std::snprintf(nullptr, 0, format.c_str(), args...) + 1 }; // Extra space for '\0'
		if (size_s <= 0)
		{
		    throw std::runtime_error("Error during formatting.");
		}
		size_t size{ static_cast<size_t>(size_s) };
		std::unique_ptr<char[]> buf{ new char[size] };
		std::snprintf(buf.get(), size, format.c_str(), args...);
		return { buf.get(), buf.get() + size - 1 }; // We don't want the '\0' inside
    }
}