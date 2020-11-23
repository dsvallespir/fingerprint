#include "serialthread.h"
#include "fingerprint.h"
#include "mainwindow.h"
#include <QtCore>

#include <QMessageBox>

unsigned int SerialThread::prevCmd;

SerialThread::SerialThread(QObject *parent):
    QThread(parent)
{

}

SysParamStructure SysParamStruct;

void SerialThread::run(){
    // Do Something....

    static stateType_t *state = new stateType_t;
    unsigned int checksum;
    unsigned int len;
    //unsigned int buffRecvCount = 0;
    bool endStateMachine = false;

    QByteArray *buffRecv = new QByteArray;
    //buffRecv->append(serial.readAll());
    qDebug() << "Tamaño del buffer de recepción: " << QString::number(MainWindow::serial.readBufferSize());

    MainWindow::serial.setReadBufferSize(0);

    qDebug() << "Tamaño del buffer seteado al máximo posible. ";

    //buffRecv->append(MainWindow::serial.readAll());

    // MainWindow::serial.flush();

    buffRecv->append(MainWindow::serial.read(40000));
    qDebug() << "Verificando si hubo errrores al leer 4000 bytes: " << MainWindow::serial.errorString();

    //MainWindow::serial.isReadable()


    qDebug() << "Datos leídos: " << buffRecv->count() << "bytes";

    //buffRecvCount = buffRecv->count();

    QByteArray *imgBuff = new QByteArray;


    if ( buffRecv->count() > 256 ) {
        qDebug() << "Hola! soy el thread, voy a procesar los bytes recibidos (si es que hay)...";

        *state = STATE_IDLE;

        do {
            switch(*state){
            case STATE_IDLE:{
                        // Si se entra acá es porqué llegaron datos nuevos
                        qDebug() << "State Idle";
                        if (buffRecv->count() > 12){
                            if (buffRecv->startsWith(QByteArray::fromHex("ef01ffffffff"))) {
                                // Es una trama
                                buffRecv->remove(0,6);
                                // Se remueve la cabecera
                            }
                            //trama procesada
                            *state = STATE_PACKET_ID;
                            checksum = 0;
                        }
                        else{ continue; }
                        break;
             }
             case STATE_PACKET_ID:{
                switch(buffRecv->at(0)){
                            case PID_CMD_PACKET: {
                                                    checksum += PID_CMD_PACKET;
                                                    buffRecv->remove(0,1);
                                                    *state = STATE_CMD_PACKET;
                                                    break;
                                                 }
                            case PID_DATA_PACKET: {
                                                    checksum += PID_DATA_PACKET;
                                                    buffRecv->remove(0,1);
                                                    *state = STATE_DATA_PACKET;
                                                    break;
                                                 }
                            case PID_ACK_PACKET: {
                                                    checksum += PID_ACK_PACKET;
                                                    buffRecv->remove(0,1);
                                                    *state = STATE_ACK_PACKET;
                                                    break;
                                                 }
                            case PID_END_PACKET: {
                                                    checksum += PID_END_PACKET;
                                                    buffRecv->remove(0,1);
                                                    *state = STATE_END_PACKET;
                                                    qDebug() << "State Packet Id Ok";
                                                    break;
                                                 }
                            default: {
                                        *state = STATE_ERROR;
                                        qDebug() << "State Error 1";
                                        break;
                                     }
                                }
                 break;
             }

             case STATE_CMD_PACKET: {
                              break;
              }
             case STATE_DATA_PACKET: {
                                    len = (unsigned int)(buffRecv->at(0) << 8 | buffRecv->at(1));
                                    buffRecv->remove(0, 2);
                                    for(unsigned int i = 0; i < len; i ++){
                                        imgBuff->append(buffRecv->at(i) & 0xF0);
                                        imgBuff->append((buffRecv->at(i) & 0x0F) << 4);
                                    }
                                    imgBuff->remove((len*2)-4,4);

                                    buffRecv->remove(0, len);
                                    qDebug() << "ImgBuff: " << imgBuff->count() << " bytes.";

                                   //loadingVal = imgBuff->count() / buffRecvCount;
                                    emit(loadingSerialBuffer(imgBuff->count()));
                                    *state = STATE_IDLE;
                                    break;
              }
             case STATE_ACK_PACKET: {
                                    // Si es un ACK, entonces un comando previo ejecutado
                                    len = (unsigned int)(buffRecv->at(0) << 8 | buffRecv->at(1));
                                    buffRecv->remove(0, 2);
                                    *state = STATE_CONFIRMATION_CODE;
                                    break;
             }
             case STATE_END_PACKET:{
                                    endStateMachine = true;
                                    qDebug() << "Fin de procesamiento de paquete de datos";
                                    break;

             }
             case STATE_CONFIRMATION_CODE:{
                                        char confirmationCode = buffRecv->at(0);
                                        switch(confirmationCode){
                                            case CONF_CODE_CMD_EXEC_COMPL:{ 			// 00h: commad execution complete;
                                                    buffRecv->remove(0,1);
                                                    *state = STATE_PROCESS_DATA;
                                                    break;
                                            }
                                            case CONF_CODE_ERR_RECV_DATA_PCKG: break; 		// 01h: error when receiving data package;
                                            case CONF_CODE_NO_FINGER: break;				// 02h: no finger on the sensor;
                                            case CONF_CODE_FAIL_TO_ENROLL: break;			// 03h: fail to enroll the finger;
                                            // 06h: fail to generate character file due to the over-disorderly fingerprint image;
                                            // 07h: fail to generate  character file due to lackness of character point or
                                            //	   	over-smallness  of fingerprint image
                                            // 08h: finger doesn’t match;
                                            // 09h: fail to find the matching finger;
                                            // 0Ah: fail to combine the character files;
                                            // 0Bh: addressing PageID is beyond the finger library;
                                            // 0Ch: error when reading template from library or the template is invalid;
                                            // 0Dh: error when uploading template;
                                            // 0Eh: Module can’t receive the following data packages.
                                            // 0Fh: error when uploading image;
                                            // 10h: fail to delete the template;
                                            // 11h: fail to clear finger library;
                                            // 13h: wrong password!
                                            // 15h: fail to generate the image for the lackness of valid primary image;
                                            // 18h: error when writing flash;
                                            // 19h: No definition error;
                                            // 1Ah: invalid register number;
                                            // 1Bh: incorrect configuration of register;
                                            // 1Ch: wrong notepad page number;
                                            // 1Dh: fail to operate the communication port;
                                            // others: system reserved;
                                            default: 	{
                                                            *state = STATE_ERROR;
                                                            break;
                                                        }
                                        }
                                        break;
               }
             case STATE_PROCESS_DATA:		// procesar datos o checksum según corresponda
                                {
                                        switch(prevCmd)
                                        {
                                            case VFY_PWD:	{ break;}
                                            case SET_PWD:	{ break;}
                                            case SET_ADDR:	{ break;}
                                            case SYS_PARAM:	{ break;}
                                            case PORT_CTRL:	{ break;}
                                            case GET_SYS_PARAM:{
                                                                            SysParamStruct.statReg[0] = buffRecv->at(0);
                                                                            SysParamStruct.statReg[1] = buffRecv->at(1);
                                                                            SysParamStruct.sysIdCode[0] = buffRecv->at(2);
                                                                            SysParamStruct.sysIdCode[1] = buffRecv->at(3);
                                                                            SysParamStruct.fingerLibSize[0] = buffRecv->at(4);
                                                                            SysParamStruct.fingerLibSize[1] = buffRecv->at(5);
                                                                            SysParamStruct.securityLevel[0] = buffRecv->at(6);
                                                                            SysParamStruct.securityLevel[1] = buffRecv->at(7);
                                                                            SysParamStruct.deviceAddress[0] = buffRecv->at(8);
                                                                            SysParamStruct.deviceAddress[1] = buffRecv->at(9);
                                                                            SysParamStruct.deviceAddress[2] = buffRecv->at(10);
                                                                            SysParamStruct.deviceAddress[3] = buffRecv->at(11);
                                                                            SysParamStruct.dataSize[0] = buffRecv->at(12);
                                                                            SysParamStruct.dataSize[1] = buffRecv->at(13);
                                                                            SysParamStruct.baudSettings[0] = buffRecv->at(14);
                                                                            SysParamStruct.baudSettings[1] = buffRecv->at(15);


                                                                            break;
                                                                        }
                                            case READ_TMPL_NUM:{ break;}
                                            case GEN_IMG:	{
                                                                unsigned int sum;
                                                                sum = (unsigned int)(buffRecv->at(0) << 8 | buffRecv->at(1));
                                                                buffRecv->remove(0,2);
                                                                if(checksum == sum){
                                                                     qDebug() << "Checksum Ok";
                                                                }
                                                                else {
                                                                    qDebug() << "Checksum Error";
                                                                }
                                                                *state = STATE_IDLE;
                                                                endStateMachine = true;
                                                                break;
                                        }
                                            case UP_IMG:	{
                                                                unsigned int sum;
                                                                sum = (unsigned int)(buffRecv->at(0) << 8 | buffRecv->at(1));
                                                                buffRecv->remove(0,2);
                                                                if(checksum == sum){
                                                                    qDebug() << "STATE_PROCESS_DATA: Checksum Ok";
                                                                }
                                                                else {
                                                                    qDebug() << "STATE_PROCESS_DATA: Checksum Error";
                                                                }
                                                                *state = STATE_IDLE;
                                                                break;
                                            }
                                            case DOWN_IMG:	{ break;}
                                            case GEN_CHAR_FILE_FROM_IMG:	{ break;}
                                            case UP_CHAR:	{ break;}
                                            case DOWN_CHAR:	{ break;}
                                            case STORE_TMPL:	{ break;}
                                            case READ_TMPL_FROM_FLASH:	{ break;}
                                            case DEL_TMPL_FROM_FLASH:	{ break;}
                                            case EMPTY_LIBRARY:	{ break;}
                                            case MATCH_TEMPL:	{ break;}
                                            case SEARCH_TEMPL:	{ break;}
                                            case GEN_RANDOM:	{ break;}
                                            case WRITE_NOTEPAD:	{ break;}
                                            case READ_NOTEPAD:	{ break;}

                                        } break;
                                    }

              case STATE_ERROR: {        //hacer algo
                            //MainWindow::close();
                            break;
                                }
             default:
                    break;

             }

         } while(endStateMachine != true);
            if (prevCmd == UP_IMG){
                emit signalOne(*imgBuff);
            }
            if(prevCmd == SYS_PARAM){

                qDebug() << "Parametros del lector: ";
                qDebug() << SysParamStruct.statReg[0];
                qDebug() <<SysParamStruct.statReg[1] ;
                qDebug() <<SysParamStruct.sysIdCode[0]  ;
                qDebug() << SysParamStruct.sysIdCode[1] ;
                qDebug() << SysParamStruct.fingerLibSize[0] ;
                qDebug() << SysParamStruct.fingerLibSize[1] ;
                qDebug() <<SysParamStruct.securityLevel[0] ;
                qDebug() <<SysParamStruct.securityLevel[1] ;
                qDebug() <<SysParamStruct.deviceAddress[0];
                qDebug() <<SysParamStruct.deviceAddress[1];
                qDebug() << SysParamStruct.deviceAddress[2];
                qDebug() << SysParamStruct.deviceAddress[3];
                qDebug() <<SysParamStruct.dataSize[0];
                qDebug() <<SysParamStruct.dataSize[1];
                qDebug() <<SysParamStruct.baudSettings[0] ;
                qDebug() <<SysParamStruct.baudSettings[1];
            }
         }
         else{
        //QMessageBox::warning(this, "Error en la comunicación","El flujo recibido no es correcto", QMessageBox::Close);
        qDebug() << "Error procesando la trama";
    }

}


void SerialThread::setFingerPrintCommand(unsigned int cmd){
    prevCmd = cmd;
}

unsigned int SerialThread::getFingerPrintCommnad(void){
    return prevCmd;
}


//Read valid template number EF01 xxxx 01H 0003H 1dH 0021H  0xef01ffffffff010100031d0021
