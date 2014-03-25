#include <SnoopCapture>
#include <VDebugNew>

// ----------------------------------------------------------------------------
// SnoopCapture
// ----------------------------------------------------------------------------
SnoopCapture::SnoopCapture(void* owner) : VObject(owner)
{
  autoRead = true;
  autoParse = true;
  packet.clear();
}

SnoopCapture::~SnoopCapture()
{
}

bool SnoopCapture::doOpen()
{
  if (autoRead)
  {
    // ----- by gilgil 2009.08.31 -----
    //
    // There can be a case that even if thread starts,
    // state remains not VState::Opened(still VState::Opening) state.
    // So set m_state into stOpened before open thread.
    //
    this->m_state = VState::Opened;
    runThread().open();
    // --------------------------------
  }
  return true;
}

bool SnoopCapture::doClose()
{
  runThread().close();
  return true;
}

int SnoopCapture::read(SnoopPacket* packet)
{
  Q_UNUSED(packet)
  SET_ERROR(SnoopError, "read not supported", VERR_NOT_READABLE);
  return -1;
}

int SnoopCapture::write(SnoopPacket* packet)
{
  Q_UNUSED(packet)
  SET_ERROR(SnoopError, "write not supported", VERR_NOT_WRITABLE);
  return -1;
}

int SnoopCapture::write(u_char* buf, int size, WINDIVERT_ADDRESS* divertAddr)
{
  Q_UNUSED(buf)
  Q_UNUSED(size)
  Q_UNUSED(divertAddr)
  SET_ERROR(SnoopError, "write not supported", VERR_NOT_WRITABLE);
  return -1;
}

#include <SnoopEth>
#include <SnoopIp>
#include <SnoopArp>
#include <SnoopTcp>
#include <SnoopUdp>
#include <SnoopIcmp>
#include <SnoopTcpData>
#include <SnoopUdpData>
void SnoopCapture::parse(SnoopPacket* packet)
{
  //
  // eth
  //
  if (SnoopEth::parse(packet))
  {
    //
    // ip
    //
    if (SnoopIp::parse(packet))
    {
      //
      // tcp
      //
      if (SnoopTcp::parse(packet))
      {
        //
        // tcp data
        //
        SnoopTcpData::parse(packet);
      } else
      //
      // udp
      //
      if (SnoopUdp::parse(packet))
      {
        //
        // udp data
        //
        SnoopUdpData::parse(packet);
      } else
      //
      // icmp
      //
      SnoopIcmp::parse(packet);
    } else
    //
    // arp
    //
    SnoopArp::parse(packet);
  }
}

bool SnoopCapture::relay(SnoopPacket* packet)
{
  Q_UNUSED(packet)
  LOG_FATAL("virtual function call");
  return false;
}

void SnoopCapture::run()
{
  emit opened();
  while (runThread().active())
  {
    int res = read(&packet);
    if (res == 0) continue;
    if (res < 0) break;
    emit captured(&packet);
    if (!packet.drop && packet.sender != NULL && packet.sender->captureType() == SnoopCaptureType::InPath)
      packet.sender->relay(&packet);
  }
  emit closed();
}

void SnoopCapture::load(VXml xml)
{
  VObject::load(xml);

  autoRead  = xml.getBool("autoRead",  autoRead);
  autoParse = xml.getBool("autoParse", autoParse);
}

void SnoopCapture::save(VXml xml)
{
  VObject::save(xml);

  xml.setBool("autoRead",  autoRead);
  xml.setBool("autoParse", autoParse);
}

#ifdef QT_GUI_LIB
void SnoopCapture::optionAddWidget(QLayout* layout)
{
  VOptionable::addCheckBox(layout, "chkAutoRead", "Auto Read", autoRead);
}

void SnoopCapture::optionSaveDlg(QDialog* dialog)
{
  autoRead = dialog->findChild<QCheckBox*>("chkAutoRead")->checkState() == Qt::Checked;
}
#endif // QT_GUI_LIB
