#ifndef SEARCHLIST_HPP
#define SEARCHLIST_HPP

#include <QObject>
#include <QtWidgets>

QT_BEGIN_NAMESPACE
class QLineEdit;
class QTreeWidget;
QT_END_NAMESPACE

class searchlist : public QObject
{
    Q_OBJECT

public:
    explicit searchlist(QLineEdit *parent = nullptr);
    ~searchlist();

    void followMove(void);
    void followMinimized();
    void followRestore();
    void followClosed();

signals:

public slots:

private:
    QLineEdit *editor;
    QTreeWidget *searchEngineItem;
    QStringList searchEngines;
};

#endif // SEARCHLIST_HPP
