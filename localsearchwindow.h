#ifndef LOCAL_SEARCH_WINDOW_H
#define LOCAL_SEARCH_WINDOW_H

#include <QWidget>
#include <QDir>
#include <QDialog>
#include <QCloseEvent>

QT_BEGIN_NAMESPACE
class QComboBox;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
QT_END_NAMESPACE

class LocalSearchWindow : public QDialog
{
    Q_OBJECT

public:
    LocalSearchWindow(QWidget *parent = 0);

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void browse();
    void find();
    void animateFindClick();
    void openFileOfItem(int row, int column);
    void contextMenu(const QPoint &pos);

    void getContent(const QString& content);

private:
    QStringList findFiles(const QStringList &files, const QString &text);
    void showFiles(const QStringList &files);
    QComboBox *createComboBox(const QString &text = QString());
    void createFilesTable();

    void resetFindStatus();

    QString searchContent;

    QComboBox *fileComboBox;
    QComboBox *textComboBox;
    QComboBox *directoryComboBox;
    QLabel *filesFoundLabel;
    QPushButton *findButton;
    QTableWidget *filesTable;

    QDir currentDir;
};

#endif
