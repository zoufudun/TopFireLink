#include "mainwindow.h"
#include "newprojectwizard.h"
#include "thememanager.h"
#include <QAction>
#include <QFileDialog>
#include <QInputDialog> // 添加此头文件
#include <QKeySequence>
#include <QLineEdit>
#include <QListWidget>
#include <QMenu> // 添加此头文件
#include <QMessageBox>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QTreeWidget>
#include <QVBoxLayout>
#include <QLabel>



MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  projectManager = new ProjectManager(this);
  componentManager = new ComponentManager(this);

  setupUI();
  // 初始化主题管理器
  themeManager = new ThemeManager(this);

  createActions();
  createMenus();
  createToolbars();
  createDockWindows();

  setWindowTitle(tr("Controller IDE"));
  setMinimumSize(800, 600);

  // 连接组件管理器信号
  connect(componentManager, &ComponentManager::componentAdded, this,
          &MainWindow::onComponentAdded);
  connect(componentManager, &ComponentManager::componentDeleted, this,
          &MainWindow::onComponentDeleted);
  connect(componentManager, &ComponentManager::componentMoved, this,
          &MainWindow::onComponentMoved);
  connect(componentManager, &ComponentManager::componentOrderChanged, this,
          &MainWindow::onComponentOrderChanged);
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
  // 中央编辑区域可以根据需要添加
  setCentralWidget(new QWidget());
}

void MainWindow::createActions() {
  newProjectAction = new QAction(tr("新建项目"), this);
  connect(newProjectAction, &QAction::triggered, this, &MainWindow::newProject);

  openProjectAction = new QAction(tr("打开项目"), this);
  connect(openProjectAction, &QAction::triggered, this,
          &MainWindow::openProject);

  saveProjectAction = new QAction(tr("保存项目"), this);
  connect(saveProjectAction, &QAction::triggered, this,
          &MainWindow::saveProject);

  saveAsProjectAction = new QAction(tr("项目另存为"), this);
  connect(saveAsProjectAction, &QAction::triggered, this,
          &MainWindow::saveProjectAs);

  addComponentAction = new QAction(tr("添加组件"), this);
  connect(addComponentAction, &QAction::triggered, this,
          &MainWindow::addComponent);

  configureComponentAction = new QAction(tr("配置组件"), this);
  connect(configureComponentAction, &QAction::triggered, this,
          &MainWindow::configureComponent);

  exitAction = new QAction(tr("退出"), this);
  connect(exitAction, &QAction::triggered, this, &QWidget::close);

  renameProjectAction = new QAction(tr("重命名项目"), this);
  connect(renameProjectAction, &QAction::triggered, this,
          &MainWindow::renameProject);

  deleteComponentAction = new QAction(tr("删除组件"), this);
  connect(deleteComponentAction, &QAction::triggered, this,
          &MainWindow::deleteComponent);

  moveComponentAction = new QAction(tr("移动组件"), this);
  connect(moveComponentAction, &QAction::triggered, this,
          &MainWindow::moveComponent);

  // 主题切换动作
  defaultThemeAction = new QAction(tr("默认主题"), this);
  defaultThemeAction->setCheckable(true);
  defaultThemeAction->setChecked(true);
  connect(defaultThemeAction, &QAction::triggered, this,
          &MainWindow::setDefaultTheme);

  atomOneThemeAction = new QAction(tr("ATOM ONE"), this);
  atomOneThemeAction->setCheckable(true);
  connect(atomOneThemeAction, &QAction::triggered, this,
          &MainWindow::setAtomOneTheme);

  solarizedLightThemeAction = new QAction(tr("Solarized Light"), this);
  solarizedLightThemeAction->setCheckable(true);
  connect(solarizedLightThemeAction, &QAction::triggered, this,
          &MainWindow::setSolarizedLightTheme);

  // 上移和下移组件动作
  moveUpAction = new QAction(tr("上移组件"), this);
  moveUpAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Up));
  connect(moveUpAction, &QAction::triggered, this,
          &MainWindow::moveComponentUp);

  moveDownAction = new QAction(tr("下移组件"), this);
  moveDownAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Down));
  connect(moveDownAction, &QAction::triggered, this,
          &MainWindow::moveComponentDown);
}

void MainWindow::createMenus() {
  QMenu *fileMenu = menuBar()->addMenu(tr("文件"));
  fileMenu->addAction(newProjectAction);
  fileMenu->addAction(openProjectAction);
  fileMenu->addAction(saveProjectAction);
  fileMenu->addAction(saveAsProjectAction);
  fileMenu->addAction(renameProjectAction); // 添加重命名项目菜单项
  fileMenu->addSeparator();
  fileMenu->addAction(exitAction);

  QMenu *componentMenu = menuBar()->addMenu(tr("组件"));
  componentMenu->addAction(addComponentAction);
  componentMenu->addAction(deleteComponentAction); // 添加删除组件菜单项
  componentMenu->addAction(moveComponentAction); // 添加移动组件菜单项
  componentMenu->addAction(configureComponentAction);

  // 添加主题菜单
  themeMenu = menuBar()->addMenu(tr("主题"));
  themeMenu->addAction(defaultThemeAction);
  themeMenu->addAction(atomOneThemeAction);
  themeMenu->addAction(solarizedLightThemeAction);

  // 添加到编辑菜单
  // Initialize editMenu
  editMenu = menuBar()->addMenu(tr("编辑"));
  editMenu->addSeparator();
  editMenu->addAction(moveUpAction);
  editMenu->addAction(moveDownAction);
}

void MainWindow::createToolbars() {
  QToolBar *fileToolBar = addToolBar(tr("文件"));
  fileToolBar->addAction(newProjectAction);
  fileToolBar->addAction(openProjectAction);
  fileToolBar->addAction(saveProjectAction);
  fileToolBar->addAction(renameProjectAction); // 添加重命名项目工具栏按钮

  QToolBar *componentToolBar = addToolBar(tr("组件"));
  componentToolBar->addAction(addComponentAction);
  componentToolBar->addAction(deleteComponentAction); // 添加删除组件工具栏按钮
  componentToolBar->addAction(moveComponentAction); // 添加移动组件工具栏按钮
  componentToolBar->addAction(configureComponentAction);
}

void MainWindow::createDockWindows() {
  // 项目树视图
  projectDock = new QDockWidget(tr("项目"), this);
  projectDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                               Qt::RightDockWidgetArea);
  projectTreeView = new QTreeView(projectDock);

  // 为项目树视图添加上下文菜单
  projectTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
  connect(projectTreeView, &QTreeView::customContextMenuRequested, this,
          &MainWindow::showProjectContextMenu);
  projectTreeView->setModel(projectManager->projectModel());

  // 连接选择改变信号
  connect(projectTreeView->selectionModel(),
          &QItemSelectionModel::currentChanged, this,
          &MainWindow::onProjectSelectionChanged);

  projectDock->setWidget(projectTreeView);
  addDockWidget(Qt::LeftDockWidgetArea, projectDock);

  // 组件列表
  componentDock = new QDockWidget(tr("组件库"), this);
  componentDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                                 Qt::RightDockWidgetArea);
  QListWidget *componentList = new QListWidget(componentDock);
  componentDock->setWidget(componentList);
  addDockWidget(Qt::LeftDockWidgetArea, componentDock);

  // 属性编辑器
  propertiesDock = new QDockWidget(tr("属性"), this);
  propertiesDock->setAllowedAreas(Qt::LeftDockWidgetArea |
                                  Qt::RightDockWidgetArea);
  QWidget *propertiesContainer = new QWidget(propertiesDock);
  QVBoxLayout *propertiesLayout = new QVBoxLayout(propertiesContainer);
  propertiesLayout->setContentsMargins(0, 0, 0, 0);
  propertiesContainer->setLayout(propertiesLayout);
  propertiesDock->setWidget(propertiesContainer);
  addDockWidget(Qt::RightDockWidgetArea, propertiesDock);
}

void MainWindow::onProjectSelectionChanged(const QModelIndex &current,
                                           const QModelIndex &previous) {
  Q_UNUSED(previous);

  // 获取属性停靠窗口的容器部件
  QWidget *container = propertiesDock->widget();
  if (!container)
    return;

  // 清除旧的配置界面
  if (container->layout()) {
    QLayoutItem *item;
    while ((item = container->layout()->takeAt(0)) != nullptr) {
      if (item->widget()) {
        delete item->widget();
      }
      delete item;
    }
  } else {
    // 如果没有布局，创建一个新的
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setContentsMargins(0, 0, 0, 0);
    container->setLayout(layout);
  }

  if (!current.isValid()) {
    return;
  }

  QStandardItem *item = projectManager->projectModel()->itemFromIndex(current);
  if (!item) {
    return;
  }

  // 获取新的配置界面
  QWidget *configWidget = componentManager->getComponentConfigWidget(item);
  if (configWidget) {
    container->layout()->addWidget(configWidget);
  } else {
    // 如果没有配置界面，显示默认信息
    QLabel *label = new QLabel(tr("选中项: %1").arg(item->text()),this);
    label->setAlignment(Qt::AlignCenter);
    container->layout()->addWidget(label);
  }
}

void MainWindow::newProject() {
  if (projectManager->hasUnsavedChanges()) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("保存更改"), tr("是否保存当前项目的更改?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Yes) {
      saveProject();
    } else if (reply == QMessageBox::Cancel) {
      return;
    }
  }

  NewProjectWizard wizard(this);
  if (wizard.exec() == QDialog::Accepted) {
    if (wizard.isImport()) {
      projectManager->loadProject(wizard.importPath());
    } else {
      projectManager->newProject(wizard.projectName(), wizard.projectPath());
    }
  }
}

void MainWindow::openProject() {
  if (projectManager->hasUnsavedChanges()) {
    QMessageBox::StandardButton reply = QMessageBox::question(
        this, tr("保存更改"), tr("是否保存当前项目的更改?"),
        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

    if (reply == QMessageBox::Yes) {
      saveProject();
    } else if (reply == QMessageBox::Cancel) {
      return;
    }
  }

  QString fileName = QFileDialog::getOpenFileName(this, tr("打开项目"), "",
                                                  tr("XML 项目文件 (*.xml)"));

  if (!fileName.isEmpty()) {
    projectManager->loadProject(fileName);
  }
}

void MainWindow::saveProject() {
  if (projectManager->currentProjectPath().isEmpty()) {
    saveProjectAs();
  } else {
    projectManager->saveProject(projectManager->currentProjectPath());
  }
}

void MainWindow::saveProjectAs() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("保存项目"), "",
                                                  tr("XML 项目文件 (*.xml)"));

  if (!fileName.isEmpty()) {
    projectManager->saveProject(fileName);
  }
}

void MainWindow::addComponent() { componentManager->showAddComponentDialog(); }

void MainWindow::configureComponent() {
  // 获取当前选中的项目
  QModelIndex selectedIndex = projectTreeView->currentIndex();
  if (!selectedIndex.isValid()) {
    QMessageBox::information(this, tr("配置组件"), tr("请先选择一个组件"));
    return;
  }

  QStandardItemModel *model =
      qobject_cast<QStandardItemModel *>(projectTreeView->model());
  if (!model) {
    return;
  }

  QStandardItem *item = model->itemFromIndex(selectedIndex);
  if (item) {
    componentManager->showConfigureComponentDialog(item);
  }
}

// 添加组件处理函数
void MainWindow::onComponentAdded(const ComponentInfo &component) {
  // 获取项目树视图的模型
  QStandardItemModel *model = projectManager->projectModel();
  if (!model) {
    return;
  }

  // 如果项目为空，创建一个根节点
  if (model->rowCount() == 0) {
    QStandardItem *rootItem = new QStandardItem("新项目");
    model->appendRow(rootItem);
  }

  // 获取当前选中的项
  QModelIndex selectedIndex = projectTreeView->currentIndex();
  QStandardItem *parentItem = nullptr;

  if (selectedIndex.isValid()) {
    parentItem = model->itemFromIndex(selectedIndex);
  } else {
    parentItem = model->item(0); // 根节点
  }

  // 根据组件层级添加到项目树中
  if (component.level == 1) {
    // 第一层级 - 主机模块
    // 如果选中的是根节点，直接添加到根节点下
    // 如果选中的不是根节点，添加到根节点下
    QStandardItem *rootItem = model->item(0);
    QStandardItem *item = new QStandardItem(component.name);
    item->setData(component.type, Qt::UserRole);

    // 设置图标
    if (!component.iconPath.isEmpty()) {
      item->setIcon(QIcon(component.iconPath));
    }

    rootItem->appendRow(item);
  } else if (component.level == 2) {
    // 第二层级 - 其他模块，需要添加到主机模块下
    // 查找主机模块
    QStandardItem *hostItem = nullptr;

    // 如果选中的是主机模块，直接添加到主机模块下
    if (parentItem &&
        parentItem->data(Qt::UserRole).toString() == "HostModule") {
      hostItem = parentItem;
    } else {
      // 否则查找主机模块
      QStandardItem *rootItem = model->item(0);
      for (int i = 0; i < rootItem->rowCount(); ++i) {
        QStandardItem *child = rootItem->child(i);
        if (child->data(Qt::UserRole).toString() == "HostModule") {
          hostItem = child;
          break;
        }
      }
    }

    if (!hostItem) {
      // 如果没有主机模块，先创建一个
      QMessageBox::warning(this, "添加组件", "请先添加主机模块！");
      return;
    }

    QStandardItem *item = new QStandardItem(component.name);
    item->setData(component.type, Qt::UserRole);

    // 设置图标
    if (!component.iconPath.isEmpty()) {
      item->setIcon(QIcon(component.iconPath));
    }

    hostItem->appendRow(item);
  }

  // 设置项目树视图的模型
  projectTreeView->setModel(model);
  projectTreeView->expandAll();

  // 标记项目有未保存的更改
  projectManager->setUnsavedChanges(true);

  statusBar()->showMessage(QString("已添加组件: %1").arg(component.name), 3000);
}

void MainWindow::renameProject() {
  bool ok;
  QString newName = QInputDialog::getText(
      this, tr("重命名项目"), tr("项目名称:"), QLineEdit::Normal,
      projectManager->projectModel()->item(0)->text(), &ok);
  if (ok && !newName.isEmpty()) {
    projectManager->renameProject(newName);
    statusBar()->showMessage(tr("项目已重命名为: %1").arg(newName), 3000);
  }
}

void MainWindow::deleteComponent() {
  QModelIndex currentIndex = projectTreeView->currentIndex();
  if (currentIndex.isValid() && currentIndex.parent().isValid()) {
    QStandardItem *item =
        projectManager->projectModel()->itemFromIndex(currentIndex);
    componentManager->showDeleteComponentDialog(item);
  } else {
    QMessageBox::warning(this, tr("删除组件"), tr("请先选择要删除的组件"));
  }
}

void MainWindow::moveComponent() {
  QModelIndex currentIndex = projectTreeView->currentIndex();
  if (currentIndex.isValid() && currentIndex.parent().isValid()) {
    QStandardItem *item =
        projectManager->projectModel()->itemFromIndex(currentIndex);
    componentManager->showMoveComponentDialog(item);
  } else {
    QMessageBox::warning(this, tr("移动组件"), tr("请先选择要移动的组件"));
  }
}

void MainWindow::onComponentDeleted(QStandardItem *item) {
  if (item) {
    QStandardItem *parentItem = item->parent();
    if (parentItem) {
      // 从父项中移除该项
      parentItem->removeRow(item->row());

      projectManager->setUnsavedChanges(true);
      statusBar()->showMessage(tr("组件已删除"), 3000);
    }
  }
}

void MainWindow::onComponentMoved(QStandardItem *item,
                                  QStandardItem *newParent) {
  if (item && newParent) {
    QStandardItem *oldParent = item->parent();
    if (oldParent) {
      // 保存项目文本，因为移除后 item 可能无效
      QString itemText = item->text();
      QVariant itemData = item->data(Qt::UserRole);

      // 创建一个新的项，复制原项的数据
      QStandardItem *newItem = new QStandardItem(itemText);
      newItem->setData(itemData, Qt::UserRole);

      // 从原位置移除
      oldParent->removeRow(item->row());

      // 添加到新位置
      newParent->appendRow(newItem);

      projectManager->setUnsavedChanges(true);
      statusBar()->showMessage(tr("组件已移动: %1").arg(itemText), 3000);
    }
  }
}

void MainWindow::showProjectContextMenu(const QPoint &pos) {
  QModelIndex index = projectTreeView->indexAt(pos);
  if (index.isValid()) {
    QMenu contextMenu(this);

    // 如果是根节点，添加重命名项目选项
    if (!index.parent().isValid()) {
      QAction *renameAction = new QAction(tr("重命名项目"), this);
      connect(renameAction, &QAction::triggered, this,
              &MainWindow::renameProject);
      contextMenu.addAction(renameAction);
      contextMenu.addSeparator();
    }

    // 添加组件选项
    QAction *addAction = new QAction(tr("添加组件"), this);
    connect(addAction, &QAction::triggered, this, &MainWindow::addComponent);
    contextMenu.addAction(addAction);

    // 如果不是根节点，添加组件配置、删除和移动选项
    if (index.parent().isValid()) {
      QAction *configureAction = new QAction(tr("配置组件"), this);
      connect(configureAction, &QAction::triggered, this, [this]() {
        QModelIndex currentIndex = projectTreeView->currentIndex();
        if (currentIndex.isValid()) {
          QStandardItem *item =
              projectManager->projectModel()->itemFromIndex(currentIndex);
          componentManager->showConfigureComponentDialog(item);
        }
      });
      contextMenu.addAction(configureAction);

      QAction *deleteAction = new QAction(tr("删除组件"), this);
      connect(deleteAction, &QAction::triggered, this, [this]() {
        QModelIndex currentIndex = projectTreeView->currentIndex();
        if (currentIndex.isValid()) {
          QStandardItem *item =
              projectManager->projectModel()->itemFromIndex(currentIndex);
          componentManager->showDeleteComponentDialog(item);
        }
      });
      contextMenu.addAction(deleteAction);

      QAction *moveAction = new QAction(tr("移动组件"), this);
      connect(moveAction, &QAction::triggered, this, [this]() {
        QModelIndex currentIndex = projectTreeView->currentIndex();
        if (currentIndex.isValid()) {
          QStandardItem *item =
              projectManager->projectModel()->itemFromIndex(currentIndex);
          componentManager->showMoveComponentDialog(item);
        }
      });
      contextMenu.addAction(moveAction);

      // 添加上移和下移选项
      contextMenu.addSeparator();

      QAction *moveUpAction = new QAction(tr("上移"), this);
      connect(moveUpAction, &QAction::triggered, this,
              &MainWindow::moveComponentUp);
      contextMenu.addAction(moveUpAction);

      QAction *moveDownAction = new QAction(tr("下移"), this);
      connect(moveDownAction, &QAction::triggered, this,
              &MainWindow::moveComponentDown);
      contextMenu.addAction(moveDownAction);
    }

    // 显示上下文菜单
    contextMenu.exec(projectTreeView->viewport()->mapToGlobal(pos));
  }
}

void MainWindow::setDefaultTheme() {
  themeManager->applyTheme(ThemeManager::Default);
  defaultThemeAction->setChecked(true);
  atomOneThemeAction->setChecked(false);
  solarizedLightThemeAction->setChecked(false);
  statusBar()->showMessage(tr("已应用默认主题"), 3000);
}

void MainWindow::setAtomOneTheme() {
  themeManager->applyTheme(ThemeManager::AtomOne);
  defaultThemeAction->setChecked(false);
  atomOneThemeAction->setChecked(true);
  solarizedLightThemeAction->setChecked(false);
  statusBar()->showMessage(tr("已应用ATOM ONE主题"), 3000);
}

void MainWindow::setSolarizedLightTheme() {
  themeManager->applyTheme(ThemeManager::SolarizedLight);
  defaultThemeAction->setChecked(false);
  atomOneThemeAction->setChecked(false);
  solarizedLightThemeAction->setChecked(true);
  statusBar()->showMessage(tr("已应用Solarized Light主题"), 3000);
}

void MainWindow::changeTheme() {
  // This method can either show a theme selection dialog
  // or toggle between themes. For simplicity, I'll implement
  // a basic theme rotation.

  if (defaultThemeAction->isChecked()) {
    setAtomOneTheme();
  } else if (atomOneThemeAction->isChecked()) {
    setSolarizedLightTheme();
  } else {
    setDefaultTheme();
  }
}

void MainWindow::onComponentOrderChanged(QStandardItem *item, bool moveUp) {
  if (item) {
    // 选中移动后的项
    QModelIndex index = item->index();
    projectTreeView->setCurrentIndex(index);

    // 标记项目有未保存的更改
    projectManager->setUnsavedChanges(true);

    QString direction = moveUp ? "上移" : "下移";
    statusBar()->showMessage(
        tr("组件已%1: %2").arg(direction).arg(item->text()), 3000);
  }
}

void MainWindow::moveComponentUp() {
  QModelIndex currentIndex = projectTreeView->currentIndex();
  if (currentIndex.isValid() && currentIndex.parent().isValid()) {
    QStandardItem *item =
        projectManager->projectModel()->itemFromIndex(currentIndex);
    componentManager->moveComponentUp(item);
  } else {
    QMessageBox::warning(this, tr("移动组件"), tr("请先选择要移动的组件"));
  }
}

void MainWindow::moveComponentDown() {
  QModelIndex currentIndex = projectTreeView->currentIndex();
  if (currentIndex.isValid() && currentIndex.parent().isValid()) {
    QStandardItem *item =
        projectManager->projectModel()->itemFromIndex(currentIndex);
    componentManager->moveComponentDown(item);
  } else {
    QMessageBox::warning(this, tr("移动组件"), tr("请先选择要移动的组件"));
  }
}
