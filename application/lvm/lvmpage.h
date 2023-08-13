#ifndef LVMPAGE_H
#define LVMPAGE_H

#include <QWidget>

namespace Ui {
    class LvmPage;
}

struct LvmPagePrivate;
class LvmPage : public QWidget {
        Q_OBJECT

    public:
        explicit LvmPage(QWidget* parent = nullptr);
        ~LvmPage();

        void setTopPadding(int padding);

    private slots:
        void on_listView_clicked(const QModelIndex& index);

    private:
        Ui::LvmPage* ui;
        LvmPagePrivate* d;
};

#endif // LVMPAGE_H
