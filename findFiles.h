#ifndef FINDFILES_H
#define FINDFILES_H

#include <QtGui>
#include <QWidget>
#include <QtGui/qdialog.h>
#include <QDir>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QStringList;
QT_END_NAMESPACE

//! [0]
class FindFiles : public QDialog
{
    Q_OBJECT
public:
    FindFiles(QWidget *parent = 0);

    void init(QStringList l){showFiles(l);}
public slots:

private slots:
    void browse();
    void find();
    void openFileOfItem(int row, int column);


private:
    QStringList findFilesList(const QStringList &files, const QString &text);
    QStringList findFilesList(const QFileInfoList &filesInfo, const QString &text);
    QFileInfoList returnListForAllDirs(QDir directory);
    void showFiles(const QStringList &files);
    QPushButton *createButton(const QString &text, const char *member);
    QPushButton *createButton(const QString &text);
    QComboBox *createComboBox(const QString &text = QString());
    void createFilesTable();

    QComboBox *regexpComboBox;
    QComboBox *directoryComboBox;
    QLabel *fileLabel;
    QLabel *textLabel;
    QLabel *directoryLabel;
    QLabel *filesFoundLabel;
    QPushButton *browseButton;
    QPushButton *findButton;
    QTableWidget *filesTable;
    QDir currentDir;
signals:
    void signalFiles(QStringList,QStringList);
};
//! [0]
#endif // FINDFILES_H
