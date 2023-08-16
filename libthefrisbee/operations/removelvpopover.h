#ifndef REMOVELVPOPOVER_H
#define REMOVELVPOPOVER_H

#include <QWidget>

namespace Ui {
    class RemoveLvPopover;
}

class LogicalVolume;
struct RemoveLvPopoverPrivate;
class RemoveLvPopover : public QWidget {
        Q_OBJECT

    public:
        explicit RemoveLvPopover(LogicalVolume* lv, QWidget* parent = nullptr);
        ~RemoveLvPopover();

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_deleteButton_clicked();

    private:
        Ui::RemoveLvPopover* ui;
        RemoveLvPopoverPrivate* d;
};

#endif // REMOVELVPOPOVER_H
