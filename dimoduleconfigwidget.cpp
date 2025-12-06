#include "dimoduleconfigwidget.h"
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QMessageBox>
#include <QSplitter>
#include <QVBoxLayout>


DIModuleConfigWidget::DIModuleConfigWidget(DIModule *module, QWidget *parent)
    : QWidget(parent), m_module(module), m_currentChannelIndex(0) {
  setupUI();
  updateBitTable(m_currentChannelIndex);
}

DIModuleConfigWidget::~DIModuleConfigWidget() {}

void DIModuleConfigWidget::setupUI() {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);

  // 通道数量选择
  QHBoxLayout *channelCountLayout = new QHBoxLayout();
  QLabel *channelCountLabel = new QLabel("通道数量:", this);
  m_channelCountCombo = new QComboBox(this);
  m_channelCountCombo->addItem("8通道", 8);
  m_channelCountCombo->addItem("16通道", 16);
  m_channelCountCombo->addItem("32通道", 32);

  // 设置当前选中的通道数量
  int currentIndex = 0;
  switch (m_module->getChannelCount()) {
  case 8:
    currentIndex = 0;
    break;
  case 16:
    currentIndex = 1;
    break;
  case 32:
    currentIndex = 2;
    break;
  }
  m_channelCountCombo->setCurrentIndex(currentIndex);

  channelCountLayout->addWidget(channelCountLabel);
  channelCountLayout->addWidget(m_channelCountCombo);

  // 添加通道选择下拉框
  QLabel *channelSelectLabel = new QLabel("选择通道:", this);
  m_channelSelectCombo = new QComboBox(this);

  // 根据当前通道数量填充通道选择下拉框
  for (int i = 0; i < m_module->getChannelCount(); ++i) {
    m_channelSelectCombo->addItem(QString("通道 %1").arg(i), i);
  }

  channelCountLayout->addSpacing(20);
  channelCountLayout->addWidget(channelSelectLabel);
  channelCountLayout->addWidget(m_channelSelectCombo);
  channelCountLayout->addStretch();

  mainLayout->addLayout(channelCountLayout);

  // 位变量表格 - 添加值列
  m_bitTable = new QTableWidget(8, 4, this);
  m_bitTable->setHorizontalHeaderLabels(QStringList() << "位"
                                                      << "变量名"
                                                      << "值"
                                                      << "描述");
  m_bitTable->verticalHeader()->setVisible(false);

  // 设置表格列可以拖动调整大小
  m_bitTable->horizontalHeader()->setSectionResizeMode(
      QHeaderView::Interactive);
  m_bitTable->horizontalHeader()->setStretchLastSection(true);

  // 允许通过拖拽调整列的顺序
  m_bitTable->horizontalHeader()->setSectionsMovable(true);

  // 设置默认列宽
  m_bitTable->setColumnWidth(0, 50);  // 位列宽度较小
  m_bitTable->setColumnWidth(1, 200); // 变量名列宽度适中
  m_bitTable->setColumnWidth(2, 50);  // 值列宽度较小

  mainLayout->addWidget(m_bitTable);

  // 添加提示标签
  QLabel *hintLabel = new QLabel("提示：可以拖动表头调整列的顺序", this);
  hintLabel->setStyleSheet("color: gray; font-style: italic;");
  mainLayout->addWidget(hintLabel);

  // 按钮
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  m_saveButton = new QPushButton("保存配置", this);

  buttonLayout->addStretch();
  buttonLayout->addWidget(m_saveButton);

  mainLayout->addLayout(buttonLayout);

  // 连接信号
  connect(m_channelCountCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &DIModuleConfigWidget::onChannelCountChanged);
  connect(m_channelSelectCombo,
          QOverload<int>::of(&QComboBox::currentIndexChanged), this,
          &DIModuleConfigWidget::onChannelSelectionChanged);
  connect(m_saveButton, &QPushButton::clicked, this,
          &DIModuleConfigWidget::saveConfiguration);
  connect(m_bitTable, &QTableWidget::cellChanged, this,
          &DIModuleConfigWidget::onBitVariableChanged);

  // 连接列顺序变化信号
  connect(m_bitTable->horizontalHeader(), &QHeaderView::sectionMoved, this,
          [this](int logicalIndex, int oldVisualIndex, int newVisualIndex) {
            // 可以在这里添加列顺序变化的处理逻辑
            // 例如，保存当前的列顺序到配置中
          });
}

void DIModuleConfigWidget::updateBitTable(int channelIndex) {
  if (channelIndex < 0 || channelIndex >= m_module->getChannelCount()) {
    return;
  }

  // 断开信号连接，避免触发onBitVariableChanged
  disconnect(m_bitTable, &QTableWidget::cellChanged, this, nullptr);

  DIChannel *channel = m_module->getChannel(channelIndex);
  if (!channel) {
    return;
  }

  // 保存当前的列顺序
  QList<int> columnOrder;
  for (int i = 0; i < m_bitTable->columnCount(); ++i) {
    columnOrder.append(m_bitTable->horizontalHeader()->visualIndex(i));
  }

  // 更新表格数据
  for (int j = 0; j < 8; ++j) {
    // 位编号
    QTableWidgetItem *bitItem = m_bitTable->item(j, 0);
    if (!bitItem) {
      bitItem = new QTableWidgetItem(QString::number(j));
      bitItem->setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);
      m_bitTable->setItem(j, 0, bitItem);
    }

    // 变量名
    QTableWidgetItem *nameItem = m_bitTable->item(j, 1);
    if (!nameItem) {
      nameItem = new QTableWidgetItem(channel->bits[j].name);
      m_bitTable->setItem(j, 1, nameItem);
    } else {
      nameItem->setText(channel->bits[j].name);
    }

    // 值 - 添加下拉框
    QComboBox *valueCombo =
        qobject_cast<QComboBox *>(m_bitTable->cellWidget(j, 2));
    if (!valueCombo) {
      valueCombo = new QComboBox();
      valueCombo->addItem("0", 0);
      valueCombo->addItem("1", 1);
      valueCombo->setCurrentIndex(channel->bits[j].value);

      // 连接值变化信号
      connect(valueCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
              this, [this, j](int index) {
                DIBitVariable variable =
                    m_module->getBitVariable(m_currentChannelIndex, j);
                variable.value = index;
                m_module->setBitVariable(m_currentChannelIndex, j, variable);
              });

      m_bitTable->setCellWidget(j, 2, valueCombo);
    } else {
      valueCombo->setCurrentIndex(channel->bits[j].value);
    }

    // 描述
    QTableWidgetItem *descItem = m_bitTable->item(j, 3);
    if (!descItem) {
      descItem = new QTableWidgetItem(channel->bits[j].description);
      m_bitTable->setItem(j, 3, descItem);
    } else {
      descItem->setText(channel->bits[j].description);
    }
  }

  // 恢复列顺序
  for (int i = 0; i < columnOrder.size(); ++i) {
    int visualIndex = m_bitTable->horizontalHeader()->visualIndex(i);
    if (visualIndex != columnOrder[i]) {
      m_bitTable->horizontalHeader()->moveSection(visualIndex, columnOrder[i]);
    }
  }

  // 重新连接信号
  connect(m_bitTable, &QTableWidget::cellChanged, this,
          &DIModuleConfigWidget::onBitVariableChanged);
}

void DIModuleConfigWidget::onBitVariableChanged(int row, int column) {
  // 获取逻辑列索引对应的实际数据列
  int nameColumn = 1; // 变量名列的逻辑索引
  int descColumn = 3; // 描述列的逻辑索引

  // 只处理变量名和描述列
  if (column != nameColumn && column != descColumn) {
    return;
  }

  // 获取变量名和描述
  QString name = m_bitTable->item(row, nameColumn)
                     ? m_bitTable->item(row, nameColumn)->text()
                     : "";
  QString description = m_bitTable->item(row, descColumn)
                            ? m_bitTable->item(row, descColumn)->text()
                            : "";

  // 获取当前值
  QComboBox *valueCombo =
      qobject_cast<QComboBox *>(m_bitTable->cellWidget(row, 2));
  int value = valueCombo ? valueCombo->currentIndex() : 0;

  // 更新模块数据
  DIBitVariable variable;
  variable.name = name;
  variable.description = description;
  variable.isGlobal = true; // 默认设置为全局变量
  variable.value = value;   // 设置值

  m_module->setBitVariable(m_currentChannelIndex, row, variable);
}

void DIModuleConfigWidget::onChannelCountChanged(int index) {
  // 获取新的通道数量
  int newChannelCount = m_channelCountCombo->itemData(index).toInt();

  // 更新模块的通道数量
  m_module->setChannelCount(newChannelCount);

  // 更新通道选择下拉框
  m_channelSelectCombo->clear();
  for (int i = 0; i < newChannelCount; ++i) {
    m_channelSelectCombo->addItem(QString("通道 %1").arg(i), i);
  }

  // 重置当前通道索引
  m_currentChannelIndex = 0;
  m_channelSelectCombo->setCurrentIndex(0);

  // 更新位变量表格
  updateBitTable(m_currentChannelIndex);
}

void DIModuleConfigWidget::onChannelSelectionChanged(int index) {
  m_currentChannelIndex = index;
  updateBitTable(m_currentChannelIndex);
}

void DIModuleConfigWidget::saveConfiguration() {
  // 这里可以添加保存配置的逻辑
  // 例如，保存到文件或数据库

  QMessageBox::information(this, "保存配置", "DI模块配置已保存");
}
