#ifndef LOOPMODULECONFIGWIDGET_H
#define LOOPMODULECONFIGWIDGET_H

#include "loopmodule.h"
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QWidget>


class LoopModuleConfigWidget : public QWidget {
  Q_OBJECT

public:
  explicit LoopModuleConfigWidget(LoopModule *module,
                                  QWidget *parent = nullptr);
  ~LoopModuleConfigWidget();

  void save(); // Public save method

private slots:
  void onChannelCountChanged(int index);
  void onChannelSelectionChanged(int index);
  void onAddDevice();
  void onRemoveDevice();
  void onSave(); // Internal slot for save button
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

  // Temporary storage for edits before saving
  QMap<int, QList<LoopDevice>> m_tempDevices;
};

#endif // LOOPMODULECONFIGWIDGET_H
