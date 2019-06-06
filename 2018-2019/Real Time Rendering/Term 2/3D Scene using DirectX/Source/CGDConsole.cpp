
//
// CGDConsole.cpp
//

#include <stdafx.h>
#include <CGDConsole.h>
#include <windows.h>
#include <cstdio>


using namespace std;



//
// Private interface implementation
//

// Private constructor
CGDConsole::CGDConsole(const wchar_t* consoleTitle) {

	// Create a new Windows console
	_allocSuccessful = AllocConsole();

	if (_allocSuccessful) {

		// Re-direct stdin, stdout, stderr to the new console
		freopen_s(&stdinFile, "CONIN$", "r", stdin);
		freopen_s(&stdoutFile, "CONOUT$", "w", stdout);
		freopen_s(&stderrFile, "CONOUT$", "w", stderr);

		SetConsoleTitle(consoleTitle);
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	}
	else {

		// Console not created so re-direct stdout and stderr to debug files
		freopen_s(&stdoutFile, "runlog_stdout.txt", "w", stdout);
		freopen_s(&stderrFile, "runlog_stderr.txt", "w", stderr);
	}
}



//
// Public interface implementation
//

// Console factory method
CGDConsole* CGDConsole::CreateConsole(const wchar_t* consoleTitle) {

	static BOOL _consoleSetup = FALSE;

	CGDConsole* console = nullptr;

	if (!_consoleSetup) {

		console = new CGDConsole(consoleTitle);

		if (console)
			_consoleSetup = TRUE;
	}

	return console;
}


CGDConsole::~CGDConsole() {

	// Flush remaining stream buffer content
	::fflush(NULL);

	// Dispose of the console attached to the host process
	if (_allocSuccessful == TRUE) {

		cout << "\nPress any key to continue...";
		_getch();

		BOOL consoleTeardown = FreeConsole();
	}

	// Close file redirections
	if (stdinFile)
		fclose(stdinFile);

	if (stdoutFile)
		fclose(stdoutFile);
	
	if (stderrFile)
		fclose(stderrFile);
}
