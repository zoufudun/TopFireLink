#ifndef DOMODULECONFIGWIDGET_H
#define DOMODULECONFIGWIDGET_H

#include "domodule.h"
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QWidget>


class DOModuleConfigWidget : public QWidget {
  Q_OBJECT

public:
  explicit DOModuleConfigWidget(DOModule *module, QWidget *parent = nullptr);
  ~DOModuleConfigWidget();

private slots:
  void onChannelCountChanged(int index);
  void onBitVariableChanged(int row, int column);
  void saveConfiguration();
  void onChannelSelectionChanged(int index);

private:
  void setupUI();
  void updateBitTable(int channelIndex);

  DOModule *m_module;
  QComboBox *m_channelCountCombo;
  QComboBox *m_channelSelectCombo; // 通道选择下拉框
  QTableWidget *m_bitTable;        // 使用单个表格替代多个选项卡
  QPushButton *m_saveButton;
  int m_currentChannelIndex; // 当前选中的通道索引
};

#endif // DOMODULECONFIGWIDGET_H
