/********************************************************************************
** Form generated from reading UI file 'monitorform.ui'
**
** Created by: Qt User Interface Compiler version 5.9.5
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MONITORFORM_H
#define UI_MONITORFORM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>

QT_BEGIN_NAMESPACE

class Ui_MonitorForm
{
public:
    QPushButton *btnReset;
    QGroupBox *groupBox;
    QLabel *label;
    QLineEdit *txtWriteCmdCount;
    QLabel *label1;
    QLabel *label2;
    QLineEdit *txtWriteSpeed;
    QLineEdit *txtReadSpeed;
    QLineEdit *txtWritePageCount;
    QLineEdit *txtReadPageCount;
    QLabel *label3;
    QLabel *label4;
    QLineEdit *txtReadCmdCount;
    QGroupBox *groupBox1;
    QLineEdit *txtGCCount;
    QLineEdit *txtWriteAmp;
    QLineEdit *txtGCErase;
    QLabel *label5;
    QLabel *label6;
    QLabel *label7;
    QGroupBox *groupBox2;
    QLabel *label8;
    QLineEdit *txtTrimCount;
    QLineEdit *txtTrimEffect;
    QLabel *label9;
    QLineEdit *txtSSDUtil;
    QLabel *label10;
    QTextEdit *txtDebug;
    QLabel *label11;
    QLineEdit *txtTime;
    QLabel *label12;
    QPushButton *btnSave;

    void setupUi(QDialog *MonitorForm)
    {
        if (MonitorForm->objectName().isEmpty())
            MonitorForm->setObjectName(QStringLiteral("MonitorForm"));
        MonitorForm->resize(600, 500);
        MonitorForm->setProperty("name", QVariant(QByteArray("Form1")));
        btnReset = new QPushButton(MonitorForm);
        btnReset->setObjectName(QStringLiteral("btnReset"));
        btnReset->setGeometry(QRect(490, 20, 100, 50));
        btnReset->setAutoDefault(true);
        btnReset->setProperty("name", QVariant(QByteArray("btnReset")));
        groupBox = new QGroupBox(MonitorForm);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        groupBox->setGeometry(QRect(21, 20, 461, 91));
        groupBox->setStyleSheet(QLatin1String("#groupBox {\n"
"	border : 1px solid gray;\n"
"	border-radius : 9px;\n"
"}"));
        groupBox->setProperty("name", QVariant(QByteArray("groupBox1")));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));
        label->setGeometry(QRect(10, 30, 56, 20));
        label->setProperty("name", QVariant(QByteArray("txtLabelWrite")));
        txtWriteCmdCount = new QLineEdit(groupBox);
        txtWriteCmdCount->setObjectName(QStringLiteral("txtWriteCmdCount"));
        txtWriteCmdCount->setGeometry(QRect(110, 30, 89, 22));
        txtWriteCmdCount->setAlignment(Qt::AlignRight);
        txtWriteCmdCount->setProperty("name", QVariant(QByteArray("txtWriteCmdCount")));
        label1 = new QLabel(groupBox);
        label1->setObjectName(QStringLiteral("label1"));
        label1->setGeometry(QRect(130, 10, 60, 20));
        label1->setProperty("name", QVariant(QByteArray("txtLabelCount")));
        label2 = new QLabel(groupBox);
        label2->setObjectName(QStringLiteral("label2"));
        label2->setGeometry(QRect(250, 10, 91, 20));
        label2->setProperty("name", QVariant(QByteArray("txtLabelSpeed")));
        txtWriteSpeed = new QLineEdit(groupBox);
        txtWriteSpeed->setObjectName(QStringLiteral("txtWriteSpeed"));
        txtWriteSpeed->setGeometry(QRect(240, 30, 89, 22));
        txtWriteSpeed->setAlignment(Qt::AlignRight);
        txtWriteSpeed->setProperty("name", QVariant(QByteArray("txtWriteSpeed")));
        txtReadSpeed = new QLineEdit(groupBox);
        txtReadSpeed->setObjectName(QStringLiteral("txtReadSpeed"));
        txtReadSpeed->setGeometry(QRect(240, 60, 89, 22));
        txtReadSpeed->setAlignment(Qt::AlignRight);
        txtReadSpeed->setProperty("name", QVariant(QByteArray("txtReadSpeed")));
        txtWritePageCount = new QLineEdit(groupBox);
        txtWritePageCount->setObjectName(QStringLiteral("txtWritePageCount"));
        txtWritePageCount->setGeometry(QRect(360, 30, 89, 22));
        txtWritePageCount->setAlignment(Qt::AlignRight);
        txtWritePageCount->setProperty("name", QVariant(QByteArray("txtWritePageCount")));
        txtReadPageCount = new QLineEdit(groupBox);
        txtReadPageCount->setObjectName(QStringLiteral("txtReadPageCount"));
        txtReadPageCount->setGeometry(QRect(360, 60, 89, 22));
        txtReadPageCount->setAlignment(Qt::AlignRight);
        txtReadPageCount->setProperty("name", QVariant(QByteArray("txtReadPageCount")));
        label3 = new QLabel(groupBox);
        label3->setObjectName(QStringLiteral("label3"));
        label3->setGeometry(QRect(370, 10, 100, 20));
        label3->setProperty("name", QVariant(QByteArray("txtLabelSectorCount")));
        label4 = new QLabel(groupBox);
        label4->setObjectName(QStringLiteral("label4"));
        label4->setGeometry(QRect(10, 60, 56, 20));
        label4->setProperty("name", QVariant(QByteArray("txtLabelRead")));
        txtReadCmdCount = new QLineEdit(groupBox);
        txtReadCmdCount->setObjectName(QStringLiteral("txtReadCmdCount"));
        txtReadCmdCount->setGeometry(QRect(110, 60, 89, 22));
        txtReadCmdCount->setAlignment(Qt::AlignRight);
        txtReadCmdCount->setProperty("name", QVariant(QByteArray("txtReadCmdCount")));
        groupBox1 = new QGroupBox(MonitorForm);
        groupBox1->setObjectName(QStringLiteral("groupBox1"));
        groupBox1->setGeometry(QRect(20, 120, 461, 80));
        groupBox1->setStyleSheet(QLatin1String("QGroupBox {\n"
"    border: 1px solid gray;\n"
"    border-radius: 9px;\n"
"    margin-top: 0.5em;\n"
"	font-weight : bold;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 3px 0 3px;\n"
"}"));
        groupBox1->setProperty("name", QVariant(QByteArray("groupBox2")));
        txtGCCount = new QLineEdit(groupBox1);
        txtGCCount->setObjectName(QStringLiteral("txtGCCount"));
        txtGCCount->setGeometry(QRect(110, 40, 89, 22));
        txtGCCount->setAlignment(Qt::AlignRight);
        txtGCCount->setProperty("name", QVariant(QByteArray("txtGCCount")));
        txtWriteAmp = new QLineEdit(groupBox1);
        txtWriteAmp->setObjectName(QStringLiteral("txtWriteAmp"));
        txtWriteAmp->setGeometry(QRect(240, 40, 89, 22));
        txtWriteAmp->setAlignment(Qt::AlignRight);
        txtWriteAmp->setProperty("name", QVariant(QByteArray("txtWriteAmp")));
        txtGCErase = new QLineEdit(groupBox1);
        txtGCErase->setObjectName(QStringLiteral("txtGCErase"));
        txtGCErase->setGeometry(QRect(360, 40, 89, 22));
        txtGCErase->setAlignment(Qt::AlignRight);
        txtGCErase->setProperty("name", QVariant(QByteArray("txtGCErase")));
        label5 = new QLabel(groupBox1);
        label5->setObjectName(QStringLiteral("label5"));
        label5->setGeometry(QRect(130, 20, 66, 20));
        label5->setProperty("name", QVariant(QByteArray("txtLabelGCCount")));
        label6 = new QLabel(groupBox1);
        label6->setObjectName(QStringLiteral("label6"));
        label6->setGeometry(QRect(240, 20, 100, 20));
        label6->setProperty("name", QVariant(QByteArray("txtLabelWriteAmp")));
        label7 = new QLabel(groupBox1);
        label7->setObjectName(QStringLiteral("label7"));
        label7->setGeometry(QRect(375, 20, 55, 20));
        label7->setProperty("name", QVariant(QByteArray("txtLabelGCSector")));
        groupBox2 = new QGroupBox(MonitorForm);
        groupBox2->setObjectName(QStringLiteral("groupBox2"));
        groupBox2->setGeometry(QRect(20, 210, 341, 70));
        groupBox2->setStyleSheet(QLatin1String("QGroupBox {\n"
"    border: 1px solid gray;\n"
"    border-radius: 9px;\n"
"    margin-top: 0.5em;\n"
"	font-weight : bold;\n"
"}\n"
"\n"
"QGroupBox::title {\n"
"    subcontrol-origin: margin;\n"
"    left: 10px;\n"
"    padding: 0 3px 0 3px;\n"
"}"));
        groupBox2->setProperty("name", QVariant(QByteArray("groupBox3")));
        label8 = new QLabel(groupBox2);
        label8->setObjectName(QStringLiteral("label8"));
        label8->setGeometry(QRect(130, 20, 56, 20));
        label8->setProperty("name", QVariant(QByteArray("txtLabelTrimCount")));
        txtTrimCount = new QLineEdit(groupBox2);
        txtTrimCount->setObjectName(QStringLiteral("txtTrimCount"));
        txtTrimCount->setGeometry(QRect(110, 40, 89, 22));
        txtTrimCount->setAlignment(Qt::AlignRight);
        txtTrimCount->setProperty("name", QVariant(QByteArray("txtTrimCount")));
        txtTrimEffect = new QLineEdit(groupBox2);
        txtTrimEffect->setObjectName(QStringLiteral("txtTrimEffect"));
        txtTrimEffect->setGeometry(QRect(240, 40, 89, 22));
        txtTrimEffect->setAlignment(Qt::AlignRight);
        txtTrimEffect->setProperty("name", QVariant(QByteArray("txtTrimEffect")));
        label9 = new QLabel(groupBox2);
        label9->setObjectName(QStringLiteral("label9"));
        label9->setGeometry(QRect(250, 20, 100, 20));
        label9->setProperty("name", QVariant(QByteArray("txtLabelTrimEffect")));
        txtSSDUtil = new QLineEdit(MonitorForm);
        txtSSDUtil->setObjectName(QStringLiteral("txtSSDUtil"));
        txtSSDUtil->setGeometry(QRect(380, 300, 89, 22));
        txtSSDUtil->setAlignment(Qt::AlignRight);
        txtSSDUtil->setProperty("name", QVariant(QByteArray("txtSSDUtil")));
        label10 = new QLabel(MonitorForm);
        label10->setObjectName(QStringLiteral("label10"));
        label10->setGeometry(QRect(270, 300, 111, 20));
        label10->setProperty("name", QVariant(QByteArray("txtLabelSSDUtil")));
        txtDebug = new QTextEdit(MonitorForm);
        txtDebug->setObjectName(QStringLiteral("txtDebug"));
        txtDebug->setGeometry(QRect(20, 360, 460, 80));
        txtDebug->setMouseTracking(false);
        txtDebug->setProperty("name", QVariant(QByteArray("txtDebug")));
        label11 = new QLabel(MonitorForm);
        label11->setObjectName(QStringLiteral("label11"));
        label11->setGeometry(QRect(30, 340, 120, 20));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label11->setFont(font);
        label11->setProperty("name", QVariant(QByteArray("txtLabelDebug")));
        txtTime = new QLineEdit(MonitorForm);
        txtTime->setObjectName(QStringLiteral("txtTime"));
        txtTime->setGeometry(QRect(130, 300, 89, 22));
        txtTime->setAlignment(Qt::AlignRight);
        txtTime->setProperty("name", QVariant(QByteArray("txtTime")));
        label12 = new QLabel(MonitorForm);
        label12->setObjectName(QStringLiteral("label12"));
        label12->setGeometry(QRect(30, 300, 50, 20));
        label12->setProperty("name", QVariant(QByteArray("txtLabelTime")));
        btnSave = new QPushButton(MonitorForm);
        btnSave->setObjectName(QStringLiteral("btnSave"));
        btnSave->setGeometry(QRect(490, 90, 100, 50));
        btnSave->setProperty("name", QVariant(QByteArray("btnSave")));

        retranslateUi(MonitorForm);

        QMetaObject::connectSlotsByName(MonitorForm);
    } // setupUi

    void retranslateUi(QDialog *MonitorForm)
    {
        MonitorForm->setProperty("caption", QVariant(QApplication::translate("MonitorForm", "SSD Monitor", Q_NULLPTR)));
        btnReset->setText(QApplication::translate("MonitorForm", "Reset", Q_NULLPTR));
        groupBox->setTitle(QString());
        label->setText(QApplication::translate("MonitorForm", "Write:", Q_NULLPTR));
        txtWriteCmdCount->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        label1->setText(QApplication::translate("MonitorForm", "Cmd Count", Q_NULLPTR));
        label2->setText(QApplication::translate("MonitorForm", "Speed [MB/s]", Q_NULLPTR));
        txtWriteSpeed->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        txtReadSpeed->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        txtWritePageCount->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        txtReadPageCount->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        label3->setText(QApplication::translate("MonitorForm", "Page Count", Q_NULLPTR));
        label4->setText(QApplication::translate("MonitorForm", "Read:", Q_NULLPTR));
        txtReadCmdCount->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        groupBox1->setTitle(QApplication::translate("MonitorForm", "Garbage Collection", Q_NULLPTR));
        txtGCCount->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        txtWriteAmp->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        txtGCErase->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        label5->setText(QApplication::translate("MonitorForm", "Call Count", Q_NULLPTR));
        label6->setText(QApplication::translate("MonitorForm", "Write Amplification", Q_NULLPTR));
        label7->setText(QApplication::translate("MonitorForm", "Block Erase", Q_NULLPTR));
        groupBox2->setTitle(QApplication::translate("MonitorForm", "TRIM", Q_NULLPTR));
        label8->setText(QApplication::translate("MonitorForm", "Call Count", Q_NULLPTR));
        txtTrimCount->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        txtTrimEffect->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        label9->setText(QApplication::translate("MonitorForm", "Trimmed LBAs", Q_NULLPTR));
        txtSSDUtil->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        label10->setText(QApplication::translate("MonitorForm", "SSD Utilization", Q_NULLPTR));
        label11->setText(QApplication::translate("MonitorForm", "Debug Status", Q_NULLPTR));
        txtTime->setText(QApplication::translate("MonitorForm", "0", Q_NULLPTR));
        label12->setText(QApplication::translate("MonitorForm", "Time", Q_NULLPTR));
        btnSave->setText(QApplication::translate("MonitorForm", "Save to File", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class MonitorForm: public Ui_MonitorForm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MONITORFORM_H
