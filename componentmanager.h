#ifndef COMPONENTMANAGER_H
#define COMPONENTMANAGER_H

#include "dimodule.h"
#include "domodule.h"
#include "hostmodule.h"
#include "loopmodule.h"
#include <QList>
#include <QObject>
#include <QStandardItem>
#include <QString>

// 组件信息结构体
struct ComponentInfo {
  QString name;
  QString type;
  QString description;
  int level;
  QString iconPath;
};

class ComponentManager : public QObject {
  Q_OBJECT

public:
  explicit ComponentManager(QObject *parent = nullptr);
  ~ComponentManager();

  void showAddComponentDialog();
  void showConfigureComponentDialog(QStandardItem *item = nullptr);
  void showDeleteComponentDialog(QStandardItem *item);
  void showMoveComponentDialog(QStandardItem *item);
  void moveComponentUp(QStandardItem *item);
  void moveComponentDown(QStandardItem *item);

  // 获取组件配置部件
  QWidget *getComponentConfigWidget(QStandardItem *item);

  // 添加DI模块配置对话框
  void showDIModuleConfigDialog(QStandardItem *item);

  // 添加DO模块配置对话框
  void showDOModuleConfigDialog(QStandardItem *item);

  // 添加主机模块配置对话框
  void showHostModuleConfigDialog(QStandardItem *item);

  // 添加回路模块配置对话框
  void showLoopModuleConfigDialog(QStandardItem *item);

  QList<ComponentInfo> getComponentTypes() const;

signals:
  void componentAdded(const ComponentInfo &component);
  void componentDeleted(QStandardItem *item);
  void componentMoved(QStandardItem *item, QStandardItem *newParent);
  void componentOrderChanged(QStandardItem *item, bool moveUp);

private:
  void initializeComponentTypes();
  QList<ComponentInfo> m_componentTypes;

  // 添加DI模块实例
  DIModule *m_diModule;

  // 添加DO模块实例
  DOModule *m_doModule;

  // 添加回路模块实例
  LoopModule *m_loopModule;

  // 添加主机模块映射，每个组件项对应一个独立的主机模块实例
  QMap<QStandardItem *, HostModule *> m_hostModules;

  // 添加辅助方法
  HostModule *getOrCreateHostModule(QStandardItem *item);
  void removeHostModule(QStandardItem *item);
};

#endif // COMPONENTMANAGER_H
