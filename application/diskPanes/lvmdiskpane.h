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

        // DiskPaneComponent interface
    public:
        int order() const;
};

#endif // LVMDISKPANE_H
