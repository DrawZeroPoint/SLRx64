/********************************************************************************
** Form generated from reading UI file 'focusassistant.ui'
**
** Created by: Qt User Interface Compiler version 5.3.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FOCUSASSISTANT_H
#define UI_FOCUSASSISTANT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_FocusAssistant
{
public:
    QGridLayout *gridLayout_2;
    QLabel *imageDisplay;
    QHBoxLayout *horizontalLayout_2;
    QHBoxLayout *horizontalLayout;
    QGroupBox *selectCamera;
    QGridLayout *gridLayout;
    QRadioButton *leftCamera;
    QRadioButton *rightCamera;
    QPushButton *okButton;
    QSpacerItem *horizontalSpacer;

    void setupUi(QWidget *FocusAssistant)
    {
        if (FocusAssistant->objectName().isEmpty())
            FocusAssistant->setObjectName(QStringLiteral("FocusAssistant"));
        FocusAssistant->resize(835, 592);
        QIcon icon;
        icon.addFile(QStringLiteral(":/splash.png"), QSize(), QIcon::Normal, QIcon::Off);
        FocusAssistant->setWindowIcon(icon);
        gridLayout_2 = new QGridLayout(FocusAssistant);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        imageDisplay = new QLabel(FocusAssistant);
        imageDisplay->setObjectName(QStringLiteral("imageDisplay"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(imageDisplay->sizePolicy().hasHeightForWidth());
        imageDisplay->setSizePolicy(sizePolicy);
        imageDisplay->setMaximumSize(QSize(1024, 800));
        imageDisplay->setScaledContents(true);

        gridLayout_2->addWidget(imageDisplay, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        selectCamera = new QGroupBox(FocusAssistant);
        selectCamera->setObjectName(QStringLiteral("selectCamera"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(selectCamera->sizePolicy().hasHeightForWidth());
        selectCamera->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(selectCamera);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        leftCamera = new QRadioButton(selectCamera);
        leftCamera->setObjectName(QStringLiteral("leftCamera"));
        sizePolicy1.setHeightForWidth(leftCamera->sizePolicy().hasHeightForWidth());
        leftCamera->setSizePolicy(sizePolicy1);
        leftCamera->setChecked(true);

        gridLayout->addWidget(leftCamera, 0, 0, 1, 1);

        rightCamera = new QRadioButton(selectCamera);
        rightCamera->setObjectName(QStringLiteral("rightCamera"));
        sizePolicy1.setHeightForWidth(rightCamera->sizePolicy().hasHeightForWidth());
        rightCamera->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(rightCamera, 0, 1, 1, 1);


        horizontalLayout->addWidget(selectCamera);

        okButton = new QPushButton(FocusAssistant);
        okButton->setObjectName(QStringLiteral("okButton"));
        QSizePolicy sizePolicy2(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(okButton->sizePolicy().hasHeightForWidth());
        okButton->setSizePolicy(sizePolicy2);

        horizontalLayout->addWidget(okButton);


        horizontalLayout_2->addLayout(horizontalLayout);

        horizontalSpacer = new QSpacerItem(402, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        gridLayout_2->addLayout(horizontalLayout_2, 1, 0, 1, 1);


        retranslateUi(FocusAssistant);

        QMetaObject::connectSlotsByName(FocusAssistant);
    } // setupUi

    void retranslateUi(QWidget *FocusAssistant)
    {
        FocusAssistant->setWindowTitle(QApplication::translate("FocusAssistant", "Focus Assistant", 0));
        imageDisplay->setText(QApplication::translate("FocusAssistant", "Live View", 0));
        selectCamera->setTitle(QApplication::translate("FocusAssistant", "Select Camera", 0));
        leftCamera->setText(QApplication::translate("FocusAssistant", "Left Camera", 0));
        rightCamera->setText(QApplication::translate("FocusAssistant", "Right Camera", 0));
        okButton->setText(QApplication::translate("FocusAssistant", "Confirm", 0));
    } // retranslateUi

};

namespace Ui {
    class FocusAssistant: public Ui_FocusAssistant {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FOCUSASSISTANT_H
