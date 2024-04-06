#pragma once
#include "defs.h"


class Driver {
	HANDLE handle = 0;

	uintptr_t module_base = 0;

	unsigned long module_size = AC_MODULE_BASE_SIZE;

	int process_id = 0;

public:

	Driver() {
		void* tmp_handle = CreateFile(L"\\\\.\\acube", GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);

		if ((long long)tmp_handle == 0xFFFFFFFFFFFFFFFF) { // INVALID_HANDLE_VALUE
			MessageBox(NULL, L"Failed to open driver connection", L"Error", MB_ICONERROR);
			exit(0);
		}

		this->handle = tmp_handle;
	}

	~Driver() {
		CloseHandle(handle);
	}

	int get_process_id(UNICODE_STRING process_name) {

		KPROCESSID_REQUEST request = { 0 };
		request.process_name = process_name;

		DWORD bytes_returned;
		DeviceIoControl(handle, PROCESS_ID_REQUEST, &request, sizeof(request), NULL, NULL, &bytes_returned, NULL);
		this->process_id = (int)bytes_returned;
		
		spdlog::info("PID: {}", process_id);

		if (!process_id) {
			MessageBox(NULL, L"Failed to get process ID, check if the process is running", L"Error", MB_ICONERROR);
			exit(0);
		}


		return process_id;
	}


	uintptr_t get_module_base(UNICODE_STRING module_name) {

		KERNEL_MODULE_REQUEST request = { 0 };
		request.pid = this->process_id;
		request.module_name = module_name;

		DWORD bytes_returned;
		DeviceIoControl(handle, MODULE_BASE_REQUEST, &request, sizeof(request), NULL, NULL, &bytes_returned, NULL);
		uintptr_t base = (uintptr_t)bytes_returned;
		this->module_base = base;
        spdlog::info("MODULE_BASE: {}", base);
		return base;
	}

	template <typename T>
	T readv(uintptr_t address) {
		T buffer{ 0 };
		_KERNEL_READ_REQUEST request = { 0 };
		request.src_pid = process_id;
		request.src_address = (void*)address;
		request.p_buffer = &buffer;
		request.size = sizeof(T);

		DeviceIoControl(handle, READ_REQUEST, &request, sizeof(request), &request, sizeof(request), NULL, NULL);


		return buffer;

	}

	std::string read_str(uintptr_t address) {
		static const int length = 64;
		std::vector<char> buffer(length);

		this->read_raw((void*)address, buffer.data(), length);

		const auto& it = find(buffer.begin(), buffer.end(), '\0');

		if (it != buffer.end())
			buffer.resize(distance(buffer.begin(), it));

		return std::string(buffer.begin(), buffer.end());
	}


	bool read_raw(void* address, void* target, size_t size) {
		_KERNEL_READ_REQUEST request = { 0 };
		request.src_pid = process_id;
		request.src_address = (void*)address;
		request.p_buffer = target;
		request.size = size;

		if (DeviceIoControl(handle, READ_REQUEST, &request, sizeof(request), &request, sizeof(request), NULL, NULL))
			return true;

		return false;
	}

	std::optional<uint32_t> find_pattern(const std::string_view& pattern, unsigned int byte_offset = 0) {



		const auto module_base = this->module_base;

		if (!module_base || !module_size)
			return {};

		const auto module_data = std::make_unique<uint8_t[]>(module_size);

		if (!this->read_raw((void*)module_base, module_data.get(), module_size))
			return {};

		const auto pattern_bytes = this->aob(pattern);

		for (auto i{ 0 }; i < module_size - pattern.size(); ++i) {
			auto found{ true };

			for (auto j{ 0 }; j < pattern_bytes.size(); ++j) {
				if (module_data[i + j] != pattern_bytes[j] && pattern_bytes[j] != -1) {
					found = false;
					break;
				}
			}

			if (found) {
                   auto pattern = module_base + i + byte_offset;
                   spdlog::info("PATTERN_FOUND: at {}", pattern);
                   return pattern;
			}
		}

		return {};
	}

	uint32_t find_pattern_offset(const std::string_view& pattern, unsigned int byte_offset = 0) {

		const auto signature = this->find_pattern(pattern, byte_offset);

		return this->readv<uint32_t>(signature.value()) - this->module_base;

	}

private:
	std::vector<int32_t> aob(const std::string_view& pattern) {
		std::vector<int32_t> bytes;

		for (auto i{ 0 }; i < pattern.size(); ++i) {
			switch (pattern[i]) {
			case '?':
				bytes.push_back(-1);
				break;

			case ' ':
				break;

			default: {
				if (i + 1 < pattern.size()) {
					auto value{ 0 };

					if (const auto [ptr, ec] = std::from_chars(pattern.data() + i, pattern.data() + i + 2, value, 16); ec == std::errc()) {
						bytes.push_back(value);
						++i;
					}
				}

				break;
			}
			}
		}

		return bytes;
	}
};

inline Driver driver;