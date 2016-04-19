#ifndef HELPBROWSER_H
#define HELPBROWSER_H

#include <QWidget>

namespace Ui {
class HelpBrowser;
}

// ********************* class HelpBrowser ********************
/**
 * \class HelpBrowser
 * \brief Класс для работы со справкой
 */
class HelpBrowser : public QWidget
{
    Q_OBJECT
    
public:
    explicit HelpBrowser(const QString& strPath,
                         const QString& strFileName,
                         QWidget *parent = 0);
    ~HelpBrowser();
    
private:
    Ui::HelpBrowser *ui;
};

#endif // HELPBROWSER_H
