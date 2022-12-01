#ifndef IMAGEBLOCKJOB_H
#define IMAGEBLOCKJOB_H

#include <tjob.h>
#include <QObject>
#include <QCoroTask>

struct ImageBlockJobPrivate;
class DiskObject;
class ImageBlockJob : public tJob
{
    Q_OBJECT
public:
    explicit ImageBlockJob(DiskObject* disk, QObject *parent = nullptr);
    ~ImageBlockJob();

    QCoro::Task<> startImage(QIODevice* dest);
    void cancel();

    QString description();
    QString displayName();

signals:
    void descriptionChanged(QString description);

private:
    ImageBlockJobPrivate* d;

    // tJob interface
public:
    quint64 progress();
    quint64 totalProgress();
    State state();
    QWidget *makeProgressWidget();
};

#endif // IMAGEBLOCKJOB_H
