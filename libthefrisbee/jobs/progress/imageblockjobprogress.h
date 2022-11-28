#ifndef IMAGEBLOCKJOBPROGRESS_H
#define IMAGEBLOCKJOBPROGRESS_H

#include <QWidget>

namespace Ui {
class ImageBlockJobProgress;
}

struct ImageBlockJobProgressPrivate;
class ImageBlockJob;
class ImageBlockJobProgress : public QWidget
{
    Q_OBJECT

public:
    explicit ImageBlockJobProgress(ImageBlockJob* job, QWidget *parent = nullptr);
    ~ImageBlockJobProgress();

private:
    Ui::ImageBlockJobProgress *ui;
    ImageBlockJobProgressPrivate* d;

    void updateState();
};

#endif // IMAGEBLOCKJOBPROGRESS_H
