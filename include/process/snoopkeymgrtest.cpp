#include <SnoopKeyMgrTest>

REGISTER_METACLASS(SnoopKeyMgrTest, SnoopProcess)

// ----------------------------------------------------------------------------
// SnoopKeyMgrTest
// ----------------------------------------------------------------------------
SnoopKeyMgrTest::SnoopKeyMgrTest(void* owner) : SnoopProcess(owner)
{
  keyMgr  = NULL;
  user    = 0;
  memSize = 0;
}

SnoopKeyMgrTest::~SnoopKeyMgrTest()
{
  close();
}

bool SnoopKeyMgrTest::doOpen()
{
  if (keyMgr == NULL)
  {
    SET_ERROR(SnoopError, "keyMgr is null", VERR_OBJECT_IS_NULL);
    return false;
  }
  keyMgr->registerAccessible(this, keyMgr->mac_items, user, memSize);

  return SnoopProcess::doOpen();
}

bool SnoopKeyMgrTest::doClose()
{
  return SnoopProcess::doClose();
}

void SnoopKeyMgrTest::onNew_Mac(SnoopMacKey* key, int user, void* mem)
{
  LOG_DEBUG("user=%d mem=%p", user, mem);
}

void SnoopKeyMgrTest::onDel_Mac(SnoopMacKey* key, int user, void* mem)
{
  LOG_DEBUG("user=%d mem=%p", user, mem);
}

void SnoopKeyMgrTest::test(SnoopPacket* packet)
{
  LOG_DEBUG("");
}

void SnoopKeyMgrTest::load(VXml xml)
{
  SnoopProcess::load(xml);

  QString keyMgrName = xml.getStr("keyMgr", "");
  if (keyMgrName != "") keyMgr = (SnoopKeyMgr*)(((VGraph*)owner)->objectList.findByName(keyMgrName));
  user = xml.getInt("user", user);
  memSize = (int)xml.getInt("memSize", (size_t)memSize);
}

void SnoopKeyMgrTest::save(VXml xml)
{
  SnoopProcess::save(xml);

  QString keyMgrName = keyMgr == NULL ? "" : keyMgr->name;
  xml.setStr("keyMgr", keyMgrName);
  xml.setInt("user", user);
  xml.setInt("memSize", (size_t)memSize);
}

#ifdef QT_GUI_LIB
void SnoopKeyMgrTest::addOptionWidget(QLayout* layout)
{
  SnoopProcess::addOptionWidget(layout);

  QStringList keyMgrList = ((VGraph*)owner)->objectList.findNamesByClassName("SnoopKeyMgr");
  QComboBox* cbxKeyMgr = VOptionable::addComboBox(layout, "cbxKeyMgr", "KeyMgr", keyMgrList, -1);
  cbxKeyMgr->setCurrentText(keyMgr == NULL ? "" : keyMgr->name);
  VOptionable::addLineEdit(layout, "leUser", "User", QString::number(user));
  VOptionable::addLineEdit(layout, "leMemSize", "Mem Size", QString::number(memSize));
}

void SnoopKeyMgrTest::saveOptionDlg(QDialog* dialog)
{
  SnoopProcess::saveOptionDlg(dialog);

  keyMgr = (SnoopKeyMgr*)(((VGraph*)owner)->objectList.findByName(dialog->findChild<QComboBox*>("cbxKeyMgr")->currentText()));
  user    = dialog->findChild<QLineEdit*>("leUser")->text().toInt();
  memSize = (size_t)dialog->findChild<QLineEdit*>("leMemSize")->text().toInt();
}
#endif // QT_GUI_LIB