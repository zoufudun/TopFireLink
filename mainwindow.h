#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "componentmanager.h"
#include "projectmanager.h"
#include "thememanager.h"
#include <QDockWidget>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeView>


class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void newProject();
  void openProject();
  void saveProject();
  void saveProjectAs();
  void renameProject(); // 添加重命名项目的槽函数
  void addComponent();
  void deleteComponent(); // 添加删除组件的槽函数
  void moveComponent();   // 添加移动组件的槽函数
  void configureComponent();
  void onComponentAdded(const ComponentInfo &component);
  void onComponentDeleted(QStandardItem *item);
  void onComponentMoved(QStandardItem *item, QStandardItem *newParent);

  void moveComponentUp();
  void moveComponentDown();
  void onComponentOrderChanged(QStandardItem *item, bool moveUp);
  void onProjectSelectionChanged(const QModelIndex &current,
                                 const QModelIndex &previous);

private:
  void showProjectContextMenu(const QPoint &pos);
  void changeTheme();
  void setDefaultTheme();
  void setAtomOneTheme();
  void setSolarizedLightTheme();
  void setupUI();
  void createActions();
  void createMenus();
  void createToolbars();
  void createDockWindows();

  QTreeView *projectTreeView;
  QDockWidget *projectDock;
  QDockWidget *componentDock;
  QDockWidget *propertiesDock;

  ProjectManager *projectManager;
  ComponentManager *componentManager;
  ThemeManager *themeManager;
  QMenu *themeMenu;
  QMenu *editMenu; // Add this line to declare editMenu
  QAction *defaultThemeAction;
  QAction *atomOneThemeAction;
  QAction *solarizedLightThemeAction;

  // Actions
  QAction *newProjectAction;
  QAction *openProjectAction;
  QAction *saveProjectAction;
  QAction *saveAsProjectAction;
  QAction *renameProjectAction; // 添加重命名项目的动作
  QAction *addComponentAction;
  QAction *deleteComponentAction; // 添加删除组件的动作
  QAction *moveComponentAction;   // 添加移动组件的动作
  QAction *configureComponentAction;
  QAction *exitAction;
  QAction *moveUpAction;
  QAction *moveDownAction;
};

#endif // MAINWINDOW_H
