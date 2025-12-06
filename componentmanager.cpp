#include "componentmanager.h"
#include <QDialog>
#include <QDialogButtonBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QStandardItem> // 添加此头文件
#include <QTreeView>     // 添加此头文件
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "dimoduleconfigdialog.h"   // 添加DI模块配置对话框头文件
#include "dimoduleconfigwidget.h"   // 添加DI模块配置部件头文件
#include "domoduleconfigdialog.h"   // 添加DO模块配置对话框头文件
#include "domoduleconfigwidget.h"   // 添加DO模块配置部件头文件
#include "hostmoduleconfigdialog.h" // 添加主机模块配置对话框头文件
#include "hostmoduleconfigwidget.h" // 添加主机模块配置部件头文件
#include "loopmoduleconfigdialog.h" // 添加回路模块配置对话框头文件
#include "loopmoduleconfigwidget.h" // 添加回路模块配置部件头文件

ComponentManager::ComponentManager(QObject *parent) : QObject(parent) {
  // 初始化组件类型列表
  initializeComponentTypes();

  // 初始化DI模块
  m_diModule = new DIModule(this);

  // 初始化DO模块
  m_doModule = new DOModule(this);

  // 初始化回路模块
  m_loopModule = new LoopModule(this);

  // 不再初始化单一的主机模块实例
  // m_hostModule = new HostModule(this);
}

// 添加获取或创建主机模块的方法
HostModule *ComponentManager::getOrCreateHostModule(QStandardItem *item) {
  if (!m_hostModules.contains(item)) {
    // 为每个主机模块组件创建独立的实例
    HostModule *hostModule = new HostModule(this);
    m_hostModules[item] = hostModule;

    // 可以根据组件名称设置默认配置
    HostConfiguration config = hostModule->getConfiguration();
    config.hostName = item->text(); // 使用组件名称作为主机名
    hostModule->setConfiguration(config);
  }
  return m_hostModules[item];
}

void ComponentManager::removeHostModule(QStandardItem *item) {
  if (m_hostModules.contains(item)) {
    delete m_hostModules[item];
    m_hostModules.remove(item);
  }
}

void ComponentManager::showHostModuleConfigDialog(QStandardItem *item) {
  if (!item) {
    return;
  }

  // 获取或创建该组件对应的主机模块实例
  HostModule *hostModule = getOrCreateHostModule(item);

  // 创建主机模块配置对话框
  HostModuleConfigDialog dialog(hostModule);

  if (dialog.exec() == QDialog::Accepted) {
    // 配置已保存，可以在这里更新项目树中的组件信息
    // 例如，更新组件名称或图标等

    // 可以根据配置更新组件显示名称
    HostConfiguration config = hostModule->getConfiguration();
    if (!config.hostName.isEmpty()) {
      item->setText(config.hostName);
    }
  }
}

ComponentManager::~ComponentManager() {}

void ComponentManager::initializeComponentTypes() {
  // 定义组件类型及其层级关系
  // 第一层级 - 主机模块
  ComponentInfo hostModule;
  hostModule.name = "主机模块";
  hostModule.type = "HostModule";
  hostModule.description = "控制器主机模块";
  hostModule.level = 1;
  hostModule.iconPath = ":/icons/host.png"; // 设置图标路径
  m_componentTypes.append(hostModule);

  // 第二层级 - 其他模块
  QStringList secondLevelModules = {"回路模块", "DI模块",     "DO模块",
                                    "AI模块",   "继电器模块", "通信模块"};

  QStringList moduleTypes = {"LoopModule", "DIModule",    "DOModule",
                             "AIModule",   "RelayModule", "CommModule"};

  QStringList iconPaths = {":/icons/loop.png",  ":/icons/di.png",
                           ":/icons/do.png",    ":/icons/ai.png",
                           ":/icons/relay.png", ":/icons/comm.png"};

  for (int i = 0; i < secondLevelModules.size(); ++i) {
    ComponentInfo module;
    module.name = secondLevelModules[i];
    module.type = i < moduleTypes.size() ? moduleTypes[i] : "UnknownModule";
    module.description =
        QString("%1，连接到主机模块").arg(secondLevelModules[i]);
    module.level = 2;
    module.iconPath =
        i < iconPaths.size() ? iconPaths[i] : ":/icons/default.png";
    m_componentTypes.append(module);
  }
}

void ComponentManager::showAddComponentDialog() {
  QDialog dialog;
  dialog.setWindowTitle("添加组件");
  dialog.setMinimumWidth(400);

  QVBoxLayout *layout = new QVBoxLayout(&dialog);

  QLabel *label = new QLabel("选择要添加的组件类型:");
  layout->addWidget(label);

  QListWidget *componentList = new QListWidget(&dialog);
  for (const ComponentInfo &component : m_componentTypes) {
    QListWidgetItem *item = new QListWidgetItem(component.name);
    item->setToolTip(component.description);
    item->setData(Qt::UserRole, component.type);
    item->setData(Qt::UserRole + 1, component.level);
    item->setData(Qt::UserRole + 2, component.iconPath);

    // 设置图标
    if (!component.iconPath.isEmpty()) {
      item->setIcon(QIcon(component.iconPath));
    }

    componentList->addItem(item);
  }
  layout->addWidget(componentList);

  // 添加组件命名输入框
  QHBoxLayout *nameLayout = new QHBoxLayout();
  QLabel *nameLabel = new QLabel("组件名称:");
  QLineEdit *nameEdit = new QLineEdit();
  nameLayout->addWidget(nameLabel);
  nameLayout->addWidget(nameEdit);
  layout->addLayout(nameLayout);

  // 当选择组件时，自动填充默认名称
  connect(componentList, &QListWidget::currentItemChanged,
          [nameEdit](QListWidgetItem *current, QListWidgetItem *) {
            if (current) {
              nameEdit->setText(current->text());
              nameEdit->selectAll();
            }
          });

  // 添加双击处理
  connect(componentList, &QListWidget::itemDoubleClicked,
          [&dialog](QListWidgetItem *) { dialog.accept(); });

  QDialogButtonBox *buttonBox = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
  layout->addWidget(buttonBox);

  connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  // 如果没有选择组件，禁用确定按钮
  QPushButton *okButton = buttonBox->button(QDialogButtonBox::Ok);
  okButton->setEnabled(false);
  connect(componentList, &QListWidget::currentItemChanged,
          [okButton](QListWidgetItem *current, QListWidgetItem *) {
            okButton->setEnabled(current != nullptr);
          });

  if (dialog.exec() == QDialog::Accepted) {
    QListWidgetItem *selectedItem = componentList->currentItem();
    if (selectedItem) {
      QString componentName = nameEdit->text().trimmed();
      if (componentName.isEmpty()) {
        componentName = selectedItem->text();
      }

      QString componentType = selectedItem->data(Qt::UserRole).toString();
      int componentLevel = selectedItem->data(Qt::UserRole + 1).toInt();
      QString iconPath = selectedItem->data(Qt::UserRole + 2).toString();

      // 创建组件信息并发送信号
      ComponentInfo component;
      component.name = componentName;
      component.type = componentType;
      component.level = componentLevel;
      component.iconPath = iconPath;

      // 查找完整的组件信息
      for (const ComponentInfo &info : m_componentTypes) {
        if (info.type == componentType) {
          component.description = info.description;
          break;
        }
      }

      emit componentAdded(component);
    }
  }
}

QWidget *ComponentManager::getComponentConfigWidget(QStandardItem *item) {
  if (!item) {
    return nullptr;
  }

  // 根据组件类型显示不同的配置对话框
  QString componentType = item->data(Qt::UserRole).toString();

  if (componentType == "DIModule") {
    return new DIModuleConfigWidget(m_diModule);
  } else if (componentType == "DOModule") {
    return new DOModuleConfigWidget(m_doModule);
  } else if (componentType == "HostModule") {
    return new HostModuleConfigWidget(getOrCreateHostModule(item));
  } else if (componentType == "LoopModule") {
    return new LoopModuleConfigWidget(m_loopModule);
  }

  return new QLabel("此组件暂无详细配置界面或尚未实现。");
}

void ComponentManager::showConfigureComponentDialog(QStandardItem *item) {
  if (!item) {
    return;
  }

  // 根据组件类型显示不同的配置对话框
  QString componentType = item->data(Qt::UserRole).toString();

  if (componentType == "DIModule") {
    showDIModuleConfigDialog(item);
  } else if (componentType == "DOModule") {
    showDOModuleConfigDialog(item);
  } else if (componentType == "HostModule") {
    showHostModuleConfigDialog(item);
  } else if (componentType == "LoopModule") {
    showLoopModuleConfigDialog(item);
  } else {
    // 其他类型的组件配置
    QMessageBox::information(nullptr, "配置组件",
                             "组件配置功能将在后续版本中实现");
  }
}

void ComponentManager::showDIModuleConfigDialog(QStandardItem *item) {
  if (!item) {
    return;
  }

  // 创建DI模块配置对话框
  DIModuleConfigDialog dialog(m_diModule);

  if (dialog.exec() == QDialog::Accepted) {
    // 配置已保存，可以在这里更新项目树中的组件信息
    // 例如，更新组件名称或图标等
  }
}

void ComponentManager::showDOModuleConfigDialog(QStandardItem *item) {
  if (!item) {
    return;
  }

  // 创建DO模块配置对话框
  DOModuleConfigDialog dialog(m_doModule);

  if (dialog.exec() == QDialog::Accepted) {
    // 配置已保存，可以在这里更新项目树中的组件信息
    // 例如，更新组件名称或图标等
  }
}

void ComponentManager::showLoopModuleConfigDialog(QStandardItem *item) {
  if (!item) {
    return;
  }

  // 创建回路模块配置对话框
  LoopModuleConfigDialog dialog(m_loopModule);

  if (dialog.exec() == QDialog::Accepted) {
    // 配置已保存
  }
}

void ComponentManager::showDeleteComponentDialog(QStandardItem *item) {
  if (!item) {
    return;
  }

  QMessageBox msgBox;
  msgBox.setWindowTitle("删除组件");
  msgBox.setText(QString("确定要删除组件 \"%1\" 吗?").arg(item->text()));
  msgBox.setInformativeText("此操作不可撤销。");
  msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
  msgBox.setDefaultButton(QMessageBox::No);

  if (msgBox.exec() == QMessageBox::Yes) {
    // 如果是主机模块，清理对应的实例
    QString componentType = item->data(Qt::UserRole).toString();
    if (componentType == "HostModule") {
      removeHostModule(item);
    }

    emit componentDeleted(item);
  }
}

void ComponentManager::showMoveComponentDialog(QStandardItem *item) {
  if (!item) {
    return;
  }

  QDialog dialog;
  dialog.setWindowTitle("移动组件");
  dialog.setMinimumWidth(400);

  QVBoxLayout *layout = new QVBoxLayout(&dialog);

  QLabel *label =
      new QLabel(QString("选择 \"%1\" 的新位置:").arg(item->text()));
  layout->addWidget(label);

  // 获取项目模型
  QStandardItemModel *model = qobject_cast<QStandardItemModel *>(item->model());
  if (!model) {
    return;
  }

  // 创建树形视图显示可能的目标位置
  QTreeView *treeView = new QTreeView(&dialog);
  treeView->setModel(model);
  treeView->expandAll();
  layout->addWidget(treeView);

  QDialogButtonBox *buttonBox = new QDialogButtonBox(
      QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
  layout->addWidget(buttonBox);

  connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
  connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

  if (dialog.exec() == QDialog::Accepted) {
    QModelIndex selectedIndex = treeView->currentIndex();
    if (selectedIndex.isValid()) {
      QStandardItem *targetItem = model->itemFromIndex(selectedIndex);
      if (targetItem && targetItem != item) {
        emit componentMoved(item, targetItem);
      }
    }
  }
}

void ComponentManager::moveComponentUp(QStandardItem *item) {
  if (!item) {
    return;
  }

  QStandardItem *parentItem = item->parent();
  if (!parentItem) {
    return;
  }

  int row = item->row();
  if (row <= 0) {
    // 已经是第一个，无法上移
    return;
  }

  // 从父项中移除该项
  QList<QStandardItem *> rowItems = parentItem->takeRow(row);

  // 在上一行位置插入
  parentItem->insertRow(row - 1, rowItems);

  // 发出顺序变更信号
  emit componentOrderChanged(rowItems.first(), true);
}

void ComponentManager::moveComponentDown(QStandardItem *item) {
  if (!item) {
    return;
  }

  QStandardItem *parentItem = item->parent();
  if (!parentItem) {
    return;
  }

  int row = item->row();
  if (row >= parentItem->rowCount() - 1) {
    // 已经是最后一个，无法下移
    return;
  }

  // 从父项中移除该项
  QList<QStandardItem *> rowItems = parentItem->takeRow(row);

  // 在下一行位置插入
  parentItem->insertRow(row + 1, rowItems);

  // 发出顺序变更信号
  emit componentOrderChanged(rowItems.first(), false);
}

QList<ComponentInfo> ComponentManager::getComponentTypes() const {
  return m_componentTypes;
}
