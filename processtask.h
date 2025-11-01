#ifndef PROCESSTASK_H
#define PROCESSTASK_H

#include <QObject>

class ProcessTask : public QObject
{
    Q_OBJECT
public:
    explicit ProcessTask(QObject *parent = nullptr);

signals:
};

#endif // PROCESSTASK_H
