#include "HelpBrowser.h"
#include "ui_HelpBrowser.h"
#include <QFile>

HelpBrowser::HelpBrowser(const QString &strPath, const QString &strFileName, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HelpBrowser)
{
    ui->setupUi(this);

    connect(ui->_pushButtonBack, SIGNAL(clicked()),
            ui->_textBrowser, SLOT(backward())
            );
    connect(ui->_pushButtonHome, SIGNAL(clicked()),
            ui->_textBrowser, SLOT(home())
            );
    connect(ui->_pushButtonForward, SIGNAL(clicked()),
            ui->_textBrowser, SLOT(forward())
            );
    connect(ui->_textBrowser, SIGNAL(backwardAvailable(bool)),
            ui->_pushButtonBack, SLOT(setEnabled(bool))
            );
    connect(ui->_textBrowser, SIGNAL(forwardAvailable(bool)),
            ui->_pushButtonForward, SLOT(setEnabled(bool))
            );

    ui->_textBrowser->setSearchPaths(QStringList() << strPath);
    ui->_textBrowser->setSource(QString(strFileName));
}

HelpBrowser::~HelpBrowser()
{
    delete ui;
}
