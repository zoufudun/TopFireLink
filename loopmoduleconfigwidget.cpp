#include "loopmoduleconfigwidget.h"
#include <QDebug>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>

LoopModuleConfigWidget::LoopModuleConfigWidget(LoopModule *module,
                                               QWidget *parent)
    : QWidget(parent), m_module(module), m_currentChannelIndex(0) {

  // Load initial data into temp storage
  for (int i = 0; i < m_module->getChannelCount(); ++i) {
    m_tempDevices[i] = m_module->getDevices(i);
  }

  setupUI();
  loadData();
}

LoopModuleConfigWidget::~LoopModuleConfigWidget() {}

void LoopModuleConfigWidget::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // Create Tab Widget
  m_tabWidget = new QTabWidget(this);
  mainLayout->addWidget(m_tabWidget);

  // --- Tab 1: Loop Configuration ---
  QWidget *configTab = new QWidget();
  QVBoxLayout *configLayout = new QVBoxLayout(configTab);

  // Top Control Area: Channel Count, Loop Mode, Options
  QGroupBox *settingsGroup = new QGroupBox("基本设置", this);
  QGridLayout *settingsLayout = new QGridLayout(settingsGroup);

  // Channel Count
  QLabel *channelCountLabel = new QLabel("通道数量:", this);
  m_channelCountCombo = new QComboBox(this);
  m_channelCountCombo->addItem("1通道", 1);
  m_channelCountCombo->addItem("2通道", 2);
  m_channelCountCombo->addItem("4通道", 4);
  m_channelCountCombo->addItem("8通道", 8);

  // Loop Mode
  QLabel *loopModeLabel = new QLabel("回路模式:", this);
  m_loopModeCombo = new QComboBox(this);
  m_loopModeCombo->addItem("Class A", (int)LoopMode::ClassA);
  m_loopModeCombo->addItem("Class B", (int)LoopMode::ClassB);
  m_loopModeCombo->addItem("Class A+B", (int)LoopMode::ClassA_Plus_B);

  // Options
  m_initCheckBox = new QCheckBox("初始化", this);
  m_mapCheckBox = new QCheckBox("支持成图", this);

  settingsLayout->addWidget(channelCountLabel, 0, 0);
  settingsLayout->addWidget(m_channelCountCombo, 0, 1);
  settingsLayout->addWidget(loopModeLabel, 0, 2);
  settingsLayout->addWidget(m_loopModeCombo, 0, 3);
  settingsLayout->addWidget(m_initCheckBox, 0, 4);
  settingsLayout->addWidget(m_mapCheckBox, 0, 5);

  configLayout->addWidget(settingsGroup);

  // Channel Selection for Device List
  QHBoxLayout *channelSelectLayout = new QHBoxLayout();
  QLabel *channelSelectLabel = new QLabel("当前配置通道:", this);
  m_channelSelectCombo = new QComboBox(this);
  channelSelectLayout->addWidget(channelSelectLabel);
  channelSelectLayout->addWidget(m_channelSelectCombo);
  channelSelectLayout->addStretch();
  configLayout->addLayout(channelSelectLayout);

  // Device List Table
  m_deviceTable = new QTableWidget(this);
  QStringList headers;
  headers << "类型"
          << "序列号"
          << "地址"
          << "个性代码"
          << "盘号"
          << "卡号"
          << "设备说明"
          << "设备标识"
          << "变量名";
  m_deviceTable->setColumnCount(headers.size());
  m_deviceTable->setHorizontalHeaderLabels(headers);
  m_deviceTable->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Interactive);
  m_deviceTable->horizontalHeader()->setStretchLastSection(true);
  m_deviceTable->horizontalHeader()->setSectionsMovable(true);
  m_deviceTable->setSelectionBehavior(QAbstractItemView::SelectRows);
  m_deviceTable->setAlternatingRowColors(true);

  // Set default column widths
  m_deviceTable->setColumnWidth(0, 150); // Type
  m_deviceTable->setColumnWidth(1, 100); // Serial
  m_deviceTable->setColumnWidth(2, 50);  // Address
  m_deviceTable->setColumnWidth(3, 70);  // Personality
  m_deviceTable->setColumnWidth(4, 50);  // Panel
  m_deviceTable->setColumnWidth(5, 50);  // Card
  m_deviceTable->setColumnWidth(6, 150); // Description
  m_deviceTable->setColumnWidth(7, 100); // Identifier
  m_deviceTable->setColumnWidth(8, 100); // Variable Name

  configLayout->addWidget(m_deviceTable);

  // Device Actions
  QHBoxLayout *actionLayout = new QHBoxLayout();
  m_addDeviceBtn = new QPushButton("增加设备", this);
  m_removeDeviceBtn = new QPushButton("删除设备", this);
  actionLayout->addWidget(m_addDeviceBtn);
  actionLayout->addWidget(m_removeDeviceBtn);
  actionLayout->addStretch();
  configLayout->addLayout(actionLayout);

  m_tabWidget->addTab(configTab, "回路配置");

  // --- Tab 2: Loop Diagnosis ---
  QWidget *diagnosisTab = new QWidget();
  QVBoxLayout *diagLayout = new QVBoxLayout(diagnosisTab);
  diagLayout->addWidget(new QLabel("回路诊断功能开发中...", diagnosisTab));
  m_tabWidget->addTab(diagnosisTab, "回路诊断");

  // --- Tab 3: Loop Mapping ---
  QWidget *mappingTab = new QWidget();
  QVBoxLayout *mapLayout = new QVBoxLayout(mappingTab);
  mapLayout->addWidget(new QLabel("回路成图功能开发中...", mappingTab));
  m_tabWidget->addTab(mappingTab, "回路成图");

  // Save Button
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  m_saveBtn = new QPushButton("应用更改", this);
  buttonLayout->addStretch();
  buttonLayout->addWidget(m_saveBtn);
  mainLayout->addLayout(buttonLayout);

  // Connections
  connect(m_channelCountCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &LoopModuleConfigWidget::onChannelCountChanged);
  connect(m_channelSelectCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &LoopModuleConfigWidget::onChannelSelectionChanged);
  connect(m_addDeviceBtn, &QPushButton::clicked, this,
          &LoopModuleConfigWidget::onAddDevice);
  connect(m_removeDeviceBtn, &QPushButton::clicked, this,
          &LoopModuleConfigWidget::onRemoveDevice);
  connect(m_saveBtn, &QPushButton::clicked, this,
          &LoopModuleConfigWidget::onSave);
}

void LoopModuleConfigWidget::loadData() {
  // Set Channel Count
  int count = m_module->getChannelCount();
  int index = m_channelCountCombo->findData(count);
  if (index != -1)
    m_channelCountCombo->setCurrentIndex(index);

  // Set Loop Mode
  int mode = (int)m_module->getLoopMode();
  index = m_loopModeCombo->findData(mode);
  if (index != -1)
    m_loopModeCombo->setCurrentIndex(index);

  // Set Options
  m_initCheckBox->setChecked(m_module->isInitialized());
  m_mapCheckBox->setChecked(m_module->isMappingSupported());

  // Populate Channel Select Combo
  m_channelSelectCombo->clear();
  for (int i = 0; i < count; ++i) {
    m_channelSelectCombo->addItem(QString("通道 %1").arg(i + 1), i);
  }

  // Load first channel
  if (count > 0) {
    m_currentChannelIndex = 0;
    updateDeviceTable(0);
  }
}

void LoopModuleConfigWidget::updateDeviceTable(int channelIndex) {
  m_deviceTable->setRowCount(0);

  if (!m_tempDevices.contains(channelIndex))
    return;

  const QList<LoopDevice> &devices = m_tempDevices[channelIndex];
  m_deviceTable->setRowCount(devices.size());

  for (int i = 0; i < devices.size(); ++i) {
    const LoopDevice &device = devices[i];

    // Type (ComboBox)
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItem("烟温复合探测器");
    typeCombo->addItem("手动报警按钮");
    typeCombo->addItem("输入输出模块");
    typeCombo->addItem("声光报警器");
    typeCombo->setCurrentText(device.type);
    m_deviceTable->setCellWidget(i, 0, typeCombo);

    // Serial Number
    m_deviceTable->setItem(i, 1, new QTableWidgetItem(device.serialNumber));

    // Address
    m_deviceTable->setItem(
        i, 2, new QTableWidgetItem(QString::number(device.address)));

    // Personality Code
    m_deviceTable->setItem(i, 3, new QTableWidgetItem(device.personalityCode));

    // Panel Number
    m_deviceTable->setItem(
        i, 4, new QTableWidgetItem(QString::number(device.panelNumber)));

    // Card Number
    m_deviceTable->setItem(
        i, 5, new QTableWidgetItem(QString::number(device.cardNumber)));

    // Description
    m_deviceTable->setItem(i, 6, new QTableWidgetItem(device.description));

    // Identifier
    m_deviceTable->setItem(i, 7, new QTableWidgetItem(device.identifier));

    // Variable Name
    m_deviceTable->setItem(i, 8, new QTableWidgetItem(device.variableName));
  }
}

void LoopModuleConfigWidget::saveCurrentChannelData() {
  QList<LoopDevice> devices;
  for (int i = 0; i < m_deviceTable->rowCount(); ++i) {
    LoopDevice device;

    // Type
    QComboBox *typeCombo =
        qobject_cast<QComboBox *>(m_deviceTable->cellWidget(i, 0));
    if (typeCombo)
      device.type = typeCombo->currentText();

    // Serial
    QTableWidgetItem *item = m_deviceTable->item(i, 1);
    if (item)
      device.serialNumber = item->text();

    // Address
    item = m_deviceTable->item(i, 2);
    if (item)
      device.address = item->text().toInt();

    // Personality
    item = m_deviceTable->item(i, 3);
    if (item)
      device.personalityCode = item->text();

    // Panel
    item = m_deviceTable->item(i, 4);
    if (item)
      device.panelNumber = item->text().toInt();

    // Card
    item = m_deviceTable->item(i, 5);
    if (item)
      device.cardNumber = item->text().toInt();

    // Description
    item = m_deviceTable->item(i, 6);
    if (item)
      device.description = item->text();

    // Identifier
    item = m_deviceTable->item(i, 7);
    if (item)
      device.identifier = item->text();

    // Variable Name
    item = m_deviceTable->item(i, 8);
    if (item)
      device.variableName = item->text();

    devices.append(device);
  }

  m_tempDevices[m_currentChannelIndex] = devices;
}

void LoopModuleConfigWidget::onChannelCountChanged(int index) {
  int newCount = m_channelCountCombo->itemData(index).toInt();

  // Update Channel Select Combo
  m_channelSelectCombo->clear();
  for (int i = 0; i < newCount; ++i) {
    m_channelSelectCombo->addItem(QString("通道 %1").arg(i + 1), i);
  }

  // If current index is out of range, reset to 0
  if (m_currentChannelIndex >= newCount) {
    m_currentChannelIndex = 0;
  }
  m_channelSelectCombo->setCurrentIndex(m_currentChannelIndex);

  updateDeviceTable(m_currentChannelIndex);
}

void LoopModuleConfigWidget::onChannelSelectionChanged(int index) {
  if (index < 0)
    return;

  // Save current table data before switching
  saveCurrentChannelData();

  m_currentChannelIndex = index;
  updateDeviceTable(m_currentChannelIndex);
}

void LoopModuleConfigWidget::onAddDevice() {
  int row = m_deviceTable->rowCount();

  // Calculate next address
  int nextAddress = 1;
  if (row > 0) {
    QTableWidgetItem *lastAddressItem = m_deviceTable->item(row - 1, 2);
    if (lastAddressItem) {
      nextAddress = lastAddressItem->text().toInt() + 1;
    }
  }

  m_deviceTable->insertRow(row);

  // Initialize new row with default widgets
  QComboBox *typeCombo = new QComboBox();
  typeCombo->addItem("烟温复合探测器");
  typeCombo->addItem("手动报警按钮");
  typeCombo->addItem("输入输出模块");
  typeCombo->addItem("声光报警器");
  m_deviceTable->setCellWidget(row, 0, typeCombo);

  m_deviceTable->setItem(row, 1, new QTableWidgetItem(""));
  m_deviceTable->setItem(row, 2,
                         new QTableWidgetItem(QString::number(nextAddress)));
  m_deviceTable->setItem(row, 3, new QTableWidgetItem(""));
  m_deviceTable->setItem(row, 4, new QTableWidgetItem("0"));
  m_deviceTable->setItem(row, 5, new QTableWidgetItem("0"));
  m_deviceTable->setItem(row, 6, new QTableWidgetItem(""));
  m_deviceTable->setItem(row, 7, new QTableWidgetItem(""));
  m_deviceTable->setItem(row, 8, new QTableWidgetItem(""));
}

void LoopModuleConfigWidget::onRemoveDevice() {
  QList<QTableWidgetItem *> selectedItems = m_deviceTable->selectedItems();
  if (selectedItems.isEmpty())
    return;

  // Get unique rows to remove
  QSet<int> rows;
  for (auto item : selectedItems) {
    rows.insert(item->row());
  }

  // Remove from bottom up
  QList<int> sortedRows = rows.values();
  std::sort(sortedRows.begin(), sortedRows.end(), std::greater<int>());

  for (int row : sortedRows) {
    m_deviceTable->removeRow(row);
  }
}

void LoopModuleConfigWidget::onSave() {
  save();
  QMessageBox::information(this, "保存", "配置已保存");
}

void LoopModuleConfigWidget::save() {
  // Save the currently visible table data first
  saveCurrentChannelData();

  // Apply all changes to the module
  m_module->setChannelCount(m_channelCountCombo->currentData().toInt());
  m_module->setLoopMode((LoopMode)m_loopModeCombo->currentData().toInt());
  m_module->setInitialized(m_initCheckBox->isChecked());
  m_module->setMappingSupported(m_mapCheckBox->isChecked());

  // Save devices for each channel
  for (auto it = m_tempDevices.begin(); it != m_tempDevices.end(); ++it) {
    // Only save up to the current channel count
    if (it.key() < m_module->getChannelCount()) {
      m_module->setDevices(it.key(), it.value());
    }
  }
}

void LoopModuleConfigWidget::onLoopModeChanged(int index) {
  // Logic for loop mode change if needed immediately
}
