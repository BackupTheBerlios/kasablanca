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

int main(int, char *)
{
	ftplib ftp;
	string input;

	ftp.SetCallbackLogFunction(CallbackLog);
	ftp.SetCallbackBytes(1024);

	while (input != "quit")
	{
		std::getline(std::cin, input);

		if (input == "connect")
		{
			string server;

			//cin >> server;
			std::getline(std::cin, server);
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

			std::getline(std::cin, user);
			std::getline(std::cin, pass);
			//cin >> user;
			//cin >> pass;
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

			//std::getline(std::cin, dir);
			std::getline(std::cin, dir);
			//dir.erase(0,1);
			cout << "kb.issue.chdir" << endl;
			if (ftp.Chdir(dir.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "rmdir")
		{
			string dir;

			std::getline(std::cin, dir);
			cout << "kb.issue.rmdir" << endl;
			if (ftp.Rmdir(dir.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "mkdir")
		{
			string dir;

			std::getline(std::cin, dir);
			cout << "kb.issue.mkdir" << endl;
			if (ftp.Mkdir(dir.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "delete")
		{
			string file;

			std::getline(std::cin, file);
			cout << "kb.issue.delete" << endl;
			if (ftp.Delete(file.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "rename")
		{
			string oldfile, newfile;

			std::getline(std::cin, oldfile);
			std::getline(std::cin, newfile);
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
			cout << "kb.issue.raw" << endl;
			if (ftp.Raw(cmd.c_str())) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
		}
		else if (input == "dir")
		{
			string file;

			std::getline(std::cin, file);
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

			std::getline(std::cin, flag);
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
		else if (input == "setpasv")
		{
			ftp.SetConnmode(ftplib::pasv);
		}
		else if (input == "setport")
		{
			ftp.SetConnmode(ftplib::port);
		}
		else if (input == "get")
		{
			string file, path;

			std::getline(std::cin, file);
			std::getline(std::cin, path);
			//cin >> file;
			//cin >> path;
			cout << "kb.issue.get" << endl;
			ftp.SetCallbackXferFunction(CallbackXfer);
			if (ftp.Get(file.c_str(), path.c_str(), ftplib::image)) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
			cout << "kb.xfer.done" << endl;
			ftp.SetCallbackXferFunction(NULL);
		}
		else if (input == "getresume")
		{
			string file, path, offset;

			//cin >> file;
			///cin >> path;
			//cin >> offset;

			std::getline(std::cin, file);
			std::getline(std::cin, path);
			std::getline(std::cin, offset);

			cout << "kb.issue.getresume" << endl;
			ftp.SetCallbackXferFunction(CallbackXfer);
			if (ftp.Get(file.c_str(), path.c_str(), ftplib::image, atoi(offset.c_str()))) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
			cout << "kb.xfer.done" << endl;
			ftp.SetCallbackXferFunction(NULL);
		}
		else if (input == "put")
		{
			string file, path;

			std::getline(std::cin, file);
			std::getline(std::cin, path);

			cout << "kb.issue.put" << endl;
			ftp.SetCallbackXferFunction(CallbackXfer);;
			if (ftp.Put(file.c_str(), path.c_str(), ftplib::image)) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
			cout << "kb.xfer.done" << endl;
			ftp.SetCallbackXferFunction(NULL);
		}
		else if (input == "putresume")
		{
			string file, path, offset;

			std::getline(std::cin, file);
			std::getline(std::cin, path);
			std::getline(std::cin, offset);
			
			cout << "kb.issue.putresume" << endl;
			ftp.SetCallbackXferFunction(CallbackXfer);
			if (ftp.Put(file.c_str(), path.c_str(), ftplib::image, atoi(offset.c_str()))) cout << "kb.success" << endl;
			else cout << "kb.failure" << endl;
			cout << "kb.xfer.done" << endl;
			ftp.SetCallbackXferFunction(NULL);
		}
		else if (input == "fxpinitpasv")
		{	
			unsigned int v[6];
			
			cout << "kb.issue.fxp.init" << endl;

			if (ftp.FxpInitPasv(v))
			{	
				cout << "kb.fxpinit.pasv." 
					<< v[0] << "."
					<< v[1] << "."
					<< v[2] << "."
					<< v[3] << "."
					<< v[4] << "."
					<< v[5] << endl;
				cout << "kb.success" << endl;
			}
			else cout << "kb.failure.fxp.init" << endl;
		}
		else if (input == "fxpinitport")
		{	
			unsigned int v[6];
			string ipport;
			
			cout << "kb.issue.fxp.init" << endl;
			
			std::getline(std::cin, ipport);
			sscanf(ipport.c_str(),"%u.%u.%u.%u.%u.%u",&v[0],&v[1],&v[2],&v[3],&v[4],&v[5]);
			if (ftp.FxpInitPort(v))
			{
				cout << "kb.fxpinit.port" << endl;
				cout << "kb.success" << endl;
			}
			else cout << "kb.failure.fxp.init" << endl;
		}
		else if (input == "fxpget")
		{
			string file;
						
			std::getline(std::cin, file);
			cout << "kb.issue.fxp.get" << endl;
			
			if (ftp.FxpGet(file.c_str())) 
			{
				cout << "kb.fxpget" << endl;
				cout << "kb.success" << endl;
			}
			else cout << "kb.failure.fxp.get" << endl;
		}
		else if (input == "fxpput")
		{
			string file;
						
			std::getline(std::cin, file);
			cout << "kb.issue.fxp.put" << endl;
			
			if (ftp.FxpPut(file.c_str()))
			{
				cout << "kb.fxpput" << endl;
				cout << "kb.success" << endl;
			}
			else cout << "kb.failure.fxp.put" << endl;
		}
		else if (input == "abor")
		{
			cout << "kb.issue.abor" << endl;
		
			if (ftp.Abor())
			{
				cout << "kb.abor" << endl;
				cout << "kb.success" << endl;
			}
			else cout << "kb.failure.fatal" << endl;	
		}
		else if (input == "fxpxferfinished")
		{
			cout << "kb.issue.fxp.finished" << endl;
		
			if (ftp.FxpXferFinished())
			{
				cout << "kb.fxpfinished" << endl;
				cout << "kb.success" << endl;
			}
			else cout << "kb.failure.fxp.finished" << endl;
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

void CallbackLog(char* str, void *)
{
	cout << str;
}

int CallbackDir(int xfered, void *)
{
	cout << "kb.dir." << xfered << endl;
	return 1;
}

int CallbackXfer(int xfered, void *)
{
	cout << "kb.xfer." << xfered << endl;
	return 1;
}
