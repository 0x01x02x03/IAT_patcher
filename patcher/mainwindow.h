#pragma once

#include <QtGui>
#include <QFile>
#include <bearparser.h>

#include "Executables.h"

#include "ui_mainwindow.h"
#include "ui_replacements.h"

#include "ImportsTableModel.h"
#include "InfoTableModel.h"
#include "ExeController.h"

class ThreadSaveCounter : public QObject
{
    Q_OBJECT

signals:
    void counterChanged();

public:
    ThreadSaveCounter() { counter = 0; }

    void inc()
    {
        {
        QMutexLocker lock(&m_Mutex);
        counter++;
        }
        emit counterChanged();
    }

    void dec()
    {
        {
        QMutexLocker lock(&m_Mutex);
        counter--;
        }
        emit counterChanged();
    }
    size_t getCount()
    {
        QMutexLocker lock(&m_Mutex);
        return counter;
    }

protected:
    size_t counter;
    QMutex m_Mutex;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

signals:
    void exeSelected(ExeHandler* exe);
    void exeUpdated(ExeHandler* exe);
    void hookRequested(ExeHandler* exe);

    void thunkSelected(offset_t thunk);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
	/* events */
	void dragEnterEvent(QDragEnterEvent* ev) { ev->accept(); }
    void dropEvent(QDropEvent* ev);


private slots:
    void filterLibs(const QString &str);
    void filterFuncs(const QString &str);

    void takeAction();
    void loadingStatusChanged();
    void onFileLoaded(AbstractByteBuffer* buf);

    void selectExe(ExeHandler* exe) 
    { 
        m_ExeSelected = exe;
        QString fName = "";
        if (exe && exe->getExe())
            fName = exe->getExe()->getFileName();
        this->m_ui.fileEdit->setText(fName);
        emit exeSelected(exe); 
    }

    void customMenuRequested(QPoint pos);
    void functionsMenuRequested(QPoint pos);
    void onHookRequested(ExeHandler* exeHndl);
    void setReplacement();
    void setThunkSelected(offset_t thunk);

    void onLoaderThreadFinished();
    void rowChangedSlot(QModelIndex, QModelIndex);
    void openExe();
    //---
    void on_pushButton_clicked();
    void on_reloadButton_clicked();
    void on_clearAllButton_clicked() { clear(); }
    void on_hookButton_clicked();
    void on_saveButton_clicked();

	void on_actionAbout_triggered() {  info(); }

private:
    void reloadExe(ExeHandler* exe);
    void removeExe(ExeHandler* exe);
	void info();
    void clear();
    bool parse(QString &fileName);

    void addExeAction(QMenu *customMenu, QString text, ExeController::EXE_ACTION a);
    void makeCustomMenu();
    void makeFunctionsMenu();
    void makeFileMenu();
    void initReplacementsDialog();

    QDialog *m_replacementsDialog;
    QMenu *customMenu, *functionsMenu;

    QSortFilterProxyModel *m2;

    Ui::MainWindow m_ui;
    Ui_Replacements* m_uiReplacements;

    ImportsTableModel *impModel;
    InfoTableModel *infoModel;
    ThreadSaveCounter m_LoadersCount;

    Executables m_exes;
    ExeHandler* m_ExeSelected;
    offset_t m_ThunkSelected;
    QLabel urlLabel;

    ExeController exeController;
};

