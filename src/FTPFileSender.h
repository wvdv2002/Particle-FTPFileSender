#ifndef __FILESENDER_H__
#define __FILESENDER_H__

#include "application.h"
#include "compressFile.h"
#include "SdFat.h"
#include "ParticleFtpClient.h"

using namespace particleftpclient;

typedef enum filesenderstate{
  FILESENDER_IDLE,
  FILESENDER_COMPRESSING,
  FILESENDER_SENDING,
  FILESENDER_DONE,
  FILESENDER_ERROR,
}fileSenderState;

typedef enum filesendererror{
  FILESENDER_NOERROR,
  FILESENDER_FILENOTFOUND,
  FILESENDER_COMPRESSERROR,
  FILESENDER_FTPNOTFOUND,
  FILESENDER_FTPSENDERROR,
  FILESENDER_FELLOUTOFSTATEMACHINE,
  FILESENDER_BUSY,
}fileSenderError;


typedef enum ftpsendstate{
  FTPIDLE,
  FTPCONNECT,
  FTPSENDUSER,
  FTPSENDPASSWD,
  FTPMKDIRS,
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
  SdFile outFile;
  CompressFile compress;
  ParticleFtpClient ftp;
  String ftpPasswd;
  String ftpServer;
  String ftpUser;
  uint16_t ftpPort;
  String fileToSend;
  ftpSendState ftpState;
  ftpSendState ftpError;
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
  fileSenderState getStatus(void);
  fileSenderError getError(void);
  ftpSendState getFTPState(void);
  ftpSendState getFTPError(void);
  compressFileState getCompressState(void);
  void clearError(void);
  FileSender(String,String,String,uint16_t);
};



#endif
