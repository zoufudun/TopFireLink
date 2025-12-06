#ifndef HOSTMODULECONFIGWIDGET_H
#define HOSTMODULECONFIGWIDGET_H

#include "hostmodule.h"
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QTextEdit>
#include <QWidget>


class HostModuleConfigWidget : public QWidget {
  Q_OBJECT

public:
  explicit HostModuleConfigWidget(HostModule *module,
                                  QWidget *parent = nullptr);
  ~HostModuleConfigWidget();

private slots:
  void onProtocolChanged(int index);
  void onDhcpToggled(bool enabled);
  void onTestConnection();
  void saveConfiguration();
  void validateInput();

private:
  void setupUI();
  void loadConfiguration();
  void updateNetworkFields(bool enabled);

  HostModule *m_module;

  // 基本信息
  QLineEdit *m_hostNameEdit;
  QTextEdit *m_descriptionEdit;

  // 网络配置
  QCheckBox *m_dhcpCheckBox;
  QLineEdit *m_ipAddressEdit;
  QLineEdit *m_subnetMaskEdit;
  QLineEdit *m_gatewayEdit;

  // 通信配置
  QComboBox *m_protocolCombo;
  QSpinBox *m_portSpinBox;

  // 按钮
  QPushButton *m_testButton;
  QPushButton *m_saveButton;

  // 状态标签
  QLabel *m_statusLabel;
};

#endif // HOSTMODULECONFIGWIDGET_H
