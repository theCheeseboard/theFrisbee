#ifndef SIZERANGEBOX_H
#define SIZERANGEBOX_H

#include <QWidget>

namespace Ui {
    class SizeRangeBox;
}

struct SizeRangeBoxPrivate;
class SizeRangeBox : public QWidget {
        Q_OBJECT

    public:
        explicit SizeRangeBox(QWidget* parent = nullptr);
        ~SizeRangeBox();

        void setMaximumSize(quint64 size);

        quint64 size();
        void setSize(quint64 size);

    private slots:
        void on_slider_valueChanged(int value);

        void on_sizeBox_editingFinished();

    private:
        Ui::SizeRangeBox* ui;
        SizeRangeBoxPrivate* d;
};

#endif // SIZERANGEBOX_H
