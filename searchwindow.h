#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QMainWindow>
#include <QEvent>
#include <QSystemTrayIcon>
#include <QHotkey>
#include <map>

#include "customsearchs.h"
#include "localsearchwindow.h"

namespace Ui {
class searchWindow;
}

class searchWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit searchWindow(QWidget *parent = 0);
    ~searchWindow();

    bool eventFilter(QObject *obj, QEvent *ev) override;

protected:
    void keyPressEvent(QKeyEvent *k) override;

signals:
    void triggerLocalSearch(const QString&);

protected slots:
    void selectSearchEngine();
    void addSearchEngines();

private slots:

    void on_searchEngineList_doubleClicked(const QModelIndex &index);

private:
    Ui::searchWindow *ui;

    void doSearch(int id);
    void resetSearchStatus();

    void createActions();
    void createTrayIcon();

    CustomSearchs *customSearchs;
    LocalSearchWindow *localSearchWindow;

    QAction *showPreferenceAction;
    QAction *showSearchBoxAction;
    QAction *showCustomSearchEnginesAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;

    QHotkey *showSearchWindow;

    std::map<QString, std::pair<int, QString> > selectedEngines;
};

#endif // SEARCHWINDOW_H
