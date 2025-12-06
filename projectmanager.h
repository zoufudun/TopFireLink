#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QObject>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

class ProjectManager : public QObject {
  Q_OBJECT

public:
  explicit ProjectManager(QObject *parent = nullptr);
  ~ProjectManager();

  bool hasUnsavedChanges() const;
  void setUnsavedChanges(bool unsaved);
  QString currentProjectPath() const;

  void newProject(const QString &name, const QString &path = QString());
  void loadProject(const QString &path);
  void saveProject(const QString &path);
  void renameProject(const QString &newName);

  QStandardItemModel *projectModel();

private:
  void saveItemToXml(QXmlStreamWriter &writer, QStandardItem *item);
  void loadItemFromXml(QXmlStreamReader &reader, QStandardItem *parentItem);

  QStandardItemModel *m_model;
  QString m_currentProjectPath;
  bool m_hasUnsavedChanges;
};

#endif // PROJECTMANAGER_H
