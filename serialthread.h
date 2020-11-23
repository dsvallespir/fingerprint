#ifndef SERIALTHREAD_H
#define SERIALTHREAD_H

#include <QThread>

class SerialThread : public QThread
{
    Q_OBJECT

public:
    explicit SerialThread(QObject *parent = 0);
    void run();
    static void setFingerPrintCommand(unsigned int);
    static unsigned int getFingerPrintCommnad(void);
    static unsigned int prevCmd;


signals:
    void signalOne(const QByteArray&);
    void signalTwo();
    void loadingSerialBuffer(int val);



public slots:

private:

    unsigned int loadingVal;

};

#endif // SERIALTHREAD_H
