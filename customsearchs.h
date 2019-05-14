#ifndef CUSTOMSEARCHS_H
#define CUSTOMSEARCHS_H

#include <QDialog>
#include <vector>
#include <array>
#include <QCloseEvent>
#include <QTimer>

namespace Ui {
class CustomSearchs;
}

struct searchEngine
{
    QString title;
    QString url;
    std::array<QChar, 3> hotKey;
};

class CustomSearchs : public QDialog
{
    Q_OBJECT

public:
    explicit CustomSearchs(QWidget *parent = 0);
    ~CustomSearchs();

    const std::vector<std::pair<searchEngine, bool> >& getSearchEngineItems() { return searchEngines; }
    QString getSearchEngineURL(const QString& title);

signals:
    void updateSearchEngines();

protected:
    void closeEvent(QCloseEvent *event);

protected slots:
    bool rewriteSearchEngineConfig();

private slots:
    void on_searchEngineItems_clicked(const QModelIndex &index);

    void on_deleteItemButton_clicked();

    void on_addItemButton_clicked();

    void on_turnUpButton_clicked();

    void on_turnDownButton_clicked();

    void on_titleLineEdit_editingFinished();

    void on_urlLineEdit_editingFinished();

    void on_firstHotKey_keySequenceChanged(const QKeySequence &keySequence);

    void on_secondHotKey_keySequenceChanged(const QKeySequence &keySequence);

    void on_thirdHotKey_keySequenceChanged(const QKeySequence &keySequence);

    void on_enableButton_clicked();

private:
    Ui::CustomSearchs *ui;

    std::vector<std::pair<searchEngine, bool> > searchEngines;

    bool isNeedStore;
    QTimer *storeConfigTimer;

    bool loadJsonFile(QString filePath, QJsonDocument &jsonDoc);
};

#endif // CUSTOMSEARCHS_H
