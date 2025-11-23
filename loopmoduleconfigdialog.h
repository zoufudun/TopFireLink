#ifndef LOOPMODULECONFIGDIALOG_H
#define LOOPMODULECONFIGDIALOG_H

#include "loopmodule.h"
#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>

class LoopModuleConfigDialog : public QDialog {
  Q_OBJECT

public:
  explicit LoopModuleConfigDialog(LoopModule *module,
                                  QWidget *parent = nullptr);
  ~LoopModuleConfigDialog();

private slots:
  void onChannelCountChanged(int index);
  void onChannelSelectionChanged(int index);
  void onAddDevice();
  void onRemoveDevice();
  void onSave();
  void onLoopModeChanged(int index);

private:
  void setupUI();
  void loadData();
  void updateDeviceTable(int channelIndex);
  void saveCurrentChannelData();

  LoopModule *m_module;
  int m_currentChannelIndex;

  // UI Elements
  QTabWidget *m_tabWidget;
  QComboBox *m_channelCountCombo;
  QComboBox *m_channelSelectCombo;
  QComboBox *m_loopModeCombo;
  QCheckBox *m_initCheckBox;
  QCheckBox *m_mapCheckBox;

  QTableWidget *m_deviceTable;
  QPushButton *m_addDeviceBtn;
  QPushButton *m_removeDeviceBtn;
  QPushButton *m_saveBtn;
  QPushButton *m_cancelBtn;

  // Temporary storage for edits before saving
  QMap<int, QList<LoopDevice>> m_tempDevices;
};

#endif // LOOPMODULECONFIGDIALOG_H
