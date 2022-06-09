#include "fs.hpp"

namespace core::fs {
	Result<String, FileOpenError> read_to_string(PathView path) {
		auto result = fs::File::open(path, FileFlags::Read);
		if (!result.is_ok()) return result.unwrap_err();
		fs::File file = result.unwrap();
		const auto size = file.size();

		Array<char> bytes;
		bytes.reserve(size + 1);
		bytes.set_len(size);
		file.read({ (u8*)bytes.ptr(), bytes.len() });
		bytes.push(0);
		return String(core::move(bytes));
	}
}