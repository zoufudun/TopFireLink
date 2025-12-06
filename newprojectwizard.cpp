#include "newprojectwizard.h"
#include <QDir>
#include <QFileDialog>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QStandardPaths>
#include <QVBoxLayout>


NewProjectWizard::NewProjectWizard(QWidget *parent) : QWizard(parent) {
  setWindowTitle(tr("新建项目向导"));
  setWizardStyle(QWizard::ModernStyle);

  addIntroPage();
  addProjectInfoPage();
}

void NewProjectWizard::addIntroPage() {
  QWizardPage *page = new QWizardPage;
  page->setTitle(tr("欢迎"));
  page->setSubTitle(tr("欢迎使用新建项目向导。此向导将帮助您创建一个新的 "
                       "FireControllerIDE 项目。"));

  QVBoxLayout *layout = new QVBoxLayout;
  QLabel *label = new QLabel(tr("点击“下一步”继续。"));
  layout->addWidget(label);
  page->setLayout(layout);

  addPage(page);
}

void NewProjectWizard::addProjectInfoPage() {
  addPage(new ProjectInfoPage(this));
}

QString NewProjectWizard::projectName() const {
  return field("projectName").toString();
}

QString NewProjectWizard::projectPath() const {
  return field("projectPath").toString();
}

bool NewProjectWizard::isImport() const { return field("isImport").toBool(); }

QString NewProjectWizard::importPath() const {
  return field("importPath").toString();
}

// ProjectInfoPage implementation

ProjectInfoPage::ProjectInfoPage(QWidget *parent) : QWizardPage(parent) {
  setTitle(tr("项目信息"));
  setSubTitle(tr("请指定项目名称和位置。"));

  projectNameLineEdit = new QLineEdit;
  projectPathLineEdit = new QLineEdit;
  importCheckBox = new QCheckBox(tr("导入现有项目"));
  importPathLineEdit = new QLineEdit;
  importPathLineEdit->setEnabled(false);
  browseImportBtn = new QPushButton(tr("浏览..."));
  browseImportBtn->setEnabled(false);

  QPushButton *browseDirBtn = new QPushButton(tr("浏览..."));

  registerField("projectName*", projectNameLineEdit);
  registerField("projectPath", projectPathLineEdit);
  registerField("isImport", importCheckBox);
  registerField("importPath", importPathLineEdit);

  QFormLayout *layout = new QFormLayout;
  layout->addRow(tr("项目名称:"), projectNameLineEdit);

  QHBoxLayout *pathLayout = new QHBoxLayout;
  pathLayout->addWidget(projectPathLineEdit);
  pathLayout->addWidget(browseDirBtn);
  layout->addRow(tr("项目位置:"), pathLayout);

  layout->addRow(importCheckBox);

  QHBoxLayout *importLayout = new QHBoxLayout;
  importLayout->addWidget(importPathLineEdit);
  importLayout->addWidget(browseImportBtn);
  layout->addRow(tr("导入路径:"), importLayout);

  setLayout(layout);

  connect(browseDirBtn, &QPushButton::clicked, this,
          &ProjectInfoPage::browseDir);
  connect(importCheckBox, &QCheckBox::toggled, this, [this](bool checked) {
    importPathLineEdit->setEnabled(checked);
    browseImportBtn->setEnabled(checked);
  });
  connect(browseImportBtn, &QPushButton::clicked, this,
          &ProjectInfoPage::browseImport);
}

void ProjectInfoPage::initializePage() {
  QString defaultPath =
      QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
  projectPathLineEdit->setText(defaultPath);
  projectNameLineEdit->setText("NewProject");
}

void ProjectInfoPage::browseDir() {
  QString dir = QFileDialog::getExistingDirectory(this, tr("选择项目位置"),
                                                  projectPathLineEdit->text());
  if (!dir.isEmpty()) {
    projectPathLineEdit->setText(dir);
  }
}

void ProjectInfoPage::browseImport() {
  QString file = QFileDialog::getOpenFileName(this, tr("选择要导入的项目文件"),
                                              projectPathLineEdit->text(),
                                              tr("XML 项目文件 (*.xml)"));
  if (!file.isEmpty()) {
    importPathLineEdit->setText(file);
  }
}

bool ProjectInfoPage::validatePage() {
  if (importCheckBox->isChecked() && importPathLineEdit->text().isEmpty()) {
    return false;
  }
  return true;
}
