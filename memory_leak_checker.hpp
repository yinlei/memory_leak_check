//
// memory_leack_checker.hpp
//
//  Aid with checking for memory leaks. Create this object and destroy it around the code you
// suspect has a memory leak. When it is destored it will make sure that no newly allocated
// memory blocks are present. Dump will occur to standard output.
//
#pragma once

#include <crtdbg.h>

class memory_leak_checker {
public:

    memory_leak_checker (void);	// Takes a snapshot of memory
    ~memory_leak_checker (void);    // Makes sure it is still the same

    // Cause you to pop into the debugger when a certian block id is allocated
    static void break_on_block (long block_id);

	// Dump output to a particular file...
	void set_file (const char *filename, bool append);

private:
    _CrtMemState *_memory_state;    // Memory snapshot
    int _old_tracking_flags;	    // What were we looking for?

    int _report_mode_warn;	    // Old modes
    int _report_mode_error;
    int _report_mode_assert;

	_HFILE _report_file_warn;	    // Old output destinations
	_HFILE _report_file_error;
	_HFILE _report_file_assert;

	char _filename[200];
	bool _filename_valid;
	void *_filename_handle;

};
