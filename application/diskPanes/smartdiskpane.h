#ifndef SMARTDISKPANE_H
#define SMARTDISKPANE_H

#include "diskpanecomponent.h"
#include <QCoroTask>

namespace Ui {
    class SmartDiskPane;
}

struct SmartDiskPanePrivate;
class DiskObject;
class SmartDiskPane : public DiskPaneComponent {
        Q_OBJECT

    public:
        explicit SmartDiskPane(DiskObject* disk, QWidget* parent = nullptr);
        ~SmartDiskPane();

    private:
        Ui::SmartDiskPane* ui;
        SmartDiskPanePrivate* d;

        void updateSmartInfo();

        // DiskPaneComponent interface
    public:
        int order() const;
    private slots:
        void on_manageSmartButton_clicked();
};

#endif // SMARTDISKPANE_H
