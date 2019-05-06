#ifndef SEARCHBOX_HPP
#define SEARCHBOX_HPP

#include <QLineEdit>
#include <QEvent>
#include <QSystemTrayIcon>

class searchlist;

class SearchBox: public QLineEdit
{
    Q_OBJECT

public:
    SearchBox(QWidget *parent = 0);
    ~SearchBox();

protected:
    void moveEvent(QMoveEvent *event);

    void closeEvent(QCloseEvent *event);
    void changeEvent(QEvent *event);

    void keyPressEvent(QKeyEvent* k);

protected slots:
    void doSearch();
    void bothShowNormal();

private:

    void createActions();
    void createTrayIcon();

    searchlist *searchEntry;

    QAction *showPreferenceAction;
    QAction *showSearchBoxAction;
    QAction *showCustomSearchEnginesAction;
    QAction *quitAction;

    QSystemTrayIcon *trayIcon;
    QMenu *trayIconMenu;
};

#endif // SEARCHBOX_HPP
