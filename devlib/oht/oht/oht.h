#ifndef OHT_H
#define OHT_H

#include "oht_global.h"

#include "../../../global/sv_idevice.h"
#include "../../../global/dev_defs.h"
#include "../../../ksuts_devices/oht/oht_defs.h"
#include "../../../../svlib/sv_exception.h"


#pragma pack(push,1)
struct OHTHeader
{
  quint8  client_addr;
  quint8  func_code;
  quint8  ADDRESS;
  quint8  OFFSET;
  quint16 register_count;
  quint8  byte_count;
};
#pragma pack(pop)

class OHTSHARED_EXPORT OHT: public idev::SvIDevice
{

public:
    OHT();
    ~OHT();

    idev::DeviceTypes type() const { return idev::sdtOHT_Gamma12700; }

    bool readyRead() { return _ready_read; }

    bool open();
    void close();

    void write(const QByteArray* data);
    void read();

  //  bool start(quint32 msecs) { Q_UNUSED(msecs); return true; }

    SerialPortParams* serialParams() { return &_serial_params; }
    bool setParams(const QString& params);

    void setSerialPortParams(const SerialPortParams& params);

  private:
    QSerialPort _serial;

    SerialPortParams _serial_params;
    bool _ready_read = false;

    OHTHeader _header;
    quint8 _data_type;
    quint8 _data_length;
    quint16 _crc;
    quint8 _crc1;
    quint8 _crc2;
    quint8 _data[512];

    SvException* _exception;

    size_t _hSize = sizeof(OHTHeader);

    QTimer _t;
    quint8 _buf[512];
    quint64 _buf_offset = 0;
    quint8 _confirm[8];

  //  quint16 crc16(uchar* buf, int length);
    void analizeData();
    void sendConfirmation();

    void func_0x00();
    void func_0x77();
    void func_0x33();
    void func_0x19();
    void func_0x02();
    void func_0x03();
    void func_0x04();
    void func_0x13();
    void func_0x14();

  private slots:
    void packetTimeout();

};

OHT* OHTSHARED_EXPORT new_instance() { return new OHT(); }
//void delete_instance(OHT* instance);

#endif // OHT_H
