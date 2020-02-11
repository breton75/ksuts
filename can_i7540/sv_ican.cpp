#include "sv_ican.h"

ican::SvICan::SvICan()
{
 
  
}







/** ***** SvCAN_Writer ***** **/
ican::SvCAN_Writer::SvCAN_Writer()
{
  
}

ican::SvCAN_Writer::~SvCAN_Writer()
{
  deleteLater();
}

int ican::SvCAN_Writer::init(ican::SvDeviceTypes dev_type, QString dev_name, CAN_Queue* out)
{
  
}

int ican::SvCAN_Writer::writeData(quint32 id, QByteArray data)
{
  try {
    
    switch (_type) {
      
      case ican::sdtI7540: {
       
        if(!_i7540) _exception.raise("No device i7540");
          
        _i7540->write(data);
        
        break;
        
      }
        
      default:
        break;
    }
  }
  
  catch(SvException& e) {
    
  }
}


/** ***** SvCAN_Reader ***** **/
ican::SvCAN_Reader::SvCAN_Reader()
{
  
}

ican::SvCAN_Reader::~SvCAN_Reader()
{
  deleteLater();
}

int ican::SvCAN_Reader::init(ican::SvDeviceTypes dev_type, QString dev_name, CAN_Queue* out)
{
  
}

