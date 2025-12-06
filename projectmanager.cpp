#include "projectmanager.h"
#include <QDebug>
#include <QFile>
#include <QIcon>

ProjectManager::ProjectManager(QObject *parent)
    : QObject(parent), m_hasUnsavedChanges(false) {
  m_model = new QStandardItemModel(this);
  m_model->setHorizontalHeaderLabels(QStringList() << "项目结构");
}

ProjectManager::~ProjectManager() {}

bool ProjectManager::hasUnsavedChanges() const { return m_hasUnsavedChanges; }

void ProjectManager::setUnsavedChanges(bool unsaved) {
  m_hasUnsavedChanges = unsaved;
}

QString ProjectManager::currentProjectPath() const {
  return m_currentProjectPath;
}

void ProjectManager::newProject(const QString &name, const QString &path) {
  if (!path.isEmpty()) {
    m_currentProjectPath = path + "/" + name + ".xml";
  } else {
    m_currentProjectPath.clear();
  }
  m_hasUnsavedChanges = true;

  m_model->clear();
  m_model->setHorizontalHeaderLabels(QStringList() << "项目结构");

  QStandardItem *rootItem = new QStandardItem(name);
  rootItem->setData("Project", Qt::UserRole);
  rootItem->setIcon(QIcon(":/icons/default.png"));
  m_model->appendRow(rootItem);
}

void ProjectManager::loadProject(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return;
  }

  QXmlStreamReader reader(&file);

  m_model->clear();
  m_model->setHorizontalHeaderLabels(QStringList() << "项目结构");

  while (!reader.atEnd()) {
    if (reader.readNextStartElement()) {
      if (reader.name().toString() == "Project") {
        QString projectName = reader.attributes().value("name").toString();
        QStandardItem *rootItem = new QStandardItem(projectName);
        rootItem->setData("Project", Qt::UserRole);
        m_model->appendRow(rootItem);

        while (reader.readNextStartElement()) {
          loadItemFromXml(reader, rootItem);
        }
      }
    }
  }

  m_currentProjectPath = path;
  m_hasUnsavedChanges = false;
  file.close();
}

void ProjectManager::saveProject(const QString &path) {
  QFile file(path);
  if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
    return;
  }

  QXmlStreamWriter writer(&file);
  writer.setAutoFormatting(true);
  writer.writeStartDocument();

  if (m_model->rowCount() > 0) {
    QStandardItem *rootItem = m_model->item(0);
    writer.writeStartElement("Project");
    writer.writeAttribute("name", rootItem->text());

    for (int i = 0; i < rootItem->rowCount(); ++i) {
      saveItemToXml(writer, rootItem->child(i));
    }

    writer.writeEndElement(); // Project
  }

  writer.writeEndDocument();

  m_currentProjectPath = path;
  m_hasUnsavedChanges = false;
  file.close();
}

QStandardItemModel *ProjectManager::projectModel() { return m_model; }

void ProjectManager::renameProject(const QString &newName) {
  if (m_model->rowCount() > 0) {
    QStandardItem *rootItem = m_model->item(0);
    rootItem->setText(newName);
    m_hasUnsavedChanges = true;
  }
}

void ProjectManager::saveItemToXml(QXmlStreamWriter &writer,
                                   QStandardItem *item) {
  writer.writeStartElement("Component");
  writer.writeAttribute("name", item->text());
  writer.writeAttribute("type", item->data(Qt::UserRole).toString());

  // Save other properties if needed

  for (int i = 0; i < item->rowCount(); ++i) {
    saveItemToXml(writer, item->child(i));
  }

  writer.writeEndElement();
}

void ProjectManager::loadItemFromXml(QXmlStreamReader &reader,
                                     QStandardItem *parentItem) {
  if (reader.name().toString() == "Component") {
    QString name = reader.attributes().value("name").toString();
    QString type = reader.attributes().value("type").toString();

    QStandardItem *item = new QStandardItem(name);
    item->setData(type, Qt::UserRole);

    // Set icon based on type if needed
    if (type == "HostModule") {
      // item->setIcon(QIcon(":/icons/host.png"));
    } else if (type == "LoopModule") {
      // item->setIcon(QIcon(":/icons/loop.png"));
    }

    parentItem->appendRow(item);

    while (reader.readNextStartElement()) {
      loadItemFromXml(reader, item);
    }
  } else {
    reader.skipCurrentElement();
  }
}