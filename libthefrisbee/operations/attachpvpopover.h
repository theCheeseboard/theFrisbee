#ifndef ATTACHPVPOPOVER_H
#define ATTACHPVPOPOVER_H

#include <QWidget>

namespace Ui {
    class AttachPvPopover;
}

class DiskObject;
struct AttachPvPopoverPrivate;
class AttachPvPopover : public QWidget {
        Q_OBJECT

    public:
        explicit AttachPvPopover(DiskObject* disk, QWidget* parent = nullptr);
        ~AttachPvPopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_vgList_activated(const QModelIndex& index);

        void on_titleLabel_2_backButtonClicked();

        void on_attachToNewVgButton_clicked();

    private:
        Ui::AttachPvPopover* ui;
        AttachPvPopoverPrivate* d;
};

#endif // ATTACHPVPOPOVER_H
