//
// memory_leak_checker.cpp
//
//  This guy's constructor will start leak checking and when
// its destructor is called it will print out error messages
// if there is any memory that has been allocated.
//
#include "memory_leak_checker.hpp"
#include <iostream>

#include <CRTDBG.H>
#include <windows.h>

//
// memory_leak_checker
//
//  Cache our state
//
memory_leak_checker :: memory_leak_checker (void)
#ifdef _DEBUG
: _memory_state (new _CrtMemState), _filename_valid (false)
#else
  : _memory_state (0)
#endif
{
#ifdef _DEBUG
	//
	// Turn on leek checking
	//

	_old_tracking_flags = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG | _CRTDBG_LEAK_CHECK_DF);// Turn on leak-checking bit

	//
	// Remember what the memory stuff looks like
	//

	_report_mode_warn = _CrtSetReportMode( _CRT_WARN, _CRTDBG_MODE_FILE );
	_report_mode_error = _CrtSetReportMode( _CRT_ERROR, _CRTDBG_MODE_FILE );
	_report_mode_assert = _CrtSetReportMode( _CRT_ASSERT, _CRTDBG_MODE_FILE );

	_report_file_warn = _CrtSetReportFile( _CRT_WARN, _CRTDBG_FILE_STDOUT );
	_report_file_error = _CrtSetReportFile( _CRT_ERROR, _CRTDBG_FILE_STDOUT );
	_report_file_assert = _CrtSetReportFile( _CRT_ASSERT, _CRTDBG_FILE_STDOUT );

	_CrtMemCheckpoint(_memory_state);

#endif
}

//
// set_file
//
//  Set the output file
//
void memory_leak_checker::set_file (const char *filename, bool append)
{
	unsigned int create_flags = CREATE_ALWAYS;
	if (append) {
		create_flags = OPEN_ALWAYS;
	}

	_filename_handle = ::CreateFile (filename, GENERIC_WRITE, 0, 0, create_flags, FILE_ATTRIBUTE_NORMAL, 0);
	_filename_valid = _filename_handle != INVALID_HANDLE_VALUE;

	if (_filename_valid) {

		//
		// Get to the end of the file
		//

		::SetFilePointer (_filename_handle, 0, 0, FILE_END);

		//
		// Set up the various report handles
		//

		_CrtSetReportFile ( _CRT_WARN, _filename_handle);
		_CrtSetReportFile ( _CRT_ERROR, _filename_handle);
		_CrtSetReportFile ( _CRT_ASSERT, _filename_handle);
	}
}

//
// ~memory_leak_checker
//
//  Restore everything and print out the current state of
// the memory heap.
//
memory_leak_checker :: ~memory_leak_checker (void)
{
#ifdef _DEBUG
	//
	// See if there is any difference between now and then.
	//

	_CrtMemState state_now;
	_CrtMemCheckpoint(&state_now);

	_CrtMemState the_difference;
	if (_CrtMemDifference(&the_difference, _memory_state, &state_now)) {
		std::cout << "***" << std::endl << "*** Memory Leak Seen!" << std::endl << "***" << std::endl;
		_CrtMemDumpStatistics(&the_difference);
		_CrtMemDumpAllObjectsSince(_memory_state);
	}

	delete _memory_state;
	//
	// Reset the memory tracking flags
	//

	_CrtSetDbgFlag (_old_tracking_flags);

	//
	// Reset the error reporting
	//

	_CrtSetReportMode( _CRT_WARN, _report_mode_warn);
	_CrtSetReportMode( _CRT_ERROR, _report_mode_error);
	_CrtSetReportMode( _CRT_ASSERT, _report_mode_assert);

	_CrtSetReportFile( _CRT_WARN, _report_file_warn);
	_CrtSetReportFile( _CRT_ERROR, _report_file_error);
	_CrtSetReportFile( _CRT_ASSERT, _report_file_assert);

	//
	// CLose a file if we have it open
	//

	if (_filename_valid) {
		::CloseHandle (_filename_handle);
	}

#endif
}

//
// break_on_block
//
//  Cause a debugger break point when a certian block is allocated.
//
void memory_leak_checker::break_on_block (long block_number)
{
#ifdef _DEBUG
	_CrtSetBreakAlloc(block_number);
	_crtBreakAlloc = block_number;

#endif
}