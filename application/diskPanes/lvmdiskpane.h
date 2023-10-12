#ifndef LVMDISKPANE_H
#define LVMDISKPANE_H

#include "diskpanecomponent.h"

namespace Ui {
    class LvmDiskPane;
}

struct LvmDiskPanePrivate;
class DiskObject;
class LvmDiskPane : public DiskPaneComponent {
        Q_OBJECT

    public:
        explicit LvmDiskPane(DiskObject* disk, QWidget* parent = nullptr);
        ~LvmDiskPane();

    private:
        Ui::LvmDiskPane* ui;
        LvmDiskPanePrivate* d;

        void updateDetails();

        // DiskPaneComponent interface
    public:
        int order() const;
    private slots:
        void on_attachVgButton_clicked();
        void on_pvSettingsButton_clicked();
};

#endif // LVMDISKPANE_H
