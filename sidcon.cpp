//
// A dirty sidtune format converter hack with almost no security code.
// Copyright (C) Michael Schwendt.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation; either version 2 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//

#if defined(HAVE_FREEBSD)
#include <sys/types.h>
#endif
#include <ctype.h>
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream.h>
#include <iomanip.h>

#include <sidplay/sidtune.h>
#include <sidplay/fformat.h>

static bool toPSID = true,
    toSIDPLAY = false,
    checkOnly = false,
    processDir = false,
    recursive = false,
    assumeAll = false;

static bool isDir(const char* file);
static void recurseDir(DIR* directory, const char* path);
static void handleFile(const char* file);
static void printUsage();

int main(int argc, char* argv[])
{
    DIR *directory;
    if ((directory = opendir(".")) == 0)
    {
        cerr << "ERROR: Cannot read current directory." << endl;
        exit(-1);
    }
    
    if (argc < 2)
    {
        printUsage();
        exit(-1);
    }
    
    int a = 0;
    
    while (a++ < argc-1)
    {
        if (myStrNcaseCmp(argv[a],"--psid") == 0)
        {
            toPSID = true;
            toSIDPLAY = false;
        }
        else if (myStrNcaseCmp(argv[a],"--sidplay") == 0)
        {
            toPSID = false;
            toSIDPLAY = true;
        }
        else if (myStrNcaseCmp(argv[a],"--") == 0)
        {
            printUsage();
            exit(-1);
        }
        else if (myStrNcaseCmp(argv[a],"-a") == 0)
        {
            assumeAll = true;
        }
        else if (myStrNcaseCmp(argv[a],"-c") == 0)
        {
            checkOnly = true;
        }
        else if (myStrNcaseCmp(argv[a],"-d") == 0)
        {
            processDir = true;
        }
        else if (myStrNcaseCmp(argv[a],"-r") == 0)
        {
            recursive = true;
        }
        else if (myStrNcaseCmp(argv[a],"-") == 0)
        {
            printUsage();
            exit(-1);
        }
    }

    if (recursive)
    {
		recurseDir(directory,".");
        exit(0);
    }
	
    if (processDir)
    {
        dirent *entry;
        while ((entry = readdir(directory)) != 0)
        {
            if ((strcmp(entry->d_name,".")!=0) &&
                (strcmp(entry->d_name,"..")!=0) &&
				!isDir(entry->d_name))
            {
                handleFile(entry->d_name);
            }
        }
        closedir(directory);
        exit(0);
    }

    // Check for any filenames given.
    bool success = true;
    a = 0;
    while (a++ < argc-1)
    {
        success = true;  // reset flag for each new file
        
        if (myStrNcaseCmp(argv[a],"-") != 0)
        {
            handleFile(argv[a]);
        }
    }
}

static void printUsage()
{
    cerr
        << "Usage: sidcon <options> [--psid|--sidplay] <files>" << endl
        << "options: -a    assume yes (y) on all questions" << endl
        << "         -c    only check each file's loadaddress" << endl
        << "         -d    process current directory" << endl
        << "         -r    recurse into current directory tree" << endl;
}

static void handleFile(const char* file)
{
    cout << file << ' ';
            
    char* outFileName = 0;
    sidTune mySidTune(file,true,0);  // true => compile and run on MS Windows
    sidTuneInfo mySTI;
            
    bool success = mySidTune.getStatus();
    bool saveIt = !checkOnly;

	if (!success)
	{
		mySidTune.getInfo(mySTI);
		cerr << mySTI.statusString << endl;
	}

	char* origDataName = 0;
	char* infoFileName = 0;

    if (success)
    {
        cout << '.';
        success &= mySidTune.getInfo(mySTI);
        if (mySTI.fixLoad)
        {
            cout << endl << "Duplicate C64 load address detected. Fix it? (y/n) " << flush;
            bool fixIt = false;
            if (assumeAll)
            {
                cout << endl;
                fixIt = true;
            }
            else
            {
                char c;
                cin >> c;
                fixIt = (tolower(c)=='y');
            }
            if (fixIt)
            {
                mySidTune.fixLoadAddress();
                saveIt = true;
            }
        }
        success &= (mySTI.dataFileName!=0);
        if (success)
        {
            success &= (0!=(outFileName = new char[strlen(mySTI.path)+strlen(mySTI.dataFileName)+4+1]));
            if (success)
            {
                strcpy(outFileName,mySTI.path);
                strcat(outFileName,mySTI.dataFileName);
				origDataName = myStrDup(outFileName);  // save it for error recovery
            }
        }
    }

 	char* tmp = fileNameWithoutPath(tmpnam(NULL));
    char* tmpDataName = new char[strlen(mySTI.path)+strlen(tmp)+1];
    strcpy(tmpDataName,mySTI.path);
    strcat(tmpDataName,tmp);
     
 	tmp = fileNameWithoutPath(tmpnam(NULL));
    char* tmpInfoName = new char[strlen(mySTI.path)+strlen(tmp)+1];
    strcpy(tmpInfoName,mySTI.path);
    strcat(tmpInfoName,tmp);
	
    if (success && saveIt)
    {
        cout << '.';
		success &= (0==rename(outFileName,tmpDataName));  // outFileName = path+dataFileName
        if (mySTI.infoFileName != 0)
        {
            strcpy(outFileName,mySTI.path);
            strcat(outFileName,mySTI.infoFileName);
			infoFileName = myStrDup(outFileName);  // save it for error recovery
			success &= (0==rename(outFileName,tmpInfoName));
        }
		else
		{
			tmpInfoName = 0;
		}
    }

	
    if (success && saveIt)
    {
        cout << '.';
        if (toPSID)
        {
            strcpy(fileExtOfPath(outFileName),".sid");
            success &= (mySidTune.savePSIDfile(outFileName));
        }
        else if (toSIDPLAY)
        {
            strcpy(fileExtOfPath(outFileName),".c64");
            success &= (mySidTune.saveC64dataFile(outFileName));
            strcpy(fileExtOfPath(outFileName),".sid");
            success &= (mySidTune.saveSIDfile(outFileName));
        }
		
		if (success)
		{
			remove(tmpDataName);
			if (tmpInfoName != 0)
				remove(tmpInfoName);
		}
		else  // recover tmp files
		{
			rename(tmpDataName,origDataName);
			if (tmpInfoName != 0)
				rename(tmpInfoName,infoFileName);
		}
    }

    if (success && saveIt)
    {
        cout << " SAVED" << endl;
    }
    else if (success)
    {
        cout << " OK" << endl;
    }

    if (tmpDataName != 0)
        delete[] tmpDataName;
    if (tmpInfoName != 0)
        delete[] tmpInfoName;
    if (outFileName != 0)
        delete[] outFileName;
	if (origDataName != 0)
		delete[] origDataName;
	if (infoFileName != 0)
		delete[] infoFileName;
}

bool isDir(const char* file)
{
	DIR *directory;
	if ((directory = opendir(file)) == 0)
		return false;
	else
	{
		closedir(directory);
		return true;
	}
}

void recurseDir(DIR* directory, const char* path)
{
	dirent *entry;
	while ((entry = readdir(directory)) != 0)
	{
		char* file = new char[strlen(path)+strlen(entry->d_name)+1+1];
		strcpy(file,path);
		strcat(file,"/");
		strcat(file,entry->d_name);
		if ((strcmp(entry->d_name,".")!=0) &&
			(strcmp(entry->d_name,"..")!=0) &&
			!isDir(file))
		{
			handleFile(file);
		}
		else if ((strcmp(entry->d_name,".")!=0) &&
				 (strcmp(entry->d_name,"..")!=0) &&
				 isDir(file))
		{
			char* newPath = new char[strlen(path)+strlen(entry->d_name)+1+1];
			strcpy(newPath,path);
			strcat(newPath,"/");
			strcat(newPath,entry->d_name);
			DIR* nextDir;
			if ((nextDir = opendir(newPath)) == 0)
			{
				cerr << "ERROR: Cannot read directory ``" << newPath << "''." << endl;
			}
			else
			{
				recurseDir(nextDir,newPath);
			}
			delete[] newPath;
		}
		delete[] file;
	}
	closedir(directory);
}
