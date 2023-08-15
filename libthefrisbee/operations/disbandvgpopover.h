#ifndef DISBANDVGPOPOVER_H
#define DISBANDVGPOPOVER_H

#include <QWidget>

namespace Ui {
    class DisbandVgPopover;
}

class VolumeGroup;
struct DisbandVgPopoverPrivate;
class DisbandVgPopover : public QWidget {
        Q_OBJECT

    public:
        explicit DisbandVgPopover(VolumeGroup* vg, QWidget* parent = nullptr);
        ~DisbandVgPopover();

    signals:
        void done();
        void deleted();

    private slots:
        void on_disbandButton_clicked();

        void on_titleLabel_backButtonClicked();

    private:
        Ui::DisbandVgPopover* ui;
        DisbandVgPopoverPrivate* d;
};

#endif // DISBANDVGPOPOVER_H
