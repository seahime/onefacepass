﻿#include <QMutexLocker>

#include "facethread.h"

FaceThread* FaceThread::self = nullptr;

FaceThread::FaceThread()
{
    facedete = new FaceDete();

    facedete->SetConfLevel(static_cast<MFloat>(0.8));

    qRegisterMetaType<QVector<QRect> >("QVector<QRect>");
    qRegisterMetaType<QVector<Student> >("QVector<Student>");
}

void FaceThread::SetPreloadPath(const QString &path)
{
    facedete->SetPreloadPath(path.toStdString());

    std::string errmsg;
    if (facedete->Loadregface(errmsg) < 0) {
        // 这个errmsg 参数应该保留在 Loadregface 内部，而不是暴露出来
        // FIXME: 没做完整的处理，出现问题记得检查
        qDebug() << "\033[31m" << "FaceThread | facedete->Loadregface() < 0" << "\033[0m";
    }

//    if (!errmsg.empty()) {
//        // 这里的处理应该移回到 FaceDete 库，而不应该把问题暴露给调用者
//        std::cerr << errmsg << "\n";
//    }
}

FaceThread* FaceThread::Instance()
{
    static QMutex mutex;

    if (!self) {
        QMutexLocker locker(&mutex);

        if (!self) {
            self = new FaceThread();
        }
    }

    return self;
}

FaceThread::~FaceThread()
{
    tasks.clear();
}

void FaceThread::run()
{
    while(!isInterruptionRequested()) {

        while (!tasks.empty()) {

#ifdef DEBUG_FACE
            qDebug() << "FaceThread | detecting...";
#endif
            { // lock begin
                QMutexLocker locker(&lock);

                t = tasks.dequeue();    // t: <图片 QImage, 是否进行检测 bool>
                if (tasks.size() > 5) {
                    // 积累的任务多了就扔吧
                    tasks.clear();
                }
#ifdef DEBUG_FACE
                qDebug() << "取到任务，"
                         << "剩余：" << tasks.size();
#endif
            } // lock end

            cv::Mat mat_tmp = cv::Mat(t.first.height(), t.first.width(), CV_8UC4, const_cast<uchar*>(t.first.bits()), t.first.bytesPerLine());
            cv::Mat mat = cv::Mat(mat_tmp.rows, mat_tmp.cols, CV_8UC3 );
            int from_to[] = { 0,0, 1,1, 2,2 };
            cv::mixChannels( &mat_tmp, 1, &mat, 1, from_to, 3 );


            if (facedete->DetectFaces(mat, detectedResult)) {
                qDebug() << "FaceThread | something is wrong while detecting";
            }

#ifdef DEBUG_FACE
        qDebug() << "FaceThread | detection finished";
#endif

            if (detectedResult.empty()) {
#ifdef DEBUG_FACE
        qDebug() << "\033[31m" << "FaceThread | detectedResult empty! "
                 << detectedResult.size() << "\033[0m";
#endif
                continue;
            }

            for (unsigned int i = 0; i < detectedResult.size(); i ++) {
                Json::Value currRes = detectedResult[std::to_string(i)];

                if (t.second) {   //--- 人脸识别
                    QString id(currRes["id"].asString().data());
                    QString name(currRes["name"].asString().data());
                    QString major(currRes["major"].asString().data());
                    bool identifiable = currRes["identifiable"].asBool();
                    QString path(currRes["pathInPreload"].asString().data());

                    // 检测模式下返回完整的人脸识别信息
                    resultComplete.push_back({identifiable,
                                              id,
                                              name,
                                              major,
                                              QRect(currRes["rect"][0].asInt(), currRes["rect"][1].asInt(),
                                                    currRes["rect"][2].asInt()-currRes["rect"][0].asInt(), currRes["rect"][3].asInt()-currRes["rect"][1].asInt()),
                                              path});
                } else {    //--- 仅人脸跟踪
                    // 跟踪模式下只返回人脸位置
                    resultOnlyTrack.push_back(QRect(currRes["rect"][0].asInt(), currRes["rect"][1].asInt(),
                            currRes["rect"][2].asInt()-currRes["rect"][0].asInt(), currRes["rect"][3].asInt()-currRes["rect"][1].asInt()));
                }
            }


            emit t.second ? DetectFinished(resultComplete) : TrackFinished(resultOnlyTrack);

            detectedResult.clear();
            resultComplete.clear();
            resultOnlyTrack.clear();

        }

    }
}

void FaceThread::ReceiveImg(bool _detect, const QImage& image)
{
    QMutexLocker locker(&lock);

    tasks.enqueue({image, _detect});
}
