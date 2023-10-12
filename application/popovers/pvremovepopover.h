#ifndef PVREMOVEPOPOVER_H
#define PVREMOVEPOPOVER_H

#include <QCoroTask>
#include <QWidget>

namespace Ui {
    class PvRemovePopover;
}

class DiskObject;
struct PvRemovePopoverPrivate;
class PvRemovePopover : public QWidget {
        Q_OBJECT

    public:
        explicit PvRemovePopover(DiskObject* disk, QWidget* parent = nullptr);
        ~PvRemovePopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        QCoro::Task<> on_doRelocateButton_clicked();

        void on_titleLabel_2_backButtonClicked();

        void on_relocateButton_clicked();

        void on_titleLabel_3_backButtonClicked();

        void on_removeButton_2_clicked();

        void on_titleLabel_4_backButtonClicked();

        QCoro::Task<> on_doRemoveButton_clicked();

        void on_removeButton_clicked();

    private:
        Ui::PvRemovePopover* ui;
        PvRemovePopoverPrivate* d;
};

#endif // PVREMOVEPOPOVER_H
