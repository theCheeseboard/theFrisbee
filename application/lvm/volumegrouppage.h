#ifndef VOLUMEGROUPPAGE_H
#define VOLUMEGROUPPAGE_H

#include <QWidget>

namespace Ui {
    class VolumeGroupPage;
}

class VolumeGroup;
struct VolumeGroupPagePrivate;
class VolumeGroupPage : public QWidget {
        Q_OBJECT

    public:
        explicit VolumeGroupPage(VolumeGroup* vg, QWidget* parent = nullptr);
        ~VolumeGroupPage();

        void setTopPadding(int padding);

    signals:
        void done();

    private slots:
        void on_titleLabel_backButtonClicked();

        void on_disbandButton_clicked();

    private:
        Ui::VolumeGroupPage* ui;
        VolumeGroupPagePrivate* d;
};

#endif // VOLUMEGROUPPAGE_H
