/********************************************************************************
** Form generated from reading UI file 'manual.ui'
**
** Created: Thu 12. Apr 23:36:08 2012
**      by: Qt User Interface Compiler version 4.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MANUAL_H
#define UI_MANUAL_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDial>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QGraphicsView>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QSpinBox>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_Manual
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *qhlHorizontal;
    QGroupBox *qgbPosition;
    QVBoxLayout *verticalLayout_2;
    QGraphicsView *qgvPosition;
    QGridLayout *gridLayout_2;
    QLabel *qlX;
    QLabel *qlY;
    QLabel *qlZ;
    QDoubleSpinBox *qdsbX;
    QDoubleSpinBox *qdsbY;
    QDoubleSpinBox *qdsbZ;
    QVBoxLayout *qhlVertical;
    QGroupBox *qgbHeading;
    QGridLayout *gridLayout;
    QLabel *qlAzimuth;
    QDial *qdAzimuth;
    QLabel *qlElevation;
    QDial *qdElevation;
    QSpinBox *qsbAzimuth;
    QSpinBox *qsbElevation;
    QSpacerItem *verticalSpacer;
    QGroupBox *qgbMeta;
    QGridLayout *gridLayout_3;
    QLineEdit *qleContext;
    QLabel *qlContext;
    QLabel *qlIdentity;
    QPushButton *qpbContext;
    QPushButton *qpbIdentity;
    QLineEdit *qleIdentity;
    QGroupBox *qgbState;
    QHBoxLayout *horizontalLayout;
    QPushButton *qpbLinked;
    QPushButton *qpbActivated;
    QPushButton *qpbUnhinge;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Manual)
    {
        if (Manual->objectName().isEmpty())
            Manual->setObjectName(QString::fromUtf8("Manual"));
        Manual->resize(631, 436);
        verticalLayout = new QVBoxLayout(Manual);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        qhlHorizontal = new QHBoxLayout();
        qhlHorizontal->setObjectName(QString::fromUtf8("qhlHorizontal"));
        qgbPosition = new QGroupBox(Manual);
        qgbPosition->setObjectName(QString::fromUtf8("qgbPosition"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(2);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qgbPosition->sizePolicy().hasHeightForWidth());
        qgbPosition->setSizePolicy(sizePolicy);
        verticalLayout_2 = new QVBoxLayout(qgbPosition);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        qgvPosition = new QGraphicsView(qgbPosition);
        qgvPosition->setObjectName(QString::fromUtf8("qgvPosition"));
        qgvPosition->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        qgvPosition->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        QBrush brush(QColor(0, 0, 0, 255));
        brush.setStyle(Qt::NoBrush);
        qgvPosition->setBackgroundBrush(brush);
        QBrush brush1(QColor(0, 0, 0, 255));
        brush1.setStyle(Qt::NoBrush);
        qgvPosition->setForegroundBrush(brush1);
        qgvPosition->setInteractive(true);
        qgvPosition->setSceneRect(QRectF(-50, -50, 100, 100));

        verticalLayout_2->addWidget(qgvPosition);

        gridLayout_2 = new QGridLayout();
        gridLayout_2->setObjectName(QString::fromUtf8("gridLayout_2"));
        qlX = new QLabel(qgbPosition);
        qlX->setObjectName(QString::fromUtf8("qlX"));
        qlX->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(qlX, 1, 0, 1, 1);

        qlY = new QLabel(qgbPosition);
        qlY->setObjectName(QString::fromUtf8("qlY"));
        qlY->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(qlY, 1, 1, 1, 1);

        qlZ = new QLabel(qgbPosition);
        qlZ->setObjectName(QString::fromUtf8("qlZ"));
        qlZ->setAlignment(Qt::AlignCenter);

        gridLayout_2->addWidget(qlZ, 1, 2, 1, 1);

        qdsbX = new QDoubleSpinBox(qgbPosition);
        qdsbX->setObjectName(QString::fromUtf8("qdsbX"));

        gridLayout_2->addWidget(qdsbX, 2, 0, 1, 1);

        qdsbY = new QDoubleSpinBox(qgbPosition);
        qdsbY->setObjectName(QString::fromUtf8("qdsbY"));

        gridLayout_2->addWidget(qdsbY, 2, 1, 1, 1);

        qdsbZ = new QDoubleSpinBox(qgbPosition);
        qdsbZ->setObjectName(QString::fromUtf8("qdsbZ"));

        gridLayout_2->addWidget(qdsbZ, 2, 2, 1, 1);


        verticalLayout_2->addLayout(gridLayout_2);


        qhlHorizontal->addWidget(qgbPosition);

        qhlVertical = new QVBoxLayout();
        qhlVertical->setObjectName(QString::fromUtf8("qhlVertical"));
        qgbHeading = new QGroupBox(Manual);
        qgbHeading->setObjectName(QString::fromUtf8("qgbHeading"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(qgbHeading->sizePolicy().hasHeightForWidth());
        qgbHeading->setSizePolicy(sizePolicy1);
        gridLayout = new QGridLayout(qgbHeading);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        qlAzimuth = new QLabel(qgbHeading);
        qlAzimuth->setObjectName(QString::fromUtf8("qlAzimuth"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Minimum);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(qlAzimuth->sizePolicy().hasHeightForWidth());
        qlAzimuth->setSizePolicy(sizePolicy2);
        qlAzimuth->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(qlAzimuth, 0, 0, 1, 1);

        qdAzimuth = new QDial(qgbHeading);
        qdAzimuth->setObjectName(QString::fromUtf8("qdAzimuth"));
        qdAzimuth->setMinimum(-180);
        qdAzimuth->setMaximum(180);
        qdAzimuth->setPageStep(30);
        qdAzimuth->setWrapping(true);
        qdAzimuth->setNotchTarget(6);
        qdAzimuth->setNotchesVisible(true);

        gridLayout->addWidget(qdAzimuth, 1, 0, 1, 1);

        qlElevation = new QLabel(qgbHeading);
        qlElevation->setObjectName(QString::fromUtf8("qlElevation"));
        sizePolicy2.setHeightForWidth(qlElevation->sizePolicy().hasHeightForWidth());
        qlElevation->setSizePolicy(sizePolicy2);
        qlElevation->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(qlElevation, 0, 1, 1, 1);

        qdElevation = new QDial(qgbHeading);
        qdElevation->setObjectName(QString::fromUtf8("qdElevation"));
        qdElevation->setMinimum(-180);
        qdElevation->setMaximum(180);
        qdElevation->setPageStep(30);
        qdElevation->setValue(90);
        qdElevation->setSliderPosition(90);
        qdElevation->setWrapping(true);
        qdElevation->setNotchTarget(6);
        qdElevation->setNotchesVisible(true);

        gridLayout->addWidget(qdElevation, 1, 1, 1, 1);

        qsbAzimuth = new QSpinBox(qgbHeading);
        qsbAzimuth->setObjectName(QString::fromUtf8("qsbAzimuth"));
        qsbAzimuth->setMaximum(360);

        gridLayout->addWidget(qsbAzimuth, 2, 0, 1, 1);

        qsbElevation = new QSpinBox(qgbHeading);
        qsbElevation->setObjectName(QString::fromUtf8("qsbElevation"));
        qsbElevation->setMinimum(-90);
        qsbElevation->setMaximum(90);

        gridLayout->addWidget(qsbElevation, 2, 1, 1, 1);


        qhlVertical->addWidget(qgbHeading);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        qhlVertical->addItem(verticalSpacer);

        qgbMeta = new QGroupBox(Manual);
        qgbMeta->setObjectName(QString::fromUtf8("qgbMeta"));
        gridLayout_3 = new QGridLayout(qgbMeta);
        gridLayout_3->setObjectName(QString::fromUtf8("gridLayout_3"));
        qleContext = new QLineEdit(qgbMeta);
        qleContext->setObjectName(QString::fromUtf8("qleContext"));

        gridLayout_3->addWidget(qleContext, 0, 1, 1, 1);

        qlContext = new QLabel(qgbMeta);
        qlContext->setObjectName(QString::fromUtf8("qlContext"));

        gridLayout_3->addWidget(qlContext, 0, 0, 1, 1);

        qlIdentity = new QLabel(qgbMeta);
        qlIdentity->setObjectName(QString::fromUtf8("qlIdentity"));

        gridLayout_3->addWidget(qlIdentity, 1, 0, 1, 1);

        qpbContext = new QPushButton(qgbMeta);
        qpbContext->setObjectName(QString::fromUtf8("qpbContext"));

        gridLayout_3->addWidget(qpbContext, 0, 2, 1, 1);

        qpbIdentity = new QPushButton(qgbMeta);
        qpbIdentity->setObjectName(QString::fromUtf8("qpbIdentity"));

        gridLayout_3->addWidget(qpbIdentity, 1, 2, 1, 1);

        qleIdentity = new QLineEdit(qgbMeta);
        qleIdentity->setObjectName(QString::fromUtf8("qleIdentity"));

        gridLayout_3->addWidget(qleIdentity, 1, 1, 1, 1);


        qhlVertical->addWidget(qgbMeta);

        qgbState = new QGroupBox(Manual);
        qgbState->setObjectName(QString::fromUtf8("qgbState"));
        sizePolicy2.setHeightForWidth(qgbState->sizePolicy().hasHeightForWidth());
        qgbState->setSizePolicy(sizePolicy2);
        horizontalLayout = new QHBoxLayout(qgbState);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        qpbLinked = new QPushButton(qgbState);
        qpbLinked->setObjectName(QString::fromUtf8("qpbLinked"));
        qpbLinked->setCheckable(true);
        qpbLinked->setChecked(true);

        horizontalLayout->addWidget(qpbLinked);

        qpbActivated = new QPushButton(qgbState);
        qpbActivated->setObjectName(QString::fromUtf8("qpbActivated"));
        qpbActivated->setCheckable(true);
        qpbActivated->setChecked(false);

        horizontalLayout->addWidget(qpbActivated);

        qpbUnhinge = new QPushButton(qgbState);
        qpbUnhinge->setObjectName(QString::fromUtf8("qpbUnhinge"));

        horizontalLayout->addWidget(qpbUnhinge);


        qhlVertical->addWidget(qgbState);


        qhlHorizontal->addLayout(qhlVertical);


        verticalLayout->addLayout(qhlHorizontal);

        buttonBox = new QDialogButtonBox(Manual);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Close|QDialogButtonBox::Reset);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(Manual);
        QObject::connect(buttonBox, SIGNAL(accepted()), Manual, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Manual, SLOT(reject()));

        QMetaObject::connectSlotsByName(Manual);
    } // setupUi

    void retranslateUi(QDialog *Manual)
    {
        Manual->setWindowTitle(QApplication::translate("Manual", "Manual Mumble Positional Audio", 0, QApplication::UnicodeUTF8));
        qgbPosition->setTitle(QApplication::translate("Manual", "Position", 0, QApplication::UnicodeUTF8));
        qlX->setText(QApplication::translate("Manual", "X", 0, QApplication::UnicodeUTF8));
        qlY->setText(QApplication::translate("Manual", "Y", 0, QApplication::UnicodeUTF8));
        qlZ->setText(QApplication::translate("Manual", "Z", 0, QApplication::UnicodeUTF8));
        qdsbX->setPrefix(QString());
        qdsbX->setSuffix(QApplication::translate("Manual", "m", 0, QApplication::UnicodeUTF8));
        qdsbY->setSuffix(QApplication::translate("Manual", "m", 0, QApplication::UnicodeUTF8));
        qdsbZ->setSuffix(QApplication::translate("Manual", "m", 0, QApplication::UnicodeUTF8));
        qgbHeading->setTitle(QApplication::translate("Manual", "Heading", 0, QApplication::UnicodeUTF8));
        qlAzimuth->setText(QApplication::translate("Manual", "Azimuth", 0, QApplication::UnicodeUTF8));
        qlElevation->setText(QApplication::translate("Manual", "Elevation", 0, QApplication::UnicodeUTF8));
        qsbAzimuth->setSuffix(QApplication::translate("Manual", "\302\260", 0, QApplication::UnicodeUTF8));
        qsbElevation->setSuffix(QApplication::translate("Manual", "\302\260", 0, QApplication::UnicodeUTF8));
        qgbMeta->setTitle(QApplication::translate("Manual", "Meta data", 0, QApplication::UnicodeUTF8));
        qlContext->setText(QApplication::translate("Manual", "Context", 0, QApplication::UnicodeUTF8));
        qlIdentity->setText(QApplication::translate("Manual", "Identity", 0, QApplication::UnicodeUTF8));
        qpbContext->setText(QApplication::translate("Manual", "Set", 0, QApplication::UnicodeUTF8));
        qpbIdentity->setText(QApplication::translate("Manual", "Set", 0, QApplication::UnicodeUTF8));
        qgbState->setTitle(QApplication::translate("Manual", "State", 0, QApplication::UnicodeUTF8));
        qpbLinked->setText(QApplication::translate("Manual", "Linked", 0, QApplication::UnicodeUTF8));
        qpbActivated->setText(QApplication::translate("Manual", "Activated", 0, QApplication::UnicodeUTF8));
        qpbUnhinge->setText(QApplication::translate("Manual", "Unhinge", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class Manual: public Ui_Manual {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MANUAL_H
