#ifndef NEWPROJECTWIZARD_H
#define NEWPROJECTWIZARD_H

#include <QCheckBox>
#include <QLineEdit>
#include <QWizard>


class NewProjectWizard : public QWizard {
  Q_OBJECT

public:
  explicit NewProjectWizard(QWidget *parent = nullptr);

  QString projectName() const;
  QString projectPath() const;
  bool isImport() const;
  QString importPath() const;

private:
  void addIntroPage();
  void addProjectInfoPage();
};

class ProjectInfoPage : public QWizardPage {
  Q_OBJECT

public:
  ProjectInfoPage(QWidget *parent = nullptr);

  void initializePage() override;
  bool validatePage() override;

private slots:
  void browseDir();
  void browseImport();

private:
  QLineEdit *projectNameLineEdit;
  QLineEdit *projectPathLineEdit;
  QCheckBox *importCheckBox;
  QLineEdit *importPathLineEdit;
  QPushButton *browseImportBtn;
};

#endif // NEWPROJECTWIZARD_H
