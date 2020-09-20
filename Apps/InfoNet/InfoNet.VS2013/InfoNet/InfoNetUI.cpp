
#include "InfoNetUI.h"

using namespace InfoNet;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// check whether command line arguments have been passed
	if (args->Length == 0)
	{
		Application::EnableVisualStyles();
		Application::SetCompatibleTextRenderingDefault(false);

		// launch Viewer
		Application::Run(gcnew InfoNetUI());
	}
	else
	{
		// Determine number of args
		int ArgCount = args->Length;

		// display a helpscreen
		// NOTE: A console window might not be activated
		if (ArgCount > 0 && args[0]->Contains("--help")) {
			Console::WriteLine("MyApp v1.0");
			Console::ReadLine();
		}
		else
		{
			// Transfer command line arguments to function parameters
			if (ArgCount > 0) {
				char* ptrArg1 = (char*)Marshal::StringToHGlobalAnsi(args[0]).ToPointer();
				// using: ptrArg1;
				Marshal::FreeHGlobal(IntPtr(ptrArg1));
			}
		}
	}
}
