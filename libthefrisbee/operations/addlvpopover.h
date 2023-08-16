#ifndef ADDLVPOPOVER_H
#define ADDLVPOPOVER_H

#include <QCoroTask>
#include <QWidget>

namespace Ui {
    class AddLvPopover;
}

class VolumeGroup;
struct AddLvPopoverPrivate;
class AddLvPopover : public QWidget {
        Q_OBJECT

    public:
        explicit AddLvPopover(VolumeGroup* vg, QWidget* parent = nullptr);
        ~AddLvPopover();

    signals:
        void done();

    private slots:
        QCoro::Task<> on_createButton_clicked();

        void on_titleLabel_backButtonClicked();

    private:
        Ui::AddLvPopover* ui;
        AddLvPopoverPrivate* d;
};

#endif // ADDLVPOPOVER_H
