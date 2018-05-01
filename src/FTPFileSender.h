#ifndef __FTPFILESENDER_H__
#define __FTPFILESENDER_H__

#include "application.h"
#include "compressFile.h"
#include "SdFat.h"
#include "ParticleFtpClient.h"

using namespace particleftpclient;

typedef enum filesenderstate{
  FILESENDER_IDLE,
  FILESENDER_COMPRESSING,
  FILESENDER_SENDING,
  FILESENDER_GETTING,
  FILESENDER_DONE,
  FILESENDER_ERROR,
}fileSenderState;

typedef enum filesendererror{
  FILESENDER_NOERROR,
  FILESENDER_FILENOTFOUND,
  FILESENDER_COMPRESSERROR,
  FILESENDER_FTPNOTFOUND,
  FILESENDER_FTPSENDERROR,
  FILESENDER_FTPGETERROR,
  FILESENDER_FELLOUTOFSTATEMACHINE,
  FILESENDER_BUSY,
}fileSenderError;


typedef enum ftpsendstate{
  FTPIDLE,
  FTPCONNECT,
  FTPSENDUSER,
  FTPSENDPASSWD,
  FTPMKDIRS,
  FTPCHDIRS,
  FTPSTARTGETDATA,
  FTPGETDATA,
  FTPGETFINISH,
  FTPGETCHECKSIZE,
  FTPGETQUIT,
  FTPSTARTSENDDATA,
  FTPSENDDATA,
  FTPSENDFINISH,
  FTPSENDQUIT,
  FTPDONE,
  FTPERROR,
} ftpSendState;


class FileSender{
private:
  fileSenderState state;
  fileSenderError error;
  File outFile;
  CompressFile compress;
  ParticleFtpClient ftp;
  String ftpPasswd;
  String ftpServer;
  String ftpUser;
  uint16_t ftpPort;
  String fileToSend;
  String fileToSave;
  ftpSendState ftpState;
  ftpSendState ftpError;
  int _getSize;
  void ftpSendTask(void);
  bool ftpStartSend(void);
  void ftpThrowError(void);
  bool changeOrMakeDir(String);
  String errorString(void);
  String statusString(void);
protected:
public:
  void task(void);
  void begin(void);
  String toString(void);
  fileSenderError sendFile(String);
  fileSenderError getFile(String,String);
  fileSenderState getStatus(void);
  fileSenderError getError(void);
  ftpSendState getFTPState(void);
  ftpSendState getFTPError(void);
  compressFileState getCompressState(void);
  void clearError(void);
  FileSender(String,String,String,uint16_t);
};



#endif
