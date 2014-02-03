#pragma once

inline void AssertOkay(int result)
{
	using namespace Microsoft::VisualStudio::CppUnitTestFramework;
	Assert::AreEqual(int(CEC_OKAY), result);
}

template <typename _MsgTy>
inline bool wait_msg(_MsgTy& msg, long timeout_ms = 0)
{
	msg_type_t msg_type;
	msg_reader reader;

	const long poll_interval = 100;
	int poll_count = static_cast<int>(timeout_ms / poll_interval);
	for (int poll = 0; poll < poll_count; ++poll) {
		auto peek_result = cl_peek_msg_from_test_session(&msg_type, &reader);
		if (peek_result == CEC_OKAY) break;
		else if (peek_result != CEC_EMPTY) return false;

		std::this_thread::sleep_for(std::chrono::milliseconds(poll_interval));
	}

	if (_MsgTy::__type == msg_type)
		reader >> msg;

	if (cl_pop_msg_from_test_session() == CEC_OKAY)
		return true;
	return false;
}