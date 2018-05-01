#include "FileSender.h"



FileSender::FileSender(String aServer, String anUser, String aPasswd, uint16_t aPort){
  ftp = ParticleFtpClient();
  ftpServer = aServer;
  ftpUser=anUser;
  ftpPasswd = aPasswd;
  ftpPort = aPort;
  state = FILESENDER_IDLE;
  ftpState = FTPIDLE;
  error = FILESENDER_NOERROR;

}

void FileSender::begin(void){

}



void FileSender::task(void){
  switch(state){
    case FILESENDER_IDLE:
    break;
    case FILESENDER_COMPRESSING:
    compress.task();
      if(compress.getError()!=COMPRESS_NOERROR)
      {
        state = FILESENDER_ERROR;
        error = FILESENDER_COMPRESSERROR;
      }
      if(compress.isDone()){
        state = FILESENDER_SENDING;
        if(!ftpStartSend()){
          state=FILESENDER_ERROR;
          error=FILESENDER_FTPSENDERROR;
        }
      }
    break;
    case FILESENDER_SENDING:
    case FILESENDER_GETTING:
      ftpSendTask();
      if (ftpState==FTPDONE){
          state = FILESENDER_DONE;
      }
      else if(ftpState == FTPERROR){
        if(state == FILESENDER_GETTING){
          error = FILESENDER_FTPGETERROR;
        }else{
          error = FILESENDER_FTPSENDERROR;
        }
        state = FILESENDER_ERROR;
      }
    break;
    case FILESENDER_DONE:
    break;
    case FILESENDER_ERROR:
    break;
    default:
      state = FILESENDER_ERROR;
      error = FILESENDER_FELLOUTOFSTATEMACHINE;
    break;
  }
}

void FileSender::ftpThrowError(void){
  if(outFile.isOpen() && (state==FILESENDER_GETTING)){
    outFile.remove();
  }else{
    outFile.close();
  }
  ftpError = ftpState;
  ftpState = FTPERROR;
}

bool FileSender::ftpStartSend(){
  if ((ftpState == FTPIDLE) || (ftpState==FTPDONE) || (ftpState==FTPERROR)){
    ftpState = FTPCONNECT;
    return 1;
  }
  return 0;
}

bool FileSender::changeOrMakeDir(String(aDir)){
  if(!ftp.cwd(aDir)){
    if(!ftp.mkd(aDir)){
      return 0;
    } else if(!ftp.cwd(aDir)){
      return 0;
    }
  }
  return 1;
}

ftpSendState FileSender::getFTPState(void){
  return ftpState;
}

ftpSendState FileSender::getFTPError(void){
  return ftpError;
}

compressFileState FileSender::getCompressState(void){
  return compress.getState();
}

void FileSender::ftpSendTask(void){
  switch(ftpState){
    case FTPIDLE:
      break;
    case FTPCONNECT:
      if(ftp.open(ftpServer,ftpPort,5)){
        ftpState = FTPSENDUSER;
      }else{
        ftpThrowError();
      }
      break;
    case FTPSENDUSER:
      if(ftp.user(ftpUser)){
        ftpState = FTPSENDPASSWD;
      }else{
        ftpThrowError();
      }
      break;
    case FTPSENDPASSWD:
      if(ftp.pass(ftpPasswd)){
        if(state == FILESENDER_SENDING){
          ftpState = FTPMKDIRS;
        }else{
          ftpState = FTPCHDIRS;
        }
      }
      else{
        ftpThrowError();
      }
      break;

    case FTPCHDIRS:
    {
      String temp;
      if(fileToSend.startsWith("/")){
        temp=fileToSend.substring(1);
      } else {
      temp = fileToSend;
      }
      while(temp.indexOf('/')!=-1){
          if(!ftp.cwd(temp.substring(0,temp.indexOf('/')))){
            ftpThrowError();
            break;
          }
          temp=temp.substring(temp.indexOf('/')+1);
      }
      if(!ftp.type("I")){
        ftpThrowError();
      }
      _getSize = ftp.size(temp);
      if(!ftp.retr(temp)){
        ftpThrowError();
      }else{
        ftpState = FTPSTARTGETDATA;
      }
    }
    break;
    case FTPMKDIRS:
      {
        String temp;
        if(fileToSend.startsWith("/")){
          temp=fileToSend.substring(1);
        } else {
        temp = fileToSend;
        }
        while(temp.indexOf('/')!=-1){
            if(!changeOrMakeDir(temp.substring(0,temp.indexOf('/')))){
              ftpThrowError();
              break;
            }
            temp=temp.substring(temp.indexOf('/')+1);
        }
        if(!ftp.type("I")){
          ftpThrowError();
        }
        if(!ftp.stor(temp+".lz")){
          ftpThrowError();
        } else {
          ftpState = FTPSTARTSENDDATA;
        }
      }break;
    case FTPSTARTGETDATA:
      if(!outFile.isOpen()){
        if(!outFile.open(fileToSave,O_WRITE | O_CREAT | O_TRUNC)){
          ftpThrowError();
        }
      }
      if(ftpState != FTPERROR){
        ftpState = FTPGETDATA;
      }
      break;
    case FTPSTARTSENDDATA:
      if(!outFile.isOpen()){
        if(!outFile.open("/temp.lz",O_READ)){
          ftpThrowError();
        }
      }
      ftpState = FTPSENDDATA;
      break;
    case FTPGETDATA:
    {
      byte buf[512];
      int len=512;
      int writeLen;
      if(ftp.data.connected()){
        if(ftp.data.available()){
          len = ftp.data.read(buf,len);
          writeLen = outFile.write(buf,len);
          if (len!=writeLen){
            ftpThrowError();
          }
        }
      }else{
        ftpState = FTPGETCHECKSIZE;
      }
    }
    break;
    case FTPSENDDATA:
      {
        byte buf[512];
        int len;
        len = outFile.read(buf,512);
        if(len>0){
          ftp.data.write(buf,len);
        }
        else{
          ftp.data.flush();
		  outFile.close();
          ftpState = FTPSENDFINISH;
        }
      }
    break;
    case FTPGETFINISH:
      if(!ftp.finish()){
        ftpThrowError();
      }else{
        ftpState = FTPGETCHECKSIZE;
      }
    break;
    case FTPGETCHECKSIZE:
    {
      if(_getSize != outFile.size()){
        ftpThrowError();
      }else{
        outFile.close();
        ftpState = FTPGETQUIT;
      }
    }
    break;
    case FTPSENDFINISH:
      if(!ftp.finish()){
        ftpThrowError();
      }
      else{
        ftpState = FTPSENDQUIT;
      }
      break;
    case FTPGETQUIT:
    case FTPSENDQUIT:
      if(!ftp.quit()){
        ftpThrowError();
      }else{
        ftpState = FTPDONE;
      }
      break;
    case FTPDONE:
      break;
    case FTPERROR:
      ftp.quit();
      ftpState = FTPIDLE;
      break;
    default:
      break;
  }
}

void FileSender::clearError(void){
  state=FILESENDER_IDLE;
  error=FILESENDER_NOERROR;
  ftpState = FTPIDLE;
  ftpError = FTPIDLE;
}

fileSenderError FileSender::getError(void){
  return error;
}

fileSenderState FileSender::getStatus(void){
  return state;
}

fileSenderError FileSender::sendFile(String aFileString){
  if(!(state == FILESENDER_IDLE || state == FILESENDER_ERROR || state == FILESENDER_DONE)){
    return FILESENDER_BUSY;
  }
  clearError();
  if(!outFile.open(aFileString,O_READ)){
    return FILESENDER_FILENOTFOUND;
  }
  outFile.close();
  fileToSend = aFileString;
  compress.startCompress(fileToSend,"temp.lz");
  state=FILESENDER_COMPRESSING;
  error=FILESENDER_NOERROR;
  return error;
}


fileSenderError FileSender::getFile(String aWriteFileString, String aReadFileString){
  if(!(state == FILESENDER_IDLE || state == FILESENDER_ERROR || state == FILESENDER_DONE)){
    return FILESENDER_BUSY;
  }

  fileToSend = aReadFileString;
  fileToSave = aWriteFileString;
  state=FILESENDER_GETTING;
  error=FILESENDER_NOERROR;
  ftpStartSend();
  return error;
}







String FileSender::statusString(void){
  switch(getStatus()){
    case FILESENDER_IDLE:
      return "FS_IDLE";
    break;
    case FILESENDER_COMPRESSING:
      return "FS_COMPR";
    break;
    case FILESENDER_SENDING:
      return "FS_SENDI";
    break;
    case FILESENDER_GETTING:
      return "FS_GETT";
    case FILESENDER_DONE:
      return "FS_DONE";
    break;
    case FILESENDER_ERROR:
      return "FS_ERROR";
    break;
  }
  return "FS_STATENF";
}

String FileSender::errorString(void){
  switch(getError()){
    case FILESENDER_NOERROR:
      return "FS_NOERR";
    break;
    case FILESENDER_FILENOTFOUND:
      return "FS_FILENF";
    break;
    case FILESENDER_COMPRESSERROR:
      return "FS_COMPERR";
    break;
    case FILESENDER_FTPNOTFOUND:
      return "FS_FTPNF";
    break;
    case FILESENDER_FTPSENDERROR:
      return "FS_FTPSENDERR";
    break;
    case FILESENDER_FTPGETERROR:
      return "FS_FTPGETERR";
    break;
    case FILESENDER_FELLOUTOFSTATEMACHINE:
      return "FS_FELLSM";
    break;
    case FILESENDER_BUSY:
      return "FS_BUSY";
    break;
  }
  return "FS_UNKWN";
}

String FileSender::toString(void){
    int aFileSize = 0;
    if(outFile.isOpen()){
      aFileSize = outFile.size();
    }
    return String("FTP: ") + statusString() + "," + errorString() + " " +
    String((int) getStatus()) + "," +
    String((int) getError()) + "," +
    String((int) getFTPState())+ "," +
    String((int) getFTPError()) + "," +
    String((int) getCompressState()) + "," + String(aFileSize);

}
