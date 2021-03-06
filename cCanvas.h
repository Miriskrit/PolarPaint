#ifndef CCANVAS_H
#define CCANVAS_H

#include <QObject>
#include <QtQuick>
#include <QDate>

class QPainter;


class cCanvas : public QQuickPaintedItem
{
    Q_OBJECT
    Q_PROPERTY(int brushSize MEMBER m_brushSize NOTIFY brushSizeChanged)
    Q_PROPERTY(QString brushColor MEMBER m_brushColor NOTIFY brushColorChanged)
    Q_PROPERTY(QPoint previousPoint MEMBER m_previuosPoint NOTIFY PreviousPointChanged)
    Q_PROPERTY(QString bgColor MEMBER m_bgColor NOTIFY bgColorChanged)
    Q_PROPERTY(double scalingFactor MEMBER m_scale NOTIFY scaleChanged)
    Q_PROPERTY(bool moveMod MEMBER m_isMoveMod NOTIFY isMoveChanged)
    Q_PROPERTY(double pixelRatio MEMBER m_PixelRatio NOTIFY pixelRatioChanged)
    Q_PROPERTY(bool isSaveWithBg MEMBER m_isSaveBg NOTIFY isBgSaveChanged)

    Q_PROPERTY(bool symmetry READ symmetry WRITE setSymmetry NOTIFY symmetryChanged)
    Q_PROPERTY(int axes READ axes WRITE setAxes NOTIFY axesNumChanged)
    Q_PROPERTY(bool isDrawAxes READ isDrawAxes WRITE setDrawAxes NOTIFY isDrawAxesChanged)
    Q_PROPERTY(int axesOpacity READ axesOpacity WRITE setAxesOpacity NOTIFY axesOpacityChanged)
    Q_PROPERTY(int cvsSize READ cvsSize WRITE setCvsSize NOTIFY cvsSizeChanged)

private:
    /* ---------PROPERTIES--------- */
    int m_brushSize;
    QString m_brushColor;
    bool m_isSymmetry;
    int m_nAxes;
    QPoint m_previuosPoint;
    QString m_bgColor;
    double m_scale;
    bool m_isMoveMod;
    double m_PixelRatio;
    bool m_isSaveBg;
    bool m_isDrawAxes;
    int m_axesOpacity;

    /* ---------MAIN CANVAS--------- */
    QImage *m_cvs;
    int m_cvsSize;
    QPoint m_offset;

    /* ---------REDO UNDO--------- */
    QImage *m_savedCvs;
    int m_maxNumSavedLines;
    struct LineObj
    {
        QList<QList<QPoint>> path;
        int size;
        QString color;
        int nAxes{0};
        bool symmetry{false};
    };
    QList <LineObj> m_savedLines;
    QStack <LineObj> m_deletedLines;
    void m_redrawCvs();

    /* ---------DRAWING--------- */
    void m_drawLine(const LineObj &line, QImage *cvs);
    void m_drawPoints(const QList<QPoint> &points, QImage *cvs);
    QPointF m_getPolarCoords(QPoint coords);

    /* ---------SCALING--------- */
    QPoint m_scalingCenterPos;

public:
    explicit cCanvas(QQuickItem *pqi = nullptr);
    virtual ~cCanvas();

    Q_INVOKABLE void clear();
    Q_INVOKABLE void redo();
    Q_INVOKABLE void undo();

    Q_INVOKABLE void move(const QPoint& path);
    Q_INVOKABLE void moveCenter();
    Q_INVOKABLE void changeScaleWithCentering(double scaleChange);

    Q_INVOKABLE bool save();

    Q_INVOKABLE QPoint getLocalPosFromReal(const QPoint& realPos);
    Q_INVOKABLE QPoint getRealPosFromLocal(const QPoint& localPos);
    Q_INVOKABLE void centeringBy(const QPoint& localPos);
    Q_INVOKABLE void moveScalingCenterTo(const QPoint& localPos, double step);
    Q_INVOKABLE void setTempCenterPos();
    Q_INVOKABLE void startLine();
    Q_INVOKABLE void continueLine(const QList<QPoint> &points);

    Q_INVOKABLE bool symmetry();
    Q_INVOKABLE void setSymmetry(bool symmetry);
    Q_INVOKABLE int axes();
    Q_INVOKABLE void setAxes(int nAxes);
    Q_INVOKABLE bool isDrawAxes();
    Q_INVOKABLE void setDrawAxes(bool draw);
    Q_INVOKABLE int axesOpacity();
    Q_INVOKABLE void setAxesOpacity(int opacity);
    Q_INVOKABLE int cvsSize();
    Q_INVOKABLE void setCvsSize(int size);

    void paint(QPainter *ppainter) override;

signals:
    void brushSizeChanged(int brushSize);
    void symmetryChanged(bool symmetry);
    void axesNumChanged(int nAxes);
    void brushColorChanged(const QString &color);
    void PreviousPointChanged(const QPoint& point);
    void bgColorChanged(const QString &color);
    void scaleChanged(double scale);
    void isMoveChanged(bool x);
    void pixelRatioChanged(double ratio);
    void isBgSaveChanged(bool x);
    void isDrawAxesChanged(bool x);
    void axesOpacityChanged(int value);
    void cvsSizeChanged(int size);
};

#endif // CCANVAS_H

