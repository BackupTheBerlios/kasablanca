#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include <cstdlib>
#include <string>
#include "ftplib.h"

using namespace std;

void CallbackLog(char* str, void* arg);
int CallbackDir(int xfered, void *arg);
int CallbackXfer(int xfered, void *arg);

int main(int argc, char *argv[])
{
	ftplib ftp;
	string input;

	ftp.SetCallbackLogFunction(CallbackLog);
	ftp.SetCallbackBytes(1024);

	while (input != "quit")
	{
		cin >> input;

		if (input == "connect")
		{
			string server;

			cin >> server;
			cout << "kb.issue.connect" << endl;
			if (ftp.Connect(server.c_str())) cout << "kb.success" << endl;
			else
			{
				cout << "connect failed" << endl;
				cout << "kb.failure.fatal" << endl;
				return EXIT_SUCCESS;
			}
		}
		else if (input == "login")
		{
			string user, pass;

			cin >> user;
			cin >> pass;
			cout << "kb.issue.login" << endl;
			if (ftp.Login(user.c_str(), pass.c_str())) cout << "kb.success" << endl;
			else
			{
				cout << "kb.failure.fatal" << endl;
				return EXIT_SUCCESS;
			}
		}
		else if (input == "negotiateencryption")
		{
			cout << "kb.issue.negotiateencryption" << endl;
			if (ftp.NegotiateEncryption()) cout << "kb.success" << endl;
			else
			{
				cout << "kb.failure.fatal" << endl;
				return EXIT_SUCCESS;
			}
		}
		else if (input == "pwd")
		{
			char pwd[256];

			cout << "kb.issue.pwd" << endl;
			if (ftp.Pwd(pwd, 256))
			{
				cout << "kb.pwd." << pwd << endl;
				cout << "kb.success" << endl;
			}
			else
			{
				cout << "kb.failure.fatal" << endl;
				return EXIT_SUCCESS;
			}
		}
		else if (input == "chdir")
		{
			string dir;

			std::getline(std::cin, dir);
			dir.erase(0,1);
			cout << "kb.issue.chdir" << endl;
			if (ftp.Chdir(dir.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "rmdir")
		{
			string dir;

			std::getline(std::cin, dir);
			dir.erase(0,1);
			cout << "kb.issue.dir" << endl;
			if (ftp.Rmdir(dir.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "mkdir")
		{
			string dir;

			cin >> dir;
			cout << "kb.issue.mkdir" << endl;
			if (ftp.Mkdir(dir.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "delete")
		{
			string file;

			cin >> file;
			cout << "kb.issue.delete" << endl;
			if (ftp.Delete(file.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "rename")
		{
			string oldfile, newfile;

			cin >> oldfile;
			cin >> newfile;
			cout << "kb.issue.rename" << endl;
			if (ftp.Rename(oldfile.c_str(), newfile.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "cdup")
		{
			cout << "kb.issue.cdup" << endl;
			if (ftp.Cdup()) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "raw")
		{
			string cmd;

			std::getline(std::cin, cmd);
			cmd.erase(0,1);
			cout << "kb.issue.raw" << endl;
			if (ftp.Raw(cmd.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "dir")
		{
			string file;

			cin >> file;
			cout << "kb.issue.dir" << endl;
			ftp.SetCallbackXferFunction(CallbackDir);
			if (ftp.Dir(file.c_str(), ""))
			{
				cout << "kb.dir.done." << file << endl;
				cout << "kb.success" << endl;
			}
			else
			{
				cout << "kb.failure.fatal" << endl;
				return EXIT_SUCCESS;
			}
			ftp.SetCallbackXferFunction(NULL);
		}
		else if (input == "setdataencryption")
		{
			string flag;

			cin >> flag;
			cout << "kb.issue.setdataencryption" << endl;
			if (flag == "on")
			{
				if (ftp.SetDataEncryption(ftplib::secure)) cout << "kb.success" << endl;
				else cout << "kb.failure" << endl;
			}
			else if (flag == "off")
			{
				if (ftp.SetDataEncryption(ftplib::unencrypted)) cout << "kb.success" << endl;
				else cout << "kb.failure" << endl;
			}
			else cout << "kb.failure" << endl;
		}
		else if (input == "setconnmode")
		{
			string flag;

			cin >> flag;
			cout << "kb.issue.setconnmode" << endl;
			if (flag == "pasv")
			{
				ftp.SetConnmode(ftplib::pasv);
				cout << "kb.success" << endl;
			}
			else if (flag == "port")
			{
				ftp.SetConnmode(ftplib::port);
				cout << "kb.success" << endl;
			}
			else cout << "kb.failure" << endl;
		}
		else if (input == "get")
		{
			string file, path;

			cin >> file;
			cin >> path;
			cout << "kb.issue.get" << endl;
			ftp.SetCallbackXferFunction(CallbackXfer);
			if (ftp.Get(file.c_str(), path.c_str(), ftplib::image)) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
			cout << "kb.xfer.done" << endl;
			ftp.SetCallbackXferFunction(NULL);
		}
		else if (input == "getresume")
		{
			string file, path;
			unsigned int offset;

			cin >> file;
			cin >> path;
			cin >> offset;
			cout << "kb.issue.getresume" << endl;
			ftp.SetCallbackXferFunction(CallbackXfer);
			if (ftp.Get(file.c_str(), path.c_str(), ftplib::image, offset)) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
			cout << "kb.xfer.done" << endl;
			ftp.SetCallbackXferFunction(NULL);
		}
		else if (input == "put")
		{
			string file, path;

			cin >> file;
			cin >> path;
			cout << "kb.issue.put" << endl;
			ftp.SetCallbackXferFunction(CallbackXfer);;
			if (ftp.Put(file.c_str(), path.c_str(), ftplib::image)) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
			cout << "kb.xfer.done" << endl;
			ftp.SetCallbackXferFunction(NULL);
		}
		else if (input == "putresume")
		{
			string file, path;
			unsigned int offset;

			cin >> file;
			cin >> path;
			cin >> offset;
			cout << "kb.issue.putresume" << endl;
			ftp.SetCallbackXferFunction(CallbackXfer);
			if (ftp.Put(file.c_str(), path.c_str(), ftplib::image, offset)) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
			cout << "kb.xfer.done" << endl;
			ftp.SetCallbackXferFunction(NULL);
		}
		else if (input == "quit")
		{
			cout << "kb.issue.quit" << endl;
			if (ftp.Quit()) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
	}

	cout << "kb.quit" << endl;
	return EXIT_SUCCESS;
}

void CallbackLog(char* str, void* arg)
{
	cout << str;
}

int CallbackDir(int xfered, void *arg)
{
	cout << "kb.dir." << xfered << endl;
	return 1;
}

int CallbackXfer(int xfered, void *arg)
{
	cout << "kb.xfer." << xfered << endl;
	return 1;
}
