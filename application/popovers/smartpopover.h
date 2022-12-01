#ifndef SMARTPOPOVER_H
#define SMARTPOPOVER_H

#include <QCoroTask>
#include <QWidget>

namespace Ui {
    class SmartPopover;
}

class AtaDriveInterface;
struct SmartPopoverPrivate;
class SmartPopover : public QWidget {
        Q_OBJECT

    public:
        explicit SmartPopover(AtaDriveInterface* ataInterface, QWidget* parent = nullptr);
        ~SmartPopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_enableSmartButton_clicked();

        void on_disableSmartButton_clicked();

        void on_titleLabel_2_backButtonClicked();

        QCoro::Task<> on_doStartSelfTestButton_clicked();

        void on_startSmartSelfTestButton_clicked();

        void on_abortSmartSelfTestButton_clicked();

    private:
        Ui::SmartPopover* ui;
        SmartPopoverPrivate* d;

        QCoro::Task<> updateSmartStatus();
};

#endif // SMARTPOPOVER_H
