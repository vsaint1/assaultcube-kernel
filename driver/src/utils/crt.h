#pragma once

#define to_lower_c(Char) ((Char >= (char*)'A' && Char <= (char*)'Z') ? (Char + 32) : Char)

namespace crt
{

	template <typename _T> __forceinline unsigned int constexpr hash_const(_T const* input) { return *input ? static_cast<unsigned int>(*input) + 33 * hash_const(input + 1) : 5381; }

	template <typename t>
	__forceinline int strlen(t str) {
		if (!str)
		{
			return 0;
		}

		t buffer = str;

		while (*buffer)
		{
			*buffer++;
		}

		return (int)(buffer - str);
	}

	__forceinline bool strcmp(const char* src, const char* dst)
	{
		if (!src || !dst)
		{
			return true;
		}

		const auto src_sz = crt::strlen(src);
		const auto dst_sz = crt::strlen(dst);

		if (src_sz != dst_sz)
		{
			return true;
		}

		for (int i = 0; i < src_sz; i++)
		{
			if (src[i] != dst[i])
			{
				return true;
			}
		}

		return false;
	}

	__forceinline int strtoi(const char* str)
	{
		int result = 0;
		while (*str)
		{
			result *= 10;
			result += *str++ - '0';
		}
		return result;
	}

	__forceinline const char* to_lower(char* str) {

		char* str_low = "";
		for (char* pointer = str; *pointer != '\0'; ++pointer)
		{
			*str_low = (char)(short)tolower(*pointer);
		}

		return str_low;
	}

	__forceinline const char* string_to_c_str(UNICODE_STRING* source, char* destination) {

		if (source->Length == 0 || source->Buffer == 0)
			return "";

		for (int i = 0; i < source->Length; i++) {

			destination[i] = source->Buffer[i];
			if (destination[i] == 0) {
				destination[i] = 0;
				break;
			}
		}

		return "";
	};

}
