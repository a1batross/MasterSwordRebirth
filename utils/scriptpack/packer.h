#include "cbase.h"
#include "sharedutil.h"
#include "msfileio.h"

typedef unsigned char byte;

class Packer 
{
public:
  Packer(char *wDir, char *rDir, char *oDir) {
    memcpy(m_WorkDir, wDir, MAX_PATH);
    memcpy(m_RootDir, rDir, MAX_PATH);
    memcpy(m_OutDir, oDir, MAX_PATH);
  }
  void packScripts();
  void cookScripts();
  
private:
  char m_WorkDir[MAX_PATH];
  char m_RootDir[MAX_PATH];
  char m_OutDir[MAX_PATH];
  msstringlist m_StoredFiles;
  
  void readDirectory(char *pszName);
  void storeFile(char *pszCurrentDir, WIN32_FIND_DATA &wfd);
};