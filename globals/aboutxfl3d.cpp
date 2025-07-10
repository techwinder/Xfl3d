/****************************************************************************

    flow5 application
    Copyright (C) Andre Deperrois
    All rights reserved.

*****************************************************************************/

#include <QDialogButtonBox>
#include <QBitmap>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>


#include "aboutxfl3d.h"

#include <xflcore/xflcore.h>

AboutXfl3d::AboutXfl3d(QWidget *pParent) : QDialog(pParent)
{
    setWindowTitle("About flow5");
    setupLayout();
}


void AboutXfl3d::setupLayout()
{
    QVBoxLayout *pLogoLayout = new QVBoxLayout;
    {
        QLabel *pLabIconF5 = new QLabel;
        pLabIconF5->setObjectName("flow5");
        QPixmap xflpix = QPixmap(QString::fromUtf8(":/icons/Xfl3d.png")).scaledToWidth(128);
        pLabIconF5->setPixmap(xflpix);
        pLabIconF5->setAlignment(Qt::AlignCenter);
        QLabel *pLab1  = new QLabel(xfl::versionName(true));
        pLab1->setAlignment(Qt::AlignVCenter| Qt::AlignLeft);

        pLogoLayout->addWidget(pLabIconF5);
        pLogoLayout->addWidget(pLab1);
    }

    QString copyright = QString::fromUtf8("Copyright © André Deperrois - GPL v3");
    QLabel *pLab4  = new QLabel(copyright);
    QLabel *pLab6  = new QLabel(  "This program is distributed in the hope that it will be useful\n"
                                  "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
                                  "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.");

    QDialogButtonBox *pButtonBox = new QDialogButtonBox(QDialogButtonBox::Close);
    {
        connect(pButtonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    }

    QVBoxLayout *pMainLayout = new QVBoxLayout;
    {
        pMainLayout->addLayout(pLogoLayout);

        pMainLayout->addWidget(pLab4);
        pMainLayout->addStretch();
        pMainLayout->addSpacing(17);
        pMainLayout->addWidget(pLab6);
        pMainLayout->addSpacing(17);
        pMainLayout->addStretch(1);
        pMainLayout->addSpacing(17);
        pMainLayout->addWidget(pButtonBox);
    }
    setLayout(pMainLayout);
}
