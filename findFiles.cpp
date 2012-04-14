/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/



#include "findFiles.h"

//! [0]
FindFiles::FindFiles(QWidget *parent)
    : QDialog(parent)
{
    browseButton = createButton(tr("&Browse..."), SLOT(browse()));
    findButton = createButton(tr("&Find"), SLOT(find()));
    regexpComboBox = createComboBox(tr(".*.jpg"));
    directoryComboBox = createComboBox(QDir::currentPath());

    fileLabel = new QLabel(tr("Named:"));
    textLabel = new QLabel(tr("Containing text:"));
    directoryLabel = new QLabel(tr("In directory:"));
    filesFoundLabel = new QLabel;
    createFilesTable();


    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setSizeConstraint(QLayout::SetNoConstraint);
    mainLayout->addWidget(fileLabel, 0, 0);
    mainLayout->addWidget(regexpComboBox, 0, 1, 1, 2);
    mainLayout->addWidget(textLabel, 1, 0);
    mainLayout->addWidget(directoryLabel, 2, 0);
    mainLayout->addWidget(directoryComboBox, 2, 1);
    mainLayout->addWidget(browseButton, 2, 2);
    mainLayout->addWidget(filesTable, 3, 0, 1, 3);
    mainLayout->addWidget(filesFoundLabel, 4, 0, 1, 2);
    mainLayout->addWidget(findButton, 2, 3);
    setLayout(mainLayout);
    setWindowTitle(tr("Find Files"));

}
void FindFiles::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                               tr("Find Files"), QDir::currentPath());

    if (!directory.isEmpty()) {
        if (directoryComboBox->findText(directory) == -1)
            directoryComboBox->addItem(directory);
        directoryComboBox->setCurrentIndex(directoryComboBox->findText(directory));
    }
}


static void updateComboBox(QComboBox *comboBox)
{
    if (comboBox->findText(comboBox->currentText()) == -1)
        comboBox->addItem(comboBox->currentText());
}

void FindFiles::find()
{
    filesTable->setRowCount(0);

    QString fileName = regexpComboBox->currentText();
    QString path = directoryComboBox->currentText();

    updateComboBox(regexpComboBox);
    updateComboBox(directoryComboBox);

    currentDir = QDir(path);
    if (fileName.isEmpty())
        fileName = ".*";
    QStringList files;
    QStringList filePaths;

    QDirIterator subdirectory_finder(currentDir.path(), QDir::Dirs | QDir::NoSymLinks, QDirIterator::Subdirectories);

    int numberOfDirs = 0;
    while(subdirectory_finder.hasNext())
    {
        subdirectory_finder.next();
        numberOfDirs++;
    }

    QDirIterator directory_walker(currentDir.path(), QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, numberOfDirs);
    progressDialog.setWindowTitle(tr("Find Files"));
    int i=0;
    while(directory_walker.hasNext())
    {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Searching file number %1 in %2 directory").arg(i++).arg(numberOfDirs));
        qApp->processEvents();
        if (progressDialog.wasCanceled())
            break;
        QString actualFileName = directory_walker.fileInfo().completeBaseName()+"."+directory_walker.fileInfo().completeSuffix();
        QString regularFileName = "";
        QRegExp regularExp("("+fileName+")");
        int pos = regularExp.indexIn(actualFileName);
        if (pos > -1){
            regularFileName = regularExp.cap(0);
        }

        if(regularFileName == actualFileName){
            filePaths.append(directory_walker.fileInfo().absoluteFilePath());
            files.append(directory_walker.fileName());

        }
        directory_walker.next();

    }

    emit signalFiles(files,filePaths);
    showFiles(files);
}

void FindFiles::showFiles(const QStringList &files)
{
    QProgressDialog progressDialog(this);
    progressDialog.setCancelButtonText(tr("&Cancel"));
    progressDialog.setRange(0, files.size());
    progressDialog.setWindowTitle(tr("Show Table"));

    for (int i = 0; i < files.size(); ++i) {
        progressDialog.setValue(i);
        progressDialog.setLabelText(tr("Setting table row %1 of %2...\nYou can skip this and whole list of files will be exported").arg(i).arg(files.size()));
        qApp->processEvents();
        if (progressDialog.wasCanceled())
            break;
        QFile file(currentDir.absoluteFilePath(files[i]));
        qint64 size = QFileInfo(file).size();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        fileNameItem->setFlags(fileNameItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *filePathItem = new QTableWidgetItem(QDir().absoluteFilePath(files[i]));
        filePathItem->setFlags(filePathItem->flags() ^ Qt::ItemIsEditable);
        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
                                             .arg(int((size + 1023) / 1024)));
        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);

        int row = filesTable->rowCount();
        filesTable->insertRow(row);
        filesTable->setItem(row, 0, fileNameItem);
        filesTable->setItem(row, 1, filePathItem);
        filesTable->setItem(row, 2, sizeItem);
    }
    filesTable->horizontalHeader()->resizeSections(QHeaderView::ResizeToContents);

    filesTable->horizontalHeader()->resizeSection(1, filesTable->width()-20-filesTable->horizontalHeader()->sectionSize(0)-filesTable->horizontalHeader()->sectionSize(2));
    //filesTable->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    filesFoundLabel->setText(tr("%1 file(s) found").arg(files.size())+(" (Double click on a file to open it)"));

    filesFoundLabel->setWordWrap(true);
}

QPushButton *FindFiles::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

QComboBox *FindFiles::createComboBox(const QString &text)
{
    QComboBox *comboBox = new QComboBox;
    comboBox->setEditable(true);
    comboBox->addItem(text);
    comboBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    return comboBox;
}

void FindFiles::createFilesTable()
{
    filesTable = new QTableWidget(0, 3);
    filesTable->setSelectionBehavior(QAbstractItemView::SelectRows);

    QStringList labels;
    labels << tr("Filename") << tr("Filepath") << tr("Size");
    filesTable->setHorizontalHeaderLabels(labels);
    filesTable->horizontalHeader()->resizeSections(QHeaderView::Stretch);
    filesTable->horizontalHeader()->setResizeMode(QHeaderView::Interactive);
    filesTable->verticalHeader()->hide();
    filesTable->setShowGrid(false);

    connect(filesTable, SIGNAL(cellActivated(int,int)),
            this, SLOT(openFileOfItem(int,int)));
}

void FindFiles::openFileOfItem(int row, int /* column */)
{
    QTableWidgetItem *item = filesTable->item(row, 0);

    QDesktopServices::openUrl(QUrl::fromLocalFile(currentDir.absoluteFilePath(item->text())));
}


