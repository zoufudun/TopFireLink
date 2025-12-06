#ifndef DIMODULECONFIGWIDGET_H
#define DIMODULECONFIGWIDGET_H

#include "dimodule.h"
#include <QComboBox>
#include <QPushButton>
#include <QTabWidget>
#include <QTableWidget>
#include <QWidget>


class DIModuleConfigWidget : public QWidget {
  Q_OBJECT

public:
  explicit DIModuleConfigWidget(DIModule *module, QWidget *parent = nullptr);
  ~DIModuleConfigWidget();

private slots:
  void onChannelCountChanged(int index);
  void onBitVariableChanged(int row, int column);
  void saveConfiguration();
  void onChannelSelectionChanged(int index);

private:
  void setupUI();
  void updateBitTable(int channelIndex);

  DIModule *m_module;
  QComboBox *m_channelCountCombo;
  QComboBox *m_channelSelectCombo; // 通道选择下拉框
  QTableWidget *m_bitTable;        // 使用单个表格替代多个选项卡
  QPushButton *m_saveButton;
  int m_currentChannelIndex; // 当前选中的通道索引
};

#endif // DIMODULECONFIGWIDGET_H
